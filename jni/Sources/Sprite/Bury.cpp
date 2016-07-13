#include "Bury.h"

//////
Bury::Bury() : Sprite2D(BURY_PIC_COUNT) {

    LOGV(LOG_LEVEL_BURY, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
}
Bury::~Bury() { LOGV(LOG_LEVEL_BURY, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__); }

void Bury::fillVerticesAnims(unsigned char animIdx) {

    LOGV(LOG_LEVEL_BURY, 0, LOG_FORMAT(" - a:%d"), __PRETTY_FUNCTION__, __LINE__, animIdx);
    static short halfW = mScreen->width >> 1;
    static short halfH = mScreen->height >> 1;

    float SR = mScreen->width / 512.f; // Screen Ratio (according 'GameLevel::mFloor' member size management)
    switch (animIdx) {
        case PIC_DINO:     positionVertices(halfW, (DINO_Y2-DINO_Y0)*SR + halfH, (DINO_X2-DINO_X0)*SR + halfW, halfH); break;
        case PIC_COMPUTER: positionVertices(halfW, (COMPUTER_Y2-COMPUTER_Y0)*SR + halfH, (COMPUTER_X2-COMPUTER_X0)*SR + halfW, halfH); break;
        case PIC_FLAPPY:   positionVertices(halfW, (FLAPPY_Y2-FLAPPY_Y0)*SR + halfH, (FLAPPY_X2-FLAPPY_X0)*SR + halfW, halfH); break;
        case PIC_CEMETERY: positionVertices(halfW, (CEMETERY_Y2-CEMETERY_Y0)*SR + halfH, (CEMETERY_X2-CEMETERY_X0)*SR + halfW, halfH); break;
        case PIC_SKULL:    positionVertices(halfW, (SKULL_Y2-SKULL_Y0)*SR + halfH, (SKULL_X2-SKULL_X0)*SR + halfW, halfH); break;
        case PIC_GHOST:    positionVertices(halfW, (GHOST_Y2-GHOST_Y0)*SR + halfH, (GHOST_X2-GHOST_X0)*SR + halfW, halfH); break;
    }

    if (animIdx == PIC_COMPUTER) centerVertices(mVertices[4] / 2.f, mVertices[1] / 2.1f);
    else centerVertices(mVertices[4] / 2.f, mVertices[1] / 2.f);
    // BUG: 2.1f and not 2.f due to a bug when displaying (a pixel lag in the texture)

    std::memcpy(mVerticesAnims[animIdx], mVertices, sizeof(float) * 8);
}

void Bury::start() {

    LOGV(LOG_LEVEL_BURY, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Sprite2D::start(TEXTURE_ID_FONT);

    for (unsigned char i = 0; i < BURY_PIC_COUNT; ++i) {

        mVerticesAnims[i] = new float[8];
        fillVerticesAnims(i);
    }
}
