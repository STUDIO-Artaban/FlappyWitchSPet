#include "Witch.h"
#include "BoundCoords.h"

#include <libeng/Tools/Tools.h>
#include <stdlib.h>

#define SPEED_VEL               7.f
#define SLOW_VEL                28.f
#define MOVE_VEL                16.f
#define FALL_VEL                -0.0035f

// Texture IDs
#define TEXTURE_ID_WITCH0       15
//#define TEXTURE_ID_WITCH1      16
//#define TEXTURE_ID_WITCH2      17
//#define TEXTURE_ID_WITCH3      18
//#define TEXTURE_ID_WITCH4      19
//#define TEXTURE_ID_WITCH5      20
//#define TEXTURE_ID_WITCH6      21
//#define TEXTURE_ID_WITCH7      22
//#define TEXTURE_ID_FAILED      23

//////
Witch::Witch() : Static2D(false), Dynamic2D(), mAnimIdx(0), mStatus(STATUS_TITLE), mFlap(false), mCount(0),
    mDelay(MAX_DELAY), mNewFlap(false) {

    LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    std::memset(mAnims, 0, ANIMS_COUNT);

    mVertices[0] = -0.5f;
    mVertices[1] = 0.5f;
    mVertices[2] = -0.5f;
    mVertices[3] = -0.5f;
    mVertices[4] = 0.5f;
    mVertices[5] = -0.5f;
    mVertices[6] = 0.5f;
    mVertices[7] = 0.5f;

    mTransform = new float[TRANSFORM_BUFFER_SIZE];
    std::memcpy(mTransform, Object2D::TransformBuffer, sizeof(float) * TRANSFORM_BUFFER_SIZE);

    mVelocities = new float[2];
    mVelocities[ROTATE_VEL] = 0.f;
    mVelocities[TRANS_VEL] = 0.f;
}
Witch::~Witch() {

    LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    for (unsigned char i = 0; i < ANIMS_COUNT; ++i)
        if (mAnims[i]) mTextures->delTexture(mAnims[i]);

    mTextureIdx = mTextures->getIndex(TEXTURE_ID_FAILED);
    if (mTextureIdx != TEXTURE_IDX_INVALID)
        mTextures->delTexture(mTextureIdx);
}

void Witch::load(unsigned char step) {

    LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(" - s:%d"), __PRETTY_FUNCTION__, __LINE__, step);
    assert((step > -1) && (step < 9));

    if (step == 8) {

        mTextures->genTexture(Textures::loadTexture(TEXTURE_ID_FAILED));

        mAnimIdx = 0;
        mTextureIdx = mAnims[0];
        return;
    }
    mAnims[step] = Textures::loadTexture(TEXTURE_ID_WITCH0 + step);
    mTextures->genTexture(mAnims[step], false);
}

void Witch::prepare() {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    assert(fequal(mTransform[Dynamic2D::SCALE_X], 1.f));
    assert(!mBoundCount);
    assert(!mBounds);
#endif

    reset();

    mBoundCount = 3;
    mBounds = new Bounds*[mBoundCount];
    mBounds[0] = new Polygone2D((WITCH_BOUND_A_X1 / WITCH_TEXTURE_SIZE) - 0.5f,
            ((WITCH_TEXTURE_SIZE - WITCH_BOUND_A_Y1) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_A_X2 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_A_Y2) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_A_X3 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_A_Y3) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_A_X4 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_A_Y4) / WITCH_TEXTURE_SIZE) - 0.5f);
    mBounds[1] = new Polygone2D((WITCH_BOUND_B_X1 / WITCH_TEXTURE_SIZE) - 0.5f,
            ((WITCH_TEXTURE_SIZE - WITCH_BOUND_B_Y1) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_B_X2 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_B_Y2) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_B_X3 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_B_Y3) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_B_X4 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_B_Y4) / WITCH_TEXTURE_SIZE) - 0.5f);
    mBounds[2] = new Polygone2D((WITCH_BOUND_C_X1 / WITCH_TEXTURE_SIZE) - 0.5f,
            ((WITCH_TEXTURE_SIZE - WITCH_BOUND_C_Y1) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_C_X2 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_C_Y2) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_C_X3 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_C_Y3) / WITCH_TEXTURE_SIZE) - 0.5f,
            (WITCH_BOUND_C_X4 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - WITCH_BOUND_C_Y4) / WITCH_TEXTURE_SIZE) - 0.5f);
