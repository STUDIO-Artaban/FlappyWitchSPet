#include "TitleLevel.h"

#ifdef __ANDROID__
#include "Sprite/Witch.h"
#else
#include "Witch.h"
#endif
#include <libeng/Game/2D/Game2D.h>
#include <boost/thread.hpp>

#define TITLE_TEXTURE_WIDTH     512.f
#define TITLE_HEIGHT            256.f
#define TITLE_WIDTH             473 // Title width in the texture

// Face
#define FACE_X0                 345
#define FACE_Y0                 218
#define FACE_X2                 410
#define FACE_Y2                 283

#define OK_TEXTURE_LEFT         423
#define OK_TEXTURE_TOP          423
#define OK_HEIGHT               75

// Start & Score
#define CMD_VEL                 (CMD_X_POS / DISP_UPD_COUNT)

// Texture IDs
#define TEXTURE_ID_TITLE        8
#define TEXTURE_ID_START        9
#define TEXTURE_ID_SCORE        10
#define TEXTURE_ID_FACE         11
#define TEXTURE_ID_SCORES       14

//////
TitleLevel::TitleLevel() : mLoadStep(0), mStartGame(true), mTitleY(0.f), mWitchY(0.f) {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);

    std::memset(&mStartArea, 0, sizeof(TouchArea));
    std::memset(&mScoresArea, 0, sizeof(TouchArea));
    std::memset(&mOkArea, 0, sizeof(TouchArea));

    mScoresData = Scores::getInstance(NULL);
#ifndef DEMO_VERSION
    mAdvertising = Advertising::getInstance();
#endif

    mTitle = NULL;
    mStart = NULL;
    mScore = NULL;
    mFace = NULL;
    mScores = NULL;
}
TitleLevel::~TitleLevel() {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);

    if (mTitle) delete mTitle;
    if (mStart) delete mStart;
    if (mScore) delete mScore;
    if (mFace) delete mFace;
    if (mScores) delete mScores;
}

bool TitleLevel::load(const Game* game) {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, (mLoadStep + 1));
#ifndef DEMO_VERSION
    static bool reqAdLoad = false;  // Avoid to load Ad again (needed coz Ad is loaded in UI thread)
                                    // -> 'Advertising::getStatus' method return value according UI thread workflow
