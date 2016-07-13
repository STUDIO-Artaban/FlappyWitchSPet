#ifndef TOWERS_H_
#define TOWERS_H_

#include <libeng/Game/Game.h>
#include <libeng/Graphic/Object/2D/Element2D.h>

#include <libeng/Graphic/Bounds/BorderLeft.h>
#include <libeng/Graphic/Bounds/BorderTop.h>
#include <libeng/Graphic/Bounds/BorderBottom.h>
#include <libeng/Graphic/Bounds/BorderRight.h>

#include "Global.h"

#ifdef __ANDROID__
#include "Title/Scores.h"
#else
#include "Scores.h"
#endif
#include <stdlib.h>

#define MEDAL_CLIP(diff, bound)     ((diff > bound)? (diff - bound):((diff < -bound)? (diff + bound):0))
#define TOWER_H_LAG                 1.2f // Horizontal towers lag (in unit)

using namespace eng;

class Tower {

private:
    Element2D mTopTower;
    Element2D mTopHead;

    Element2D mBottomHead;
    Element2D mBottomTower;

    BorderLeft mRightBounds;
    BorderRight mLeftBounds;
    BorderRight mEndBounds;
    BorderBottom mTopBounds;
    BorderTop mBottomBounds;

public:
    Tower() : mLeftBounds(0.f), mTopBounds(0.f), mBottomBounds(0.f), mRightBounds(0.f), mEndBounds(0.f), mPosY(0.f) {
        LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    }
    virtual ~Tower() { LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__); }

    //
    inline const Element2D* getTop() const { return &mTopHead; }
    inline const Element2D* getBottom() const { return &mBottomHead; }

    inline const Bounds* getLeftBound() const { return static_cast<const Bounds*>(&mLeftBounds); }
    inline const Bounds* getRightBound() const { return static_cast<const Bounds*>(&mRightBounds); }
    inline const Bounds* getEndBound() const { return static_cast<const Bounds*>(&mEndBounds); }
    inline const Bounds* getTopBound() const { return static_cast<const Bounds*>(&mTopBounds); }
    inline const Bounds* getBottomBound() const { return static_cast<const Bounds*>(&mBottomBounds); }

    static float scaleTH;
    float mPosY;

    //////
    void start(const Game* game, unsigned char rank);
    inline void pause() {

        LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mTopTower.pause();
        mTopHead.pause();
        mBottomHead.pause();
        mBottomTower.pause();
    }
    void resume();

    inline void scroll(float x, float y) {

        mTopTower.translate(x, y);
        mTopHead.translate(x, y);

        mBottomHead.translate(x, y);
        mBottomTower.translate(x, y);

        //
        mLeftBounds.mRight += x;
        mRightBounds.mLeft += x;
        mEndBounds.mRight += x;
        mTopBounds.mBottom += y;
        mBottomBounds.mTop += y;
    }
    void render() const;

};

//////
class Towers {

private:
    Tower mTowers[3];

    unsigned char mTowerIdx; // Index of the tower which is facing the witch (to check collision)
    short mLastPos; // Last displayed tower Y position [0;99]

    static float maxPos;
    static float minPos;

    static inline float getPosition(short &lastPos, short score) {

        short newPos = static_cast<short>(rand() % 100);
        short diffPos = newPos - lastPos;

        if (score < MEDAL_WHITE) // White
            newPos -= MEDAL_CLIP(diffPos, 20);
        else if (score < MEDAL_BRONZE) // Bronze
            newPos -= MEDAL_CLIP(diffPos, 30);
        else if (score < MEDAL_SILVER) // Silver
            newPos -= MEDAL_CLIP(diffPos, 50);
        //else // Gold

        lastPos = newPos;
        return (((maxPos - minPos) / 99.f) * lastPos) + minPos;
    }

public:
    Towers();
    virtual ~Towers();

    inline const Tower* get() const { return &mTowers[mTowerIdx]; }
    inline void next() { if (++mTowerIdx > 2) mTowerIdx = 0; }

    //////
    void start(const Game* game, float top, float bottom);
    inline void pause() {

        LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mTowers[0].pause();
        mTowers[1].pause();
        mTowers[2].pause();
    }
    void resume();

    inline void scroll(float x) {

        mTowers[0].scroll(x, 0.f);
        mTowers[1].scroll(x, 0.f);
        mTowers[2].scroll(x, 0.f);
    }
    unsigned char update(short score);

    void render() const;

};

#endif // TOWERS_H_
