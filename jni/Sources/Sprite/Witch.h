#ifndef WITCH_H_
#define WITCH_H_

#include <libeng/Graphic/Object/2D/Static2D.h>
#include <libeng/Graphic/Object/2D/Dynamic2D.h>

#include <libeng/Graphic/Bounds/2D/Polygone2D.h>
#include <libeng/Graphic/Bounds/BorderBottom.h>
#include <libeng/Graphic/Bounds/2D/Overlap2D.h>
#include <libeng/Graphic/Bounds/Overlap.h>

#include "Global.h"

#define WITCH_SCALE_RATIO       (0.5f * REF_SCREEN_RATIO) // Witch scale according that whitch size is 1
#define ANIMS_COUNT             8
#define RISE_VEL                0.03f
#define WITCH_TEXTURE_SIZE      512.f
#define MAX_DELAY               6

// Bounds IDs
#define BOUND_ID_FLOOR          0
#define BOUND_ID_PASSED         1
#define BOUND_ID_SCORE          2
#define BOUND_ID_TOPHEAD        3
#define BOUND_ID_BOTHEAD        4
#define BOUND_ID_TOWER          5
#define BOUND_ID_TOP            6
#define BOUND_ID_BOTTOM         7

// Texture IDs
#define TEXTURE_ID_FAILED       23

using namespace eng;

//////
class Witch : public Static2D, public Dynamic2D {

private:
    unsigned char mAnimIdx;
    unsigned char mAnims[ANIMS_COUNT];

    unsigned char mCount;
    unsigned char mDelay;
    bool mNewFlap;

    enum {

        STATUS_TITLE = 0,
        STATUS_PLAYING,
        STATUS_SCREAMING
    };
    unsigned char mStatus;

    bool mFlap;

    enum {

        ROTATE_VEL = 0,
        TRANS_VEL
    };

public:
    Witch();
    virtual ~Witch();

    void prepare();
    inline void play(bool force) {

        LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT("- f:%s"), __PRETTY_FUNCTION__, __LINE__, (force)? "true":"false");

        if (mStatus == STATUS_PLAYING)
            return;

        if ((!mAnimIdx) || (force)) {

            mFlap = true;
            mVelocities[ROTATE_VEL] = PI_F / 8.f;
            mVelocities[TRANS_VEL] = RISE_VEL;
            mStatus = STATUS_PLAYING;
            mCount = 0;
            mDelay = MAX_DELAY;
            mNewFlap = false;
            mAnimIdx = 0;
        }
    }
    inline void scream() {

        LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mTextureIdx = mTextures->getIndex(TEXTURE_ID_FAILED);
        mStatus = STATUS_SCREAMING;
    }
    inline void terminate() {

        LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mStatus = STATUS_TITLE;

        reset();
        rotate(0.f);
        scale(1.f, 1.f);
    }

    //////
    void load(unsigned char step);
    inline void stop() { // Pause

        LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        pause();

        std::memset(mAnims, 0, ANIMS_COUNT);
    }

    void update(const Game* game, const Level* level);

    inline bool checkCollision(const Bounds* bounds, unsigned char boundsId) {

        switch (boundsId) {
            case BOUND_ID_FLOOR:
                return ((static_cast<const BorderBottom*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[1]))) ||
                        (static_cast<const BorderBottom*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[0]))));

            case BOUND_ID_PASSED:
                return (static_cast<const BorderLeft*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[0])));

            case BOUND_ID_SCORE:
                return ((static_cast<const BorderRight*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[1]))) ||
                        (static_cast<const BorderRight*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[2]))));

            case BOUND_ID_TOPHEAD:
                return ((static_cast<const Rectangle2D*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[2]))) ||
                        (static_cast<const Rectangle2D*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[1]))) ||
                        (static_cast<const Rectangle2D*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[0]))));

            case BOUND_ID_BOTHEAD:
                return ((static_cast<const Rectangle2D*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[0]))) ||
                        (static_cast<const Rectangle2D*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[1]))) ||
                        (static_cast<const Rectangle2D*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[2]))));

            case BOUND_ID_TOWER:
                return ((static_cast<const BorderRight*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[1]))) ||
                        (static_cast<const BorderRight*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[2]))) ||
                        (static_cast<const BorderRight*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[0]))));

            case BOUND_ID_TOP:
                return ((static_cast<const BorderBottom*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[1]))) ||
                        (static_cast<const BorderBottom*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[0]))));

            case BOUND_ID_BOTTOM:
                return ((static_cast<const BorderTop*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[2]))) ||
                        (static_cast<const BorderTop*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[1]))) ||
                        (static_cast<const BorderTop*>(bounds)->overlap(static_cast<const Polygone2D*>(mBounds[0]))));
        }
        return false;
    }
    inline bool checkCollision(const Object* object, unsigned char objectId) { assert(NULL); return false; }

    void render(bool resetUniform) const;

};

#endif // WITCH_H_
