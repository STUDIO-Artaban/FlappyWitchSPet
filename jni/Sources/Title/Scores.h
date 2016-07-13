#ifndef SCORES_H_
#define SCORES_H_

#include <libeng/Game/Game.h>
#include <libeng/Storage/StoreData.h>
#include <libeng/Graphic/Text/2D/Text2D.h>
#include <libeng/Graphic/Object/2D/Static2D.h>

#include "Global.h"
#include <list>

#define MAX_SCORE_COUNT         10
#define SCORES_SIZE             512.f // Scores texture size (ID: 13)
#define MEDAL_SIZE              277 // In the texture

// Texture IDs
#define TEXTURE_ID_PANEL        13

// Sound IDs
#define SOUND_ID_SCORE          3

// Medals
#define MEDAL_WHITE             5
#define MEDAL_BRONZE            10
#define MEDAL_SILVER            20

using namespace eng;

inline void setMedal(Panel2D* medal, short score) {

    assert(medal);
    if (score < MEDAL_WHITE)
        return; // White

    if (score < MEDAL_BRONZE) { // Bronze

        medal->setRed(0xc0 / 255.f);
        medal->setGreen(0x6c / 255.f);
        medal->setBlue(0x44 / 255.f);
    }
    else if (score < MEDAL_SILVER) { // Silver

        medal->setRed(0x80 / 255.f);
        medal->setGreen(0x80 / 255.f);
        medal->setBlue(0x80 / 255.f);
    }
    else { // Gold

        medal->setRed(0xe8 / 255.f);
        medal->setGreen(0xde / 255.f);
        medal->setBlue(0x4d / 255.f);
    }
};

//////
class Scores {

private:
    static Scores* mThis;
#ifdef DEBUG
    const unsigned int* mLog;
#endif
    StoreData* mData;
    void fill(unsigned char from = 0);

    typedef struct ScoreData {

        ScoreData() : score(0), month(0), day(0), year(0) { }
        ScoreData(const ScoreData& copy) : score(copy.score), month(copy.month), day(copy.day), year(copy.year) { }

        short score;

        unsigned char month;
        unsigned char day;
        unsigned char year; // 20xx

        inline bool operator<(const ScoreData& compare) const {

            return ((compare.score != score)? (compare.score < score): // Highest to lowest score...
                    ((compare.year != year)? (compare.year < year): // ...Earliest to latest date
                            ((compare.month != month)? (compare.month < month):
                                    ((compare.day != day)? (compare.day < day):false))));
        }

    } ScoreData;
    std::list<ScoreData> mScoreList; // Sorted by score & date (see 'ScoreData::operator<' above)

    void save();

    Text2D* mScores[MAX_SCORE_COUNT];
    Static2D* mMedals[MAX_SCORE_COUNT];

    std::wstring getScore(unsigned char scoreIdx) const;

    Text2D* mDispScore;
    Text2D* mDispBest;
    Static2D* mDispMedal;

    Static2D* mNewA;
    Static2D* mNewB;

    float mDispWidth; // Digit width (in unit)

private:
    Scores(StoreData* data);
    virtual ~Scores();

    mutable short mCurScore;
    short mBestScore;
    bool mCalculate; // TRUE: Game score need to be displayed from 0 to current score; FALSE: Already displayed
    bool mNewBest; // TRUE: New best score has been done; FALSE: No best score

public:
    static Scores* getInstance(StoreData* data) {
        if (!mThis)
            mThis = new Scores(data);
        return mThis;
    }
    static void freeInstance() {
        if (mThis) {
            delete mThis;
            mThis = NULL;
        }
    }

    short mScore; // Last score done
    bool mDispTitle; // TRUE: Display all scores from title level; FALSE: Display last score done from game level

    inline void show() {

#ifdef DEBUG
        LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        if (mDispTitle)
            for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

                assert(mScores[i]);
                assert(mMedals[i]);
            }
#endif
        if (mDispTitle) // Title scores
            for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

                mScores[i]->setAlpha(1.f);
                mMedals[i]->setAlpha(1.f);
            }

        else { // Game score

            mDispScore->setAlpha(1.f);
            mDispBest->setAlpha(1.f);
            mDispMedal->setAlpha(1.f);

            if (mNewBest) {

                mNewA->setAlpha(1.f);
                mNewB->setAlpha(1.f);
            }
        }
    };
    inline void hide() {

#ifdef DEBUG
        LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        if (mDispTitle)
            for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

                assert(mScores[i]);
                assert(mMedals[i]);
            }
#endif
        if (mDispTitle) // Title scores
            for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

                mScores[i]->setAlpha(0.f);
                mMedals[i]->setAlpha(0.f);
            }

        else { // Game score

            mDispScore->setAlpha(0.f);
            mDispBest->setAlpha(0.f);
            mDispMedal->setAlpha(0.f);

            mNewA->setAlpha(0.f);
            mNewB->setAlpha(0.f);
        }
    };
    inline void prepare() {

        LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mCalculate = true;
        mNewBest = false;
        hide();
    }
    inline bool isBestScore() const { return mNewBest; }

    //////
    void load(const Game* game, unsigned char step);
    inline void pause() {

        LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            if (mScores[i]) mScores[i]->pause();
            if (mMedals[i]) mMedals[i]->pause();
        }
        if (mDispScore) mDispScore->pause();
        if (mDispBest) mDispBest->pause();
        if (mDispMedal) mDispMedal->pause();
        if (mNewA) mNewA->pause();
        if (mNewB) mNewB->pause();
    };

    void create();
    void read();
    bool update();

    void render() const;

};

#endif // SCORES_H_
