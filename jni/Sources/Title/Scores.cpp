#include "Scores.h"

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Storage/Storage.h>
#include <libeng/Tools/Tools.h>
#include <ctime>

#ifdef __ANDROID__
#include "Game/PanelCoords.h"
#else
#include "PanelCoords.h"
#endif

#define DIGIT_COUNT(num)    ((num < 10)? 1:((num < 100)? 2:((num < 1000)? 3:(num < 10000)? 4:5)))

#define SCORE_Y             (0.5f - (((SCORE_Y_POS - PANEL_Y0) * 2.f) / PANEL_TEXTURE_SIZE))
#define BEST_Y              (0.5f - (((BEST_Y_POS - PANEL_Y0) * 2.f) / PANEL_TEXTURE_SIZE))
#define DISP_SCORE_DELAY    7
#define FONT_SCALE_RATIO    1200.f // 0.5 with a width of 600 pixel (Samsung Galaxy Tab 2)

// First score position in the scores texture (in pixel)
#define FIRST_SCORE_X       49
#define FIRST_SCORE_Y       116

// Data key(s)
#define DATA_KEY_MONTH      "Month"
#define DATA_KEY_DAY        "Day"
#define DATA_KEY_YEAR       "Year"
#define DATA_KEY_SCORE      "Score"

Scores* Scores::mThis = NULL;

//////
Scores::Scores(StoreData* data) : mData(data), mCurScore(0), mBestScore(0), mDispTitle(true), mScore(0),
        mDispWidth(0.f), mCalculate(false), mNewBest(false) {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - d:%x"), __PRETTY_FUNCTION__, __LINE__, data);
    assert(data);

    std::memset(mScores, 0, sizeof(Text2D*) * MAX_SCORE_COUNT);
    std::memset(mMedals, 0, sizeof(Static2D*) * MAX_SCORE_COUNT);

    mDispScore = NULL;
    mDispBest = NULL;
    mDispMedal = NULL;

    mNewA = NULL;
    mNewB = NULL;

#ifdef DEBUG
    mLog = NULL;
#endif
}
Scores::~Scores() {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

        if (mScores[i]) delete mScores[i];
        if (mMedals[i]) delete mMedals[i];
    }
    if (mDispScore) delete mDispScore;
    if (mDispBest) delete mDispBest;
    if (mDispMedal) delete mDispMedal;
}

std::wstring Scores::getScore(unsigned char scoreIdx) const {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - s:%d"), __PRETTY_FUNCTION__, __LINE__, scoreIdx);
    assert(scoreIdx < MAX_SCORE_COUNT);

    // Rank
    std::wstring score;
    if (scoreIdx != 9)
        score.append(1, L'0');
    score.append(numToWStr<unsigned char>(scoreIdx + 1));
    score.append(1, L'-');

    // Month
    std::string field(DATA_KEY_MONTH);
    field.append(1, (scoreIdx | 0x30)); // "| 0x30" to ASCII code
    std::wstring data(numToWStr<unsigned char>(*static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()))));
    if (data.size() == 1) score.append(L"0");
    score.append(data);
    score.append(L"/");

    // Day
    field.assign(DATA_KEY_DAY);
    field.append(1, (scoreIdx | 0x30));
    data.assign(numToWStr<unsigned char>(*static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()))));
    if (data.size() == 1) score.append(L"0");
    score.append(data);
    score.append(L"/");

    // Year
    field.assign(DATA_KEY_YEAR);
    field.append(1, (scoreIdx | 0x30));
    data.assign(numToWStr<unsigned char>(*static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()))));
    if (data.size() == 1) score.append(L"0");
    score.append(data);

    // Score
    field.assign(DATA_KEY_SCORE);
    field.append(1, (scoreIdx | 0x30));

    mCurScore = *static_cast<short*>(mData->get(DATA_SHORT, field.c_str()));
    score.append(6 - DIGIT_COUNT(mCurScore), L' ');
    score.append(numToWStr<short>(mCurScore));

    return score;
}

void Scores::load(const Game* game, unsigned char step) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - g:%x; s:%d"), __PRETTY_FUNCTION__, __LINE__, game, step);
    mLog = game->getLog();
