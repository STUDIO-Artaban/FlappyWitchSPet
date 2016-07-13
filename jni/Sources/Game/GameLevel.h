#ifndef GAMELEVEL_H_
#define GAMELEVEL_H_

#include "Global.h"

#include <libeng/Game/Game.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Graphic/Scrolling/Scrolling.h>
#include <libeng/Graphic/Scrolling/Panel.h>
#include <libeng/Graphic/Text/2D/Text2D.h>

#include <libeng/Graphic/Bounds/BorderBottom.h>
#ifndef DEMO_VERSION
#include <libeng/Advertising/Advertising.h>
#endif

#ifdef __ANDROID__
#include "Sprite/Witch.h"
#include "Sprite/TouchArea.h"
#include "Sprite/Towers.h"
#include "Sprite/Bury.h"
#include "Sprite/Share.h"
#include "Title/Scores.h"
#else
#include "Witch.h"
#include "TouchArea.h"
#include "Towers.h"
#include "Bury.h"
#include "Share.h"
#include "Scores.h"
#endif

#define MAX_GAME_LOAD           38

#define MAX_COLOR_COUNT         4
#define SCORE_FONT_HEIGHT       122
#define PLAY_SCALE              4.f

#define FLOOR_TEXTURE_HEIGHT    128.f
#define FLOOR_HEIGHT            85
#define HALLOWEEN_VEL           ((FLOOR_HEIGHT / (FLOOR_TEXTURE_HEIGHT * 2.f)) / DISP_UPD_COUNT)
#define TRANS_VELOCITY          -0.01f

// Depth
#define TWILIGHT_DEPTH          0.2f
#define HALLOWEEN_DEPTH         0.4f

// Texture IDs
#define TEXTURE_ID_PUMPKIN      2
#define TEXTURE_ID_FLOOR        7

// Advertising IDs
#define ADVERTISING_ID_TITLE    0

using namespace eng;

//////
class GameLevel {

private:
    unsigned char mLoadStep;
    Player* mPlayer;
#ifndef DEMO_VERSION
    Advertising* mAdvertising;
#endif

    Static2D* mBack;
    Static2D* mMoon;
    Scroll* mTwilight;
    Scroll* mHalloween;
    Scroll* mFloor;
    Bury* mBury;

    Element2D* mPauseA;
    Element2D* mPauseB;

    Static2D* mPlay;

    Text2D* mScore;
    Scores* mScoreData;
    Towers* mTowers;

    void increaseScore();

    Element2D* mReady;
    Element2D* mGrayWitch;
    Element2D* mArrow;
    Element2D* mHand;
    Element2D* mTopA;
    Element2D* mTopB;
    Element2D* mTopC;

    Element2D* mGameOver;
    Element2D* mPanel;
    Share* mShare;

    TouchArea mOkArea;

    float mPauseY;
    float mScoreX;
    float mReadyY;
    float mHandY;
    float mPlayS;

    enum {

        GAME_INIT = 0,
        GAME_READY,
        GAME_STARTING,
        GAME_STARTED,
        GAME_PAUSE,
        GAME_OVER, // Witch's pet has touched something
        GAME_SCREAMING,
        GAME_SCORING,
        GAME_SHARE
    };
    unsigned char mStep;

    inline void moveWorld() {

        mTwilight->scrollX(TRANS_VELOCITY * TWILIGHT_DEPTH, true);
        mHalloween->scrollX(TRANS_VELOCITY * HALLOWEEN_DEPTH, true);
        mFloor->scrollX(TRANS_VELOCITY, false);
        mBury->translate(TRANS_VELOCITY, 0.f);

        mTowers->scroll(TRANS_VELOCITY);
    }
    void terminate();

    //
    BorderBottom* mFloorBound;

public:
    GameLevel();
    virtual ~GameLevel();

#ifndef DEMO_VERSION
    static unsigned char adDisplayID;
#endif

    inline unsigned char getLoadStep() const { return mLoadStep; }
    inline void setColor(const float colors[MAX_COLOR_COUNT][3], unsigned char colorIdx) {

        LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - c:%x; i:%d"), __PRETTY_FUNCTION__, __LINE__, colors, colorIdx);
        assert(mBack);
        assert(mPauseA);

        mBack->setRed(colors[colorIdx][0]);
        mBack->setGreen(colors[colorIdx][1]);
        mBack->setBlue(colors[colorIdx][2]);

        if (++colorIdx == MAX_COLOR_COUNT)
            colorIdx = 0;

        mPauseA->setRed(colors[colorIdx][0]);
        mPauseA->setGreen(colors[colorIdx][1]);
        mPauseA->setBlue(colors[colorIdx][2]);

        mTopA->setRed(colors[colorIdx][0]);
        mTopA->setGreen(colors[colorIdx][1]);
        mTopA->setBlue(colors[colorIdx][2]);

        if (++colorIdx == MAX_COLOR_COUNT)
            colorIdx = 0;

        mReady->setRed(colors[colorIdx][0]);
        mReady->setGreen(colors[colorIdx][1]);
        mReady->setBlue(colors[colorIdx][2]);

        mGameOver->setRed(colors[colorIdx][0]);
        mGameOver->setGreen(colors[colorIdx][1]);
        mGameOver->setBlue(colors[colorIdx][2]);
    }