#ifdef DEBUG
    mBounds[0]->setLog(mLog);
    mBounds[1]->setLog(mLog);
    mBounds[2]->setLog(mLog);
#endif
}

void Witch::update(const Game* game, const Level* level) {

    switch (mStatus) {
        case STATUS_TITLE: {

            if (++mCount == MAX_DELAY) {

                mCount = 0;
                if (++mAnimIdx == ANIMS_COUNT)
                    mAnimIdx = 0;
                mTextureIdx = mAnims[mAnimIdx];
            }
            break;
        }
        case STATUS_PLAYING: {

            if ((mFlap) && (++mCount == mDelay)) {

                mCount = 0;
                if (++mAnimIdx == ANIMS_COUNT) {

                    if (!(mFlap = mNewFlap)) {

                        mDelay = MAX_DELAY;
                        mVelocities[ROTATE_VEL] = PI_F / -2.f;
                    }
                    else
                        mNewFlap = false;
                    mAnimIdx = 0;
                }
                mTextureIdx = mAnims[mAnimIdx];
            }
            rotate(mTransform[Dynamic2D::ROTATE_C] - ((mTransform[Dynamic2D::ROTATE_C] -
                    mVelocities[ROTATE_VEL]) / ((mVelocities[ROTATE_VEL] < 0.f)? SLOW_VEL:SPEED_VEL)));
            mVelocities[TRANS_VEL] -= mVelocities[TRANS_VEL] / MOVE_VEL;
            translate(0.f, mVelocities[TRANS_VEL]);
            if (!mFlap)
                mVelocities[TRANS_VEL] += FALL_VEL;

            //////
            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue;

                if (mFlap) {

                    if (mNewFlap)
                        mVelocities[ROTATE_VEL] = PI_F / 4.f;
                    mDelay = MAX_DELAY >> 1;
                    mNewFlap = true;
                }
                else {

                    mFlap = true;
                    mVelocities[ROTATE_VEL] = PI_F / 8.f;
                }
                mVelocities[TRANS_VEL] += RISE_VEL;
                mCount = mDelay - 1;
                break;
            }
            break;
        }
        case STATUS_SCREAMING: {

            static float scaleVel = 0.f;
            static float rotateVel = 0.f;
            static float transVel = 0.f;
            static unsigned char incIdx = 0;
            if (mBounds) {

                assert(mBoundCount);
                delete mBounds[0];
                delete mBounds[1];
                delete mBounds[2];
                delete [] mBounds;
                mBounds = NULL;
                mBoundCount = 0;

                scaleVel = ((rand() % 300) / 100.f) +
                        (WITCH_SCALE_RATIO * game->getScreen()->height / game->getScreen()->width);
                rotateVel = (rand() % (314 >> 1)) / -200.f;
                transVel = (rand() % 100) / 2000.f;
                incIdx = 0;
            }
            scale(mTransform[Dynamic2D::SCALE_X] - ((mTransform[Dynamic2D::SCALE_X] - scaleVel) / MOVE_VEL),
                    mTransform[Dynamic2D::SCALE_Y] - ((mTransform[Dynamic2D::SCALE_Y] - scaleVel) / MOVE_VEL));
            rotate(mTransform[Dynamic2D::ROTATE_C] + rotateVel);
            if (incIdx < INCREASE_BUFFER_SIZE) {

                translate(transVel * (1.f - IncreaseBuffer[incIdx]), (1.f - IncreaseBuffer[incIdx]) / MOVE_VEL);
                if (++incIdx == INCREASE_BUFFER_SIZE)
                    transVel = FALL_VEL;
            }
            else {

                translate(0.f, transVel);
                transVel += FALL_VEL;
            }
            break;
        }
    }
}

void Witch::render(bool resetUniform) const {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WITCH, (*mLog % 100), LOG_FORMAT(" - r:%s (u:%s)"), __PRETTY_FUNCTION__, __LINE__,
            (resetUniform)? "true":"false", (mUpdated)? "true":"false");
    if ((resetUniform) || (mUpdated))
        Dynamic2D::transform(getShader2D(), mLog);
#else
    if ((resetUniform) || (mUpdated))
        Dynamic2D::transform(getShader2D());
#endif
    Panel2D::render(false);
}