#endif
    switch (step) {
        case 0: { // Title scores

            for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

                if (!mScores[i]) {

                    mScores[i] = new Text2D(1);
                    mScores[i]->initialize(game2DVia(game));
                    mScores[i]->start(getScore(i));

                    float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                    mScores[i]->scale(scale, scale);
                    mScores[i]->position(((FIRST_SCORE_X << 1) / SCORES_SIZE) - 1.f, 1.f - ((FIRST_SCORE_Y << 1) / SCORES_SIZE) -
                            ((i)? ((mScores[i - 1]->getTop() - mScores[i - 1]->getBottom()) * 1.25f * i):0.f));

                    if (!i) // First score is best score
                        mBestScore = mCurScore;
                }
                else
                    mScores[i]->resume();

                if (!mMedals[i]) {

                    mMedals[i] = new Static2D;
                    mMedals[i]->initialize(game2DVia(game));
                    mMedals[i]->start(TEXTURE_ID_PANEL);

                    static const float texCoords[8] = { 0.f, 0.f, 0.f, MEDAL_SIZE / PANEL_TEXTURE_SIZE,
                            MEDAL_SIZE / PANEL_TEXTURE_SIZE, MEDAL_SIZE / PANEL_TEXTURE_SIZE, MEDAL_SIZE / PANEL_TEXTURE_SIZE, 0.f };
                    mMedals[i]->setTexCoords(texCoords);

                    float verCoords[8];
                    verCoords[0] = mScores[i]->getLeft() + (12.f * ((mScores[i]->getRight() - mScores[i]->getLeft()) / 17.f));
                    verCoords[1] = mScores[i]->getTop();                            // Put medal after the 12th letter in code above
                    verCoords[2] = verCoords[0];                                    // -> "/ 17.f" == sizeof("99-99/99/99   999")
                    verCoords[3] = mScores[i]->getBottom();
                    verCoords[4] = verCoords[0] + (verCoords[1] - verCoords[3]);
                    verCoords[5] = verCoords[3];
                    verCoords[6] = verCoords[4];
                    verCoords[7] = verCoords[1];
                    mMedals[i]->setVertices(verCoords);

                    setMedal(static_cast<Panel2D*>(mMedals[i]), mCurScore);
                }
                else
                    mMedals[i]->resume(TEXTURE_ID_PANEL);
            }
            if (mDispTitle)
                hide();
            break;
        }
        case 1: { // Score

            if (!mDispScore) {

                mDispScore = new Text2D(1);
                mDispScore->initialize(game2DVia(game));
                mDispScore->start(L"0");

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mDispScore->scale(scale, scale);
                mDispScore->position(((SCORE_X_POS << 1) / PANEL_TEXTURE_SIZE) - 1.f, SCORE_Y);

                mDispWidth = mDispScore->getRight() - mDispScore->getLeft();
            }
            else
                mDispScore->resume();
            break;
        }
        case 2: { // Best

            if (!mDispBest) {

                mDispBest = new Text2D(1);
                mDispBest->initialize(game2DVia(game));
                mDispBest->start(numToWStr<short>(mBestScore));

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mDispBest->scale(scale, scale);
                mDispBest->position((((BEST_X_POS << 1) / PANEL_TEXTURE_SIZE) - 1.f) - (mDispWidth * (DIGIT_COUNT(mBestScore) - 1)),
                        BEST_Y);
            }
            else
                mDispBest->resume();
            break;
        }
        case 3: { // Medal

            if (!mDispMedal) {

                mDispMedal = new Static2D;
                mDispMedal->initialize(game2DVia(game));
                mDispMedal->start(TEXTURE_ID_PANEL);

                static const float texCoords[8] = { 0.f, 0.f, 0.f, MEDAL_SIZE / PANEL_TEXTURE_SIZE,
                        MEDAL_SIZE / PANEL_TEXTURE_SIZE, MEDAL_SIZE / PANEL_TEXTURE_SIZE, MEDAL_SIZE / PANEL_TEXTURE_SIZE, 0.f };
                mDispMedal->setTexCoords(texCoords);

                short medalSize = (MEDAL_SIZE_POS * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE);
                short medalPosX = (MEDAL_X_POS * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE);
                short medalPosY = ((MEDAL_Y_POS - PANEL_Y0) * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE);
                mDispMedal->setVertices(medalPosX, (game->getScreen()->height >> 1) + (game->getScreen()->width >> 2) - medalPosY,
                        medalPosX + medalSize, (game->getScreen()->height >> 1) + (game->getScreen()->width >> 2) - medalSize -
                        medalPosY);
            }
            else
                mDispMedal->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 4: { // New background (NewA)

            if (!mNewA) {

                mNewA = new Static2D;
                mNewA->initialize(game2DVia(game));
                mNewA->start(0xff, 0x00, 0x00);
                mNewA->setTexCoords(FULL_TEXCOORD_BUFFER);

                short xPos = (NEW_A_X0 * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE);
                short yPos = (game->getScreen()->height >> 1) + (game->getScreen()->width >> 2) -
                        ((NEW_A_Y0 - PANEL_Y0) * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE);
                mNewA->setVertices(xPos, yPos, xPos + ((NEW_A_W * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE)),
                        yPos - ((NEW_A_H * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE)));
            }
            else
                mNewA->resume(0xff, 0x00, 0x00);
            break;
        }
        case 5: { // New text (NewB)

            if (!mNewB) {

                mNewB = new Static2D;
                mNewB->initialize(game2DVia(game));
                mNewB->start(TEXTURE_ID_FONT);
                mNewB->copyVertices(*mNewA);

                static const float texCoords[8] = { NEW_B_X0 / PANEL_TEXTURE_SIZE, NEW_B_Y0 / PANEL_TEXTURE_SIZE,
                        NEW_B_X0 / PANEL_TEXTURE_SIZE, NEW_B_Y2 / PANEL_TEXTURE_SIZE,
                        NEW_B_X2 / PANEL_TEXTURE_SIZE, NEW_B_Y2 / PANEL_TEXTURE_SIZE,
                        NEW_B_X2 / PANEL_TEXTURE_SIZE, NEW_B_Y0 / PANEL_TEXTURE_SIZE };
                mNewB->setTexCoords(texCoords);
            }
            else
                mNewB->resume(TEXTURE_ID_FONT);
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown load step: %d"), __PRETTY_FUNCTION__, __LINE__, step);
            assert(NULL);
            break;
        }
#endif
    }
}

