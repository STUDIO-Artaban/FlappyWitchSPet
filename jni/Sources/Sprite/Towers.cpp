#include "Towers.h"
#include "Witch.h"

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Graphic/Bounds/2D/Rectangle2D.h>

#define TOWER_SCALE                 (0.25f * REF_SCREEN_RATIO) // Tower width (in unit)
#define TOWERS_SCALE                (0.3f * REF_SCREEN_RATIO) // Tower head width (in unit)
#define TOWER_V_LAG                 (0.38f * REF_SCREEN_RATIO) // Vertical towers lag (in unit)

#define TEXTURE_TOWER_WIDTH         128
#define TEXTURE_TOWER_HEIGHT        64
#define TEXTURE_TOWERS_SIZE         128.f
#define TOWERS_WIDTH                128
#define TOWERS_HEIGHT               59

// Texture IDs
#define TEXTURE_ID_TOWER            24
#define TEXTURE_ID_TOWERS           25

float Tower::scaleTH = 0.f;

void Tower::start(const Game* game, unsigned char rank) {

    LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(" - g:%x; r:%d"), __PRETTY_FUNCTION__, __LINE__, game, rank);
    mTopTower.initialize(game2DVia(game));
    mTopHead.initialize(game2DVia(game));

    mBottomHead.initialize(game2DVia(game));
    mBottomTower.initialize(game2DVia(game));

#ifdef DEBUG
    mLeftBounds.setLog(game->getLog());
    mEndBounds.setLog(game->getLog());
    mRightBounds.setLog(game->getLog());
    mTopBounds.setLog(game->getLog());
    mBottomBounds.setLog(game->getLog());
#endif
    mTopTower.start(TEXTURE_ID_TOWER);

    float texCoords[8] = { 1.f, 32.f, 1.f, 0.f, 0.f, 0.f, 0.f, 32.f };
    mTopTower.setTexCoords(texCoords);

    mTopTower.setVertices((game->getScreen()->width >> 1) - (TEXTURE_TOWER_WIDTH >> 1),
            (game->getScreen()->height >> 1) + (TEXTURE_TOWER_HEIGHT << 4),
            (game->getScreen()->width >> 1) + (TEXTURE_TOWER_WIDTH >> 1),
            (game->getScreen()->height >> 1) - (TEXTURE_TOWER_HEIGHT << 4));

    mBottomTower.start(TEXTURE_ID_TOWER);

    texCoords[0] = 0.f;
    texCoords[1] = 0.f;
    texCoords[2] = 0.f;
    texCoords[3] = 32.f;
    texCoords[4] = 1.f;
    texCoords[5] = 32.f;
    texCoords[6] = 1.f;
    texCoords[7] = 0.f;
    mBottomTower.setTexCoords(texCoords);
    mBottomTower.copyVertices(mTopTower);

    mTopHead.start(TEXTURE_ID_TOWERS);

    texCoords[3] = TOWERS_HEIGHT / TEXTURE_TOWERS_SIZE;
    texCoords[5] = texCoords[3];
    mTopHead.setTexCoords(texCoords);
    mTopHead.setVertices((game->getScreen()->width >> 1) - (TOWERS_WIDTH >> 1),
            (game->getScreen()->height >> 1) + (TOWERS_HEIGHT >> 1),
            (game->getScreen()->width >> 1) + (TOWERS_WIDTH >> 1),
            (game->getScreen()->height >> 1) - (TOWERS_HEIGHT >> 1));

#ifdef DEBUG
    Rectangle2D* rect2D = new Rectangle2D(mTopHead.getLeft(), mTopHead.getRight(), mTopHead.getTop(), mTopHead.getBottom());
    mTopHead.addBound(rect2D);
    rect2D->setLog(game->getLog());
#else
    mTopHead.addBound(new Rectangle2D(mTopHead.getLeft(), mTopHead.getRight(), mTopHead.getTop(), mTopHead.getBottom()));
#endif

    mBottomHead.start(TEXTURE_ID_TOWERS);

    texCoords[1] = texCoords[3];
    texCoords[3] *= 2.f;
    texCoords[5] = texCoords[3];
    texCoords[7] = texCoords[1];
    mBottomHead.setTexCoords(texCoords);
    mBottomHead.copyVertices(mTopHead);

#ifdef DEBUG
    rect2D = new Rectangle2D(mBottomHead.getLeft(), mBottomHead.getRight(), mBottomHead.getTop(), mBottomHead.getBottom());
    mBottomHead.addBound(rect2D);
    rect2D->setLog(game->getLog());
#else
    mBottomHead.addBound(new Rectangle2D(mBottomHead.getLeft(), mBottomHead.getRight(), mBottomHead.getTop(),
            mBottomHead.getBottom()));
