#ifndef BURY_H_
#define BURY_H_

#include <libeng/Game/Game.h>
#include <libeng/Graphic/Object/2D/Sprite2D.h>

#include "Global.h"

#define BURY_PIC_COUNT          6
#define FONT_TEXTURE_SIZE       1024.f
#define LAG_FROM_BOTTOM         0.12f

#ifdef __ANDROID__
#include "Game/PanelCoords.h"
#else
#include "PanelCoords.h"
#endif

static const float buryPicCoords[BURY_PIC_COUNT][8] = {
        { DINO_X0 / FONT_TEXTURE_SIZE, DINO_Y0 / FONT_TEXTURE_SIZE, DINO_X0 / FONT_TEXTURE_SIZE, DINO_Y2 / FONT_TEXTURE_SIZE,
          DINO_X2 / FONT_TEXTURE_SIZE, DINO_Y2 / FONT_TEXTURE_SIZE, DINO_X2 / FONT_TEXTURE_SIZE, DINO_Y0 / FONT_TEXTURE_SIZE },
        { COMPUTER_X0 / FONT_TEXTURE_SIZE, COMPUTER_Y0 / FONT_TEXTURE_SIZE, COMPUTER_X0 / FONT_TEXTURE_SIZE, COMPUTER_Y2 / FONT_TEXTURE_SIZE,
          COMPUTER_X2 / FONT_TEXTURE_SIZE, COMPUTER_Y2 / FONT_TEXTURE_SIZE, COMPUTER_X2 / FONT_TEXTURE_SIZE, COMPUTER_Y0 / FONT_TEXTURE_SIZE },
        { FLAPPY_X0 / FONT_TEXTURE_SIZE, FLAPPY_Y0 / FONT_TEXTURE_SIZE, FLAPPY_X0 / FONT_TEXTURE_SIZE, FLAPPY_Y2 / FONT_TEXTURE_SIZE,
          FLAPPY_X2 / FONT_TEXTURE_SIZE, FLAPPY_Y2 / FONT_TEXTURE_SIZE, FLAPPY_X2 / FONT_TEXTURE_SIZE, FLAPPY_Y0 / FONT_TEXTURE_SIZE },
        { CEMETERY_X0 / FONT_TEXTURE_SIZE, CEMETERY_Y0 / FONT_TEXTURE_SIZE, CEMETERY_X0 / FONT_TEXTURE_SIZE, CEMETERY_Y2 / FONT_TEXTURE_SIZE,
          CEMETERY_X2 / FONT_TEXTURE_SIZE, CEMETERY_Y2 / FONT_TEXTURE_SIZE, CEMETERY_X2 / FONT_TEXTURE_SIZE, CEMETERY_Y0 / FONT_TEXTURE_SIZE },
        { SKULL_X0 / FONT_TEXTURE_SIZE, SKULL_Y0 / FONT_TEXTURE_SIZE, SKULL_X0 / FONT_TEXTURE_SIZE, SKULL_Y2 / FONT_TEXTURE_SIZE,
          SKULL_X2 / FONT_TEXTURE_SIZE, SKULL_Y2 / FONT_TEXTURE_SIZE, SKULL_X2 / FONT_TEXTURE_SIZE, SKULL_Y0 / FONT_TEXTURE_SIZE },
        { GHOST_X0 / FONT_TEXTURE_SIZE, GHOST_Y0 / FONT_TEXTURE_SIZE, GHOST_X0 / FONT_TEXTURE_SIZE, GHOST_Y2 / FONT_TEXTURE_SIZE,
          GHOST_X2 / FONT_TEXTURE_SIZE, GHOST_Y2 / FONT_TEXTURE_SIZE, GHOST_X2 / FONT_TEXTURE_SIZE, GHOST_Y0 / FONT_TEXTURE_SIZE }
};

using namespace eng;

//////
class Bury : public Sprite2D {

    enum {

        PIC_DINO = 0,
        PIC_COMPUTER,
        PIC_FLAPPY,
        PIC_CEMETERY,
        PIC_SKULL,
        PIC_GHOST
    };

public:
    Bury();
    virtual ~Bury();

protected:
    void fillVerticesAnims(unsigned char animIdx);

public:
    void start();
    inline void next(float posX, bool init) {

        LOGV(LOG_LEVEL_BURY, 0, LOG_FORMAT(" - x:%f; i:%s"), __PRETTY_FUNCTION__, __LINE__, posX, (init)? "true":"false");
        reset();

        if ((init) || (++mCurAnim > (BURY_PIC_COUNT - 1)))
            mCurAnim = 0;

        animVertices(mCurAnim);
        animTexCoords(buryPicCoords, mCurAnim, false);
        translate(posX, mScreen->bottom + LAG_FROM_BOTTOM);
    }

    //////
    inline void update(const Game* game, const Level* level) { assert(NULL); }

    inline bool checkCollision(const Bounds* bounds, unsigned char boundsId) { assert(NULL); return false; }
    inline bool checkCollision(const Object* object, unsigned char objectId) { assert(NULL); return false; }

};

#endif // BURY_H_