    inline void reset(float bottom, short width) {

        LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - b:%f; w:%d"), __PRETTY_FUNCTION__, __LINE__, bottom, width);
        assert(mTwilight);
        assert(mHalloween);
        assert(mFloor);

        float verticesY[4];
        verticesY[1] = bottom + 0.25f;
        verticesY[0] = verticesY[1] + 1.f;
        verticesY[2] = verticesY[1];
        verticesY[3] = verticesY[0];
        panelVia((*mTwilight)[0])->setVerticesH(verticesY);
        panelVia((*mTwilight)[1])->setVerticesH(verticesY);
        panelVia((*mTwilight)[2])->setVerticesH(verticesY);

        verticesY[0] = 2.f + bottom;
        verticesY[1] = bottom;
        verticesY[2] = bottom;
        verticesY[3] = verticesY[0];
        panelVia((*mHalloween)[0])->setVerticesH(verticesY);
        panelVia((*mHalloween)[1])->setVerticesH(verticesY);
        panelVia((*mHalloween)[2])->setVerticesH(verticesY);

        verticesY[0] = bottom;
        verticesY[1] = bottom - 0.5f;
        verticesY[2] = verticesY[1];
        verticesY[3] = verticesY[0];
        panelVia((*mFloor)[0])->setVerticesH(verticesY);
        panelVia((*mFloor)[1])->setVerticesH(verticesY);
        panelVia((*mFloor)[2])->setVerticesH(verticesY);
    }
    inline void setPosition() {

        assert(mTwilight);
        assert(mHalloween);
        assert(mFloor);

        float verticesY[4];

        float floorVel = 0.5f / DISP_UPD_COUNT;
        verticesY[0] = mFloor->getScrollTop() + floorVel;
        verticesY[1] = mFloor->getScrollBottom() + floorVel;
        verticesY[2] = verticesY[1];
        verticesY[3] = verticesY[0];
        panelVia((*mFloor)[0])->setVerticesH(verticesY);
        panelVia((*mFloor)[1])->setVerticesH(verticesY);
        panelVia((*mFloor)[2])->setVerticesH(verticesY);

        verticesY[0] = mTwilight->getScrollTop() + HALLOWEEN_VEL;
        verticesY[1] = mTwilight->getScrollBottom() + HALLOWEEN_VEL;
        verticesY[2] = verticesY[1];
        verticesY[3] = verticesY[0];
        panelVia((*mTwilight)[0])->setVerticesH(verticesY);
        panelVia((*mTwilight)[1])->setVerticesH(verticesY);
        panelVia((*mTwilight)[2])->setVerticesH(verticesY);

        verticesY[0] = mHalloween->getScrollTop() + HALLOWEEN_VEL;
        verticesY[1] = mHalloween->getScrollBottom() + HALLOWEEN_VEL;
        verticesY[2] = verticesY[1];
        verticesY[3] = verticesY[0];
        panelVia((*mHalloween)[0])->setVerticesH(verticesY);
        panelVia((*mHalloween)[1])->setVerticesH(verticesY);
        panelVia((*mHalloween)[2])->setVerticesH(verticesY);
    }

    Witch* mWitch;

    //////
    inline void pause() {

        mLoadStep = 0;

        if (mBack) mBack->pause();
        if (mMoon) mMoon->pause();
        if (mTwilight) mTwilight->pauseScroll();
        if (mHalloween) mHalloween->pauseScroll();
        if (mFloor) mFloor->pauseScroll();
        if (mBury) mBury->pause();
        if (mWitch) mWitch->stop(); // Pause
        if (mTowers) mTowers->pause();
        if (mPauseA) mPauseA->pause();
        if (mPauseB) mPauseB->pause();
        if (mPlay) mPlay->pause();
        if (mScore) mScore->pause();
        if (mReady) mReady->pause();
        if (mGrayWitch) mGrayWitch->pause();
        if (mArrow) mArrow->pause();
        if (mHand) mHand->pause();
        if (mTopA) mTopA->pause();
        if (mTopB) mTopB->pause();
        if (mTopC) mTopC->pause();
        if (mGameOver) mGameOver->pause();
        if (mPanel) mPanel->pause();
        if (mShare) mShare->pause();

        if (mStep == GAME_STARTED) {

            mPauseA->reset();
            mPauseA->scale(mPlayS, mPlayS);
            mPlay->setAlpha(1.f);
            mStep = GAME_PAUSE;
        }
    }
    bool load(const Game* game);
    bool update(const Game* game);

    void renderBackground() const;
    void render() const;

};

#endif // GAMELEVEL_H_