void Scores::fill(unsigned char from) { // Fill 'mData' with empty data from 'from' index

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - f:%d"), __PRETTY_FUNCTION__, __LINE__, from);
    for (unsigned char i = from; i < MAX_SCORE_COUNT; ++i) {

        // Month
        std::string field(DATA_KEY_MONTH);
        field.append(1, (i | 0x30)); // "| 0x30" to ASCII code
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char(0)));

        // Day
        field.assign(DATA_KEY_DAY);
        field.append(1, (i | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char(0)));

        // Year
        field.assign(DATA_KEY_YEAR);
        field.append(1, (i | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char(0)));

        // Score
        field.assign(DATA_KEY_SCORE);
        field.append(1, (i | 0x30));
        mData->add(DATA_SHORT, field.c_str(), static_cast<void*>(new short(0)));
    }
}
void Scores::create() {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mData->clear();
    fill(); // Fill 'mData' with empty data (00/00/00 - 0)
}
void Scores::read() { // 'mData' -> 'mScoreList'

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

        ScoreData scoreData;

        // Month
        std::string field(DATA_KEY_MONTH);
        field.append(1, (i | 0x30)); // "| 0x30" to ASCII code
        scoreData.month = *static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()));
        if (!scoreData.month) // 00/00/00 - 0 ?
            break; // No more score (exit)

        // Day
        field.assign(DATA_KEY_DAY);
        field.append(1, (i | 0x30));
        scoreData.day = *static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()));

        // Year
        field.assign(DATA_KEY_YEAR);
        field.append(1, (i | 0x30));
        scoreData.year = *static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()));

        // Score
        field.assign(DATA_KEY_SCORE);
        field.append(1, (i | 0x30));
        scoreData.score = *static_cast<short*>(mData->get(DATA_SHORT, field.c_str()));

        mScoreList.push_back(scoreData);
    }
    // The list should be already sorted
}
void Scores::save() { // 'mScoreList' -> 'mData'

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    time_t curDate = time(0);
    struct tm* now = localtime(&curDate);

    ScoreData scoreData;
    scoreData.month = static_cast<unsigned char>(now->tm_mon + 1);
    scoreData.day = static_cast<unsigned char>(now->tm_mday);
    scoreData.year = static_cast<unsigned char>(now->tm_year + 1900 - 2000); // 20xx

    scoreData.score = mScore;

    mScoreList.push_back(scoreData);
    mScoreList.sort(); // Sort score list (after having added new score)

    // Remove last score if more than MAX_SCORE_COUNT in the score list
#ifdef DEBUG
    if (mScoreList.size() > MAX_SCORE_COUNT) {

        LOGI(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - Last score removed"), __PRETTY_FUNCTION__, __LINE__);
        mScoreList.pop_back();
    }
#else
    if (mScoreList.size() > MAX_SCORE_COUNT)
        mScoreList.pop_back();
#endif

    mData->clear();
    unsigned char scoreIdx = 0;
    for (std::list<ScoreData>::iterator iter = mScoreList.begin(); iter != mScoreList.end(); ++iter, ++scoreIdx) {

        assert(scoreIdx < MAX_SCORE_COUNT);

        // Month
        std::string field(DATA_KEY_MONTH);
        field.append(1, (scoreIdx | 0x30)); // "| 0x30" to ASCII code
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char((*iter).month)));

        // Day
        field.assign(DATA_KEY_DAY);
        field.append(1, (scoreIdx | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char((*iter).day)));

        // Year
        field.assign(DATA_KEY_YEAR);
        field.append(1, (scoreIdx | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char((*iter).year)));

        // Score
        field.assign(DATA_KEY_SCORE);
        field.append(1, (scoreIdx | 0x30));
        mData->add(DATA_SHORT, field.c_str(), static_cast<void*>(new short((*iter).score)));
    }
    fill(scoreIdx); // Needed if 'mScoreData' length < MAX_SCORE_COUNT
    if (!Storage::saveFile(g_GameFile, mData)) {

        LOGW(LOG_FORMAT(" - Failed to save data into '%s' file"), __PRETTY_FUNCTION__, __LINE__, g_GameFile);
        assert(NULL);
    }
}
bool Scores::update() {

#ifdef DEBUG
    if (mDispTitle) {
        LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - (d:true)"), __PRETTY_FUNCTION__, __LINE__);
    }
    else {
        LOGV(LOG_LEVEL_SCORES, (*mLog % 100), LOG_FORMAT(" - (d:false)"), __PRETTY_FUNCTION__, __LINE__);
    }
#endif
    if (mDispTitle) // Title scores
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            std::wstring score(getScore(i));
            if (mScores[i]->getText() != score) {
                mScores[i]->update(score);
                setMedal(static_cast<Panel2D*>(mMedals[i]), mCurScore);
            }
        }

    else { // Game score

        static short incScore = LIBENG_NO_DATA;
        if (mCalculate) {

            if (mDispScore->getText() != L"0") {

                mDispScore->position(mDispScore->getLeft() + ((mDispScore->getText().length() - 1) * mDispWidth), SCORE_Y);
                mDispScore->update(L"0");
            }
            mDispMedal->setRed(1.f);
            mDispMedal->setGreen(1.f);
            mDispMedal->setBlue(1.f);

            incScore = LIBENG_NO_DATA;
            mCalculate = false;
        }

        static unsigned char updateCount = 0;
        if (++updateCount != DISP_SCORE_DELAY)
            return (incScore != mScore);
        updateCount = 0;

        if (incScore == mScore)
            return false;

        unsigned char digitCount = DIGIT_COUNT(incScore);
        if (++incScore) {

            assert(Player::getInstance()->getIndex(SOUND_ID_SCORE) == 5);
            Player::getInstance()->play(5);

            setMedal(static_cast<Panel2D*>(mDispMedal), incScore);
            mDispScore->update(numToWStr<short>(incScore));
            if (DIGIT_COUNT(incScore) != digitCount)
                mDispScore->position(mDispScore->getLeft() - mDispWidth, SCORE_Y);

            if (incScore > mBestScore) {

                if (!mNewBest) {

                    mNewA->setAlpha(1.f);
                    mNewB->setAlpha(1.f);

                    mNewBest = true;
                }
                if (DIGIT_COUNT(incScore) != DIGIT_COUNT(mBestScore))
                    mDispBest->position(mDispBest->getLeft() - mDispWidth, BEST_Y);

                mBestScore = incScore;
                mDispBest->update(numToWStr<short>(incScore));
            }
        }
        if (incScore == mScore) {

            save(); // Save score into 'mData' & into file game
            return false;
        }
    }
    return true;
}

void Scores::render() const {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SCORES, (*mLog % 100), LOG_FORMAT(" - (d:%s)"), __PRETTY_FUNCTION__, __LINE__, (mDispTitle)? "true":"false");
    if (mDispTitle)
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            assert(mScores[i]);
            assert(mMedals[i]);
        }
#endif
    if (mDispTitle) // Title scores
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            mScores[i]->render(true);
            mMedals[i]->render(true);
        }

    else { // Game score

        mDispScore->render(true);
        mDispBest->render(true);
        mDispMedal->render(true);

        mNewA->render(true);
        mNewB->render(false);
    }
}