#endif

    //
    scaleTH = (TOWERS_SCALE * game->getScreen()->height / game->getScreen()->width) / (mTopHead.getRight() -
            mTopHead.getLeft());
    mTopHead.scale(scaleTH, scaleTH);

    float towerV = TOWER_V_LAG * game->getScreen()->height / game->getScreen()->width;
    mTopHead.translate(0.f, ((mTopHead.getTop() - mTopHead.getBottom()) * scaleTH / 2.f) + towerV);
    mTopBounds.mBottom = towerV;

    mBottomHead.scale(scaleTH, scaleTH);
    mBottomHead.translate(0.f, ((mBottomHead.getBottom() - mBottomHead.getTop()) * scaleTH / 2.f) - towerV);
    mBottomBounds.mTop = -towerV;

    float scaleT = (TOWER_SCALE * game->getScreen()->height / game->getScreen()->width) / (mTopTower.getRight() -
            mTopTower.getLeft());
    mTopTower.scale(scaleT, scaleT);
    mTopTower.translate(0.f,
            (scaleT * (mTopTower.getTop() - mTopTower.getBottom()) / 2.f) +
            ((mTopHead.getTop() - mTopHead.getBottom()) * scaleTH) + towerV);

    mBottomTower.scale(scaleT, scaleT);
    mBottomTower.translate(0.f,
            (scaleT * (mBottomTower.getBottom() - mBottomTower.getTop()) / 2.f) - towerV -
            ((mBottomHead.getTop() - mBottomHead.getBottom()) * scaleTH));

    mRightBounds.mLeft = mTopHead.getRight() * scaleTH;
    mEndBounds.mRight = mTopTower.getRight() * scaleT;
    mLeftBounds.mRight = mTopTower.getLeft() * scaleT;

    scroll((rank * TOWER_H_LAG) + 1.4f, 0.f);
    // With 1.4f for first horizontal tower lag
}
void Tower::resume() {

    LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mTopTower.resume(TEXTURE_ID_TOWER);
    mTopHead.resume(TEXTURE_ID_TOWERS);

    mBottomHead.resume(TEXTURE_ID_TOWERS);
    mBottomTower.resume(TEXTURE_ID_TOWER);
}

void Tower::render() const {

    mTopTower.render(true);
    mTopHead.render(true);

    mBottomHead.render(true);
    mBottomTower.render(true);
}

//////
float Towers::maxPos = 0.f;
float Towers::minPos = 0.f;

Towers::Towers() : mTowerIdx(0), mLastPos(0) {
    LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
}
Towers::~Towers() { LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__); }

void Towers::start(const Game* game, float top, float bottom) {

    LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(" - g:%x; t:%f; b:%f"), __PRETTY_FUNCTION__, __LINE__, game, top, bottom);
    mTowers[0].start(game, 1);
    mTowers[1].start(game, 2);
    mTowers[2].start(game, 3);

    float towerH = ((TOWERS_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * Tower::scaleTH;
    float towerV = TOWER_V_LAG * game->getScreen()->height / game->getScreen()->width;
    maxPos = top - towerV - (towerH / 2.f);
    minPos = bottom + towerV + (3.f * towerH / 2.f);

    float midPos = maxPos - ((maxPos - minPos) / 2.f);
    mTowers[0].scroll(0.f, midPos);
    mTowers[1].scroll(0.f, midPos);
    mTowers[2].scroll(0.f, midPos);

    maxPos -= midPos;
    minPos -= midPos;

    mLastPos = 50;
    mTowers[0].mPosY = getPosition(mLastPos, 0);
    mTowers[0].scroll(0.f, mTowers[0].mPosY);
    mTowers[1].mPosY = getPosition(mLastPos, 0);
    mTowers[1].scroll(0.f, mTowers[1].mPosY);
    mTowers[2].mPosY = getPosition(mLastPos, 0);
    mTowers[2].scroll(0.f, mTowers[2].mPosY);
}
void Towers::resume() {

    LOGV(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mTowers[0].resume();
    mTowers[1].resume();
    mTowers[2].resume();
}

unsigned char Towers::update(short score) {

    char swapIdx = static_cast<char>(LIBENG_NO_DATA);
    if (bLeftVia(mTowers[0].getRightBound())->mLeft < -1.f) swapIdx = 0;
    if (bLeftVia(mTowers[1].getRightBound())->mLeft < -1.f) swapIdx = 1;
    if (bLeftVia(mTowers[2].getRightBound())->mLeft < -1.f) swapIdx = 2;

    short lastPos = mLastPos;
    float newPos = getPosition(lastPos, score);

#ifdef DEBUG
    if (swapIdx != static_cast<char>(LIBENG_NO_DATA)) {
        LOGI(LOG_LEVEL_TOWERS, 0, LOG_FORMAT(" - Swap tower (idx:%d; last:%d; new:%d"), __PRETTY_FUNCTION__, __LINE__, swapIdx,
                mLastPos, lastPos);
    }
#endif
    mTowers[0].scroll(0.f, (swapIdx == 0)? -mTowers[0].mPosY:0.f);
    mTowers[1].scroll(0.f, (swapIdx == 1)? -mTowers[1].mPosY:0.f);
    mTowers[2].scroll(0.f, (swapIdx == 2)? -mTowers[2].mPosY:0.f);

    mTowers[0].scroll((swapIdx == 0)? (3.f * TOWER_H_LAG):0.f, (swapIdx == 0)? newPos:0.f);
    mTowers[1].scroll((swapIdx == 1)? (3.f * TOWER_H_LAG):0.f, (swapIdx == 1)? newPos:0.f);
    mTowers[2].scroll((swapIdx == 2)? (3.f * TOWER_H_LAG):0.f, (swapIdx == 2)? newPos:0.f);

    mTowers[0].mPosY = (swapIdx == 0)? newPos:mTowers[0].mPosY;
    mTowers[1].mPosY = (swapIdx == 1)? newPos:mTowers[1].mPosY;
    mTowers[2].mPosY = (swapIdx == 2)? newPos:mTowers[2].mPosY;

    mLastPos = (swapIdx != static_cast<char>(LIBENG_NO_DATA))? lastPos:mLastPos;

    // In any case all of this code takes the same time of execution (to avoid lag when playing)
    // -> The way to code described above is not needed coz should be executed quickly (FPS >= 60). This is usefull only
    //    when FPS < 60 all along the playing game

    return swapIdx;
}
void Towers::render() const {

    mTowers[0].render();
    mTowers[1].render();
    mTowers[2].render();
}