#endif
    switch (++mLoadStep) {
        case 1: {

            if (!mTitle) {

                mTitle = new Element2D;
                mTitle->initialize(game2DVia(game));
                mTitle->start(TEXTURE_ID_TITLE);

                static float texCoords[8] = {0.f, 0.f, 0.f, 1.f, TITLE_WIDTH / TITLE_TEXTURE_WIDTH, 1.f,
                        TITLE_WIDTH / TITLE_TEXTURE_WIDTH, 0.f};
                mTitle->setTexCoords(texCoords);

                short halfHeight = static_cast<short>(game->getScreen()->height / 5.f) >> 1; // 80% of height / 2
                short halfWidth = static_cast<short>((TITLE_WIDTH / TITLE_HEIGHT) * halfHeight);
                if (halfWidth > (game->getScreen()->width >> 1)) {

                    halfWidth = static_cast<short>(0.8f * game->getScreen()->width) >> 1; // 80% of width
                    halfHeight = static_cast<short>((TITLE_HEIGHT / TITLE_WIDTH) * halfWidth);
                }
                mTitle->setVertices((game->getScreen()->width >> 1) - halfWidth, game->getScreen()->height + (halfHeight << 1),
                        (game->getScreen()->width >> 1) + halfWidth, game->getScreen()->height);

                mWitchY = ((mTitle->getBottom() - mTitle->getTop()) - (0.05f * game->getScreen()->top)) +
                        game->getScreen()->top;
            }
            else
                mTitle->resume(TEXTURE_ID_TITLE);
            break;
        }
        case 2: {

            if (!mStart) {

                mStart = new Element2D;
                mStart->initialize(game2DVia(game));
                mStart->start(TEXTURE_ID_START);
                mStart->setTexCoords(FULL_TEXCOORD_BUFFER);

                short halfWidth = static_cast<short>(game->getScreen()->width / WIDTH_RATIO);
                mStartArea.left = (game->getScreen()->width >> 2) - halfWidth;
                mStartArea.right = (game->getScreen()->width >> 2) + halfWidth;
                mStartArea.top = game->getScreen()->height - (game->getScreen()->height >> 2);
                mStartArea.bottom = game->getScreen()->height - ((game->getScreen()->height >> 2) - halfWidth);
                mStart->setVertices(-(halfWidth << 1), (game->getScreen()->height >> 2) + halfWidth,
                        0, (game->getScreen()->height >> 2) - halfWidth);

                mTitleY = (mStart->getTop() + (((game->getScreen()->top + mWitchY) - mStart->getTop()) / 2.f)) -
                        (game->getScreen()->top * 2.f) + ((mTitle->getTop() - mTitle->getBottom()) / 2.f);
            }
            else
                mStart->resume(TEXTURE_ID_START);
            break;
        }
        case 3: {

            if (!mScore) {

                mScore = new Element2D;
                mScore->initialize(game2DVia(game));
                mScore->start(TEXTURE_ID_SCORE);
                mScore->setTexCoords(FULL_TEXCOORD_BUFFER);

                short halfWidth = static_cast<short>(game->getScreen()->width / WIDTH_RATIO);
                mScoresArea.left = (game->getScreen()->width >> 1) + ((game->getScreen()->width >> 2) - halfWidth);
                mScoresArea.right = (game->getScreen()->width >> 1) + ((game->getScreen()->width >> 2) + halfWidth);
                mScoresArea.top = game->getScreen()->height - ((game->getScreen()->height >> 2) + halfWidth);
                mScoresArea.bottom = game->getScreen()->height - ((game->getScreen()->height >> 2) - halfWidth);
                mScore->setVertices(game->getScreen()->width, (game->getScreen()->height >> 2) + halfWidth,
                        game->getScreen()->width + (halfWidth << 1), (game->getScreen()->height >> 2) - halfWidth);
            }
            else
                mScore->resume(TEXTURE_ID_SCORE);
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9: {

            mScoresData->load(game, mLoadStep - 4);
            break;
        }
        case 10: {

            if (!mFace) {

                mFace = new Static2D;
                mFace->initialize(game2DVia(game));
                mFace->start(TEXTURE_ID_FACE);
                mFace->setTexCoords(FULL_TEXCOORD_BUFFER);

                static const float verCoords[8] = { (FACE_X0 / WITCH_TEXTURE_SIZE) - 0.5f,
                        ((WITCH_TEXTURE_SIZE - FACE_Y0) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY,
                        (FACE_X0 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - FACE_Y2) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY,
                        (FACE_X2 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - FACE_Y2) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY,
                        (FACE_X2 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - FACE_Y0) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY };
                mFace->setVertices(verCoords);
                mFace->setAlpha(0.f);
            }
            else
                mFace->resume(TEXTURE_ID_FACE);
            break;
        }
        case 11: {

            if (!mScores) {

                mScores = new Element2D;
                mScores->initialize(game2DVia(game));
                mScores->start(TEXTURE_ID_SCORES);

                mScores->setTexCoords(FULL_TEXCOORD_BUFFER);
                mScores->setVertices(0, 0, game->getScreen()->width, -game->getScreen()->width);

                mOkArea.left = static_cast<short>((OK_TEXTURE_LEFT * game->getScreen()->width) / SCORES_SIZE);
                mOkArea.right = game->getScreen()->width;
                mOkArea.top = game->getScreen()->height - (((game->getScreen()->height - game->getScreen()->width) >> 1) +
                        static_cast<short>(((SCORES_SIZE - OK_TEXTURE_TOP) * game->getScreen()->width) / SCORES_SIZE));
                mOkArea.bottom = mOkArea.top + static_cast<short>((OK_HEIGHT * game->getScreen()->width) / SCORES_SIZE);
            }
            else
                mScores->resume(TEXTURE_ID_SCORES);

#ifndef DEMO_VERSION
            reqAdLoad = false;
#endif
            break;
        }
        case MAX_TITLE_LOAD: {

#ifndef DEMO_VERSION
            unsigned char adStatus = Advertising::getStatus();
            assert(adStatus != Advertising::STATUS_NONE);

            if ((adStatus != Advertising::STATUS_READY) && (adStatus != Advertising::STATUS_LOADING))
                break;

            --mLoadStep;
            if ((adStatus == Advertising::STATUS_LOADING) || (reqAdLoad))
                boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            else
                reqAdLoad = mAdvertising->load();
#endif
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown load step: %d"), __PRETTY_FUNCTION__, __LINE__, mLoadStep);
            assert(NULL);
            break;
        }
#endif
    }
    return (mLoadStep == MAX_TITLE_LOAD);
}

bool TitleLevel::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_TITLELEVEL, (*game->getLog() % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
#endif
#ifndef DEMO_VERSION
    unsigned char adStatus = mAdvertising->refresh();
#endif

    static float titleVel = mTitleY / DISP_UPD_COUNT;
    static bool bOk = false; // Scores 'Ok' has been touched
    if (mStartGame) {

        if (mStart->getTransform()[Dynamic2D::TRANS_X] < CMD_X_POS) {

            mTitle->translate(0.f, titleVel);
            mStart->translate(CMD_VEL, 0.f);
            mScore->translate(-CMD_VEL, 0.f);
            return true;
        }
        else
            mFace->setAlpha(1.f);
    }
    else if (!mStartGame) {

        static float scoreVel = (game->getScreen()->top + 1.f) / DISP_UPD_COUNT;
        if (mStart->getTransform()[Dynamic2D::TRANS_X] > 0.f) {

            mTitle->translate(0.f, -titleVel);
            mStart->translate(-CMD_VEL, 0.f);
            mScore->translate(CMD_VEL, 0.f);
            return true;
        }
        else if (bOk) {

            mScoresData->hide();
            if (mScores->getTransform()[Dynamic2D::TRANS_Y] > 0.f)
                mScores->translate(0.f, -scoreVel);
            else {

                bOk = false;
                mStartGame = true;
                return false;
            }
            return true;
        }
        else if (mScores->getTransform()[Dynamic2D::TRANS_Y] < (game->getScreen()->top + 1.f)) {

            mScores->translate(0.f, scoreVel);
            return true;
        }
        else
            mScoresData->show();
    }

    unsigned char touchCount = game->mTouchCount;
    while (touchCount--) {

        if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
            continue;

        if (!mStartGame) {

            // Check 'Ok' touched
            if ((game->mTouchData[touchCount].X > mOkArea.left) && (game->mTouchData[touchCount].X < mOkArea.right) &&
                    (game->mTouchData[touchCount].Y > mOkArea.top) && (game->mTouchData[touchCount].Y < mOkArea.bottom)) {

                bOk = true;
                return true;
            }
            continue;
        }

        // Check scores touched
        if ((game->mTouchData[touchCount].X > mScoresArea.left) && (game->mTouchData[touchCount].X < mScoresArea.right) &&
                (game->mTouchData[touchCount].Y > mScoresArea.top) && (game->mTouchData[touchCount].Y < mScoresArea.bottom)) {

            mFace->setAlpha(0.f);
            mStartGame = false;
            return false;
        }

#ifndef DEMO_VERSION
#ifdef DEBUG
        if ((adStatus == Advertising::STATUS_LOADING) || (adStatus == Advertising::STATUS_DISPLAYING)) {

            LOGI(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(" - Do not start when loading/displaying advertising"),
                    __PRETTY_FUNCTION__, __LINE__);
            break; // Do not hide advertising if loading/displaying (especially if loading)
        }
#else
        if ((adStatus == Advertising::STATUS_LOADING) || (adStatus == Advertising::STATUS_DISPLAYING))
            break;
#endif
#endif
        // Check start game touched
        if ((game->mTouchData[touchCount].X > mStartArea.left) && (game->mTouchData[touchCount].X < mStartArea.right) &&
                (game->mTouchData[touchCount].Y > mStartArea.top) && (game->mTouchData[touchCount].Y < mStartArea.bottom)) {

            mFace->setAlpha(0.f);
            return false;
        }
    }
    return true;
}

void TitleLevel::render() const {

    if ((mStartGame) || (mStart->getTransform()[Dynamic2D::TRANS_X] > 0.f)) {

        mTitle->render(true);
        mStart->render(true);
        mScore->render(true);
        mFace->render(true);
    }
    else {

        mScores->render(true);
        mScoresData->render();
    }
}
