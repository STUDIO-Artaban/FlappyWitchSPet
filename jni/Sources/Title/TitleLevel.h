#ifndef TITLELEVEL_H_
#define TITLELEVEL_H_

#include "Global.h"
#include "Scores.h"

#include <libeng/Game/Game.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#ifndef DEMO_VERSION
#include <libeng/Advertising/Advertising.h>
#endif

#include "Scores.h"

#ifdef __ANDROID__
#include "Sprite/TouchArea.h"
#else
#include "TouchArea.h"
#endif

#define MAX_TITLE_LOAD          12

// Start & Score
#define WIDTH_RATIO             7.f
#define CMD_X_POS               (0.5f + (2.f / WIDTH_RATIO))

using namespace eng;

//////
class TitleLevel {

private:
    unsigned char mLoadStep;
#ifndef DEMO_VERSION
    Advertising* mAdvertising;
#endif

    Element2D* mTitle;
    Element2D* mStart;
    Element2D* mScore;
    Static2D* mFace;

    float mTitleY;

    Element2D* mScores;
    Scores* mScoresData;

    TouchArea mStartArea;
    TouchArea mScoresArea;
    TouchArea mOkArea;

public:
    TitleLevel();
    virtual ~TitleLevel();

    float mWitchY;
    unsigned char mStartGame; // TRUE: Title; FALSE: Scores

    inline unsigned char getLoadStep() const { return mLoadStep; }
    inline void reset() {

        assert(mTitle);
        assert(mStart);
        assert(mScore);

        mTitle->translate(0.f, -mTitleY);
        mStart->translate(-CMD_X_POS, 0.f);
        mScore->translate(CMD_X_POS, 0.f);
    };

    //////
    inline void pause() {

        mLoadStep = 0;

        if (mTitle) mTitle->pause();
        if (mStart) mStart->pause();
        if (mScore) mScore->pause();
        if (mFace) mFace->pause();
        if (mScores) mScores->pause();
    };

    bool load(const Game* game);
    bool update(const Game* game);

    void render() const;

};

#endif // TITLELEVEL_H_
