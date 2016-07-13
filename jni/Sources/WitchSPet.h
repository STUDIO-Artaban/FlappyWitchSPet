#ifndef WITCH_S_PET_H_
#define WITCH_S_PET_H_

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Intro/Intro.h>

#ifdef __ANDROID__
#include "Title/TitleLevel.h"
#include "Title/Scores.h"
#include "Game/GameLevel.h"
#else
#include "TitleLevel.h"
#include "Scores.h"
#include "GameLevel.h"
#endif

#define FILE_NAME       "WitchSPet.backup"

using namespace eng;

//////
class WitchSPet : public Game2D {

private:
    WitchSPet();
    virtual ~WitchSPet();

    unsigned char mLoadStep;
    Scores* mScores;

    enum {

        STEP_LOAD = 0,
        STEP_TITLE,
        STEP_SCORES,
        STEP_GAME

    };
    unsigned char mStep;

    unsigned char mColorIdx;
    inline void changeColor() {

        if (++mColorIdx == MAX_COLOR_COUNT)
            mColorIdx = 0;
    }

    Static2D* mPumpkin;
    Element2D* mProgress;
    WaitConn* mWaitNet;

    TitleLevel* mTitle;
    GameLevel* mWitchSPet;

public:
    static WitchSPet* getInstance() {
        if (!mThis)
            mThis = new WitchSPet;
        return static_cast<WitchSPet*>(mThis);
    }
    static void freeInstance() {
        if (mThis) {
            delete mThis;
            mThis = NULL;
        }
    }

    //////
    void init() { }

    bool start();
    void pause();
    void destroy();

    void wait(float millis);
    void update();

    void render() const;

};

#endif // WITCH_S_PET_H_
