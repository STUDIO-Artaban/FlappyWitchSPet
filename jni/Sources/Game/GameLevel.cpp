#include "GameLevel.h"
#include "PanelCoords.h"

#ifdef __ANDROID__
#include "Title/Scores.h"
#else
#include "Scores.h"

#include "OpenGLES/ES2/gl.h"
#endif

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Tools/Tools.h>

#include <libeng/Social/Networks/Facebook.h>
#include <libeng/Social/Networks/Twitter.h>
#include <libeng/Social/Networks/Google.h>

#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <stdlib.h>

#define MOON_GREEN              (247.f / 255.f)
#define MOON_BLUE               (84.f / 255.f)

#define BACK_TEXTURE_SIZE       1024.f
#define FONT_HEIGHT_RATIO       1.5f    // For 'GetReady' & 'GAMEOVER'
#define HAND_TOUCH_SIZE         104     // Hand touch size in texture
#define TOP_SCALE               0.94f
#define TOPA_TIP_SIZE           33      // Tip size of the topA in the panel texture (in pixel)
#define DELAY_B4_SCREAMING      60      // About 1 second

#define ALPHA_VELOCITY          0.025f

// Texture IDs
#define TEXTURE_ID_BACK         3
#define TEXTURE_ID_MOON         4
#define TEXTURE_ID_HALLOWEEN    5
#define TEXTURE_ID_TWILIGHT     6
#define TEXTURE_ID_GRAYWITCH    12

// Sound IDs
#define SOUND_ID_FLAP           1
#define SOUND_ID_TOUCHED        2
#define SOUND_ID_SCREAM1        4
#define SOUND_ID_SCREAM2        5
#define SOUND_ID_SCREAM3        6

// Advertising IDs
#define ADVERTISING_ID_SCORE    1

// Set texture coordinates of a scroll panel according its rank
inline void setPanelTexCoords(Panel* panel, unsigned char rank) {

    switch (rank) {
        case 0:
        case 2: {
            static const float texCoords[8] = { 0.f, 0.f, 0.f, 1.f, 0.5f, 1.f, 0.5f, 0.f };
            panel->setTexCoords(texCoords);
            break;
        }
        case 1: {
            static const float texCoords[8] = { 0.5f, 0.f, 0.5f, 1.f, 1.f, 1.f, 1.f, 0.f };
            panel->setTexCoords(texCoords);
            break;
        }
    }
}

#ifndef DEMO_VERSION
unsigned char GameLevel::adDisplayID = 0;
#endif

//////
GameLevel::GameLevel() : mLoadStep(0), mStep(GAME_INIT), mPauseY(0.f), mScoreX(0.f), mReadyY(0.f), mHandY(0.f),
    mPlayS(0.f), mFloorBound(NULL) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    std::memset(&mOkArea, 0, sizeof(TouchArea));

    mPlayer = Player::getInstance();
    mScoreData = Scores::getInstance(NULL);
#ifndef DEMO_VERSION
    mAdvertising = Advertising::getInstance();
#endif

    mBack = NULL;
    mMoon = NULL;
    mTwilight = NULL;
    mHalloween = NULL;
    mFloor = NULL;
    mBury = NULL;
    mWitch = NULL;
    mTowers = NULL;
    mPauseA = NULL;
    mPauseB = NULL;
    mPlay = NULL;
    mScore = NULL;
    mReady = NULL;
    mGrayWitch = NULL;
    mArrow = NULL;
    mHand = NULL;
    mTopA = NULL;
    mTopB = NULL;
    mTopC = NULL;
    mGameOver = NULL;
    mPanel = NULL;
    mShare = NULL;
}
GameLevel::~GameLevel() {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);

    if (mBack) delete mBack;
    if (mMoon) delete mMoon;
    if (mTwilight) delete mTwilight;
    if (mHalloween) delete mHalloween;
    if (mFloor) delete mFloor;
    if (mBury) delete mBury;
    if (mWitch) delete mWitch;
    if (mTowers) delete mTowers;
    if (mPauseA) delete mPauseA;
    if (mPauseB) delete mPauseB;
    if (mPlay) delete mPlay;
    if (mScore) delete mScore;
    if (mReady) delete mReady;
    if (mGrayWitch) delete mGrayWitch;
    if (mArrow) delete mArrow;
    if (mHand) delete mHand;
    if (mTopA) delete mTopA;
    if (mTopB) delete mTopB;
    if (mTopC) delete mTopC;
    if (mGameOver) delete mGameOver;
    if (mPanel) delete mPanel;
    if (mShare) delete mShare;
}

bool GameLevel::load(const Game* game) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, (mLoadStep + 1));
#ifndef DEMO_VERSION
    static bool reqAdDisplay = false;   // Avoid to display Ad again (needed coz Ad is displayed in UI thread)
                                        // -> 'Advertising::getStatus' method return value according UI thread workflow
#endif
    switch (++mLoadStep) {
        case 1: {

            if (!mTwilight) {

                mTwilight = new Scroll;
                mTwilight->assign(0, new Panel);
                mTwilight->assign(1, new Panel);
                mTwilight->assign(2, new Panel);
                mTwilight->initScroll(game2DVia(game));
                mTwilight->startScroll(TEXTURE_ID_TWILIGHT, true);

                setPanelTexCoords(panelVia((*mTwilight)[0]), 0);
                setPanelTexCoords(panelVia((*mTwilight)[1]), 1);
                setPanelTexCoords(panelVia((*mTwilight)[2]), 2);
            }
            else
                mTwilight->resumeScroll(TEXTURE_ID_TWILIGHT);
            break;
        }
        case 2: {

            if (!mBack) {

                mBack = new Static2D;
                mBack->initialize(game2DVia(game));
                mBack->start(TEXTURE_ID_BACK);
                mBack->setTexCoords(FULL_TEXCOORD_BUFFER);

                short halfWidth = (BACK_TEXTURE_SIZE / 2.f) * (1.f -
                        (game->getScreen()->width / static_cast<float>(game->getScreen()->height)));
                mBack->setVertices(-halfWidth, game->getScreen()->height, game->getScreen()->width + halfWidth, 0);
            }
            else
                mBack->resume(TEXTURE_ID_BACK);
            break;
        }
        case 3: {

            if (!mMoon) {

                mMoon = new Static2D;
                mMoon->initialize(game2DVia(game));
                mMoon->start(TEXTURE_ID_MOON);
                mMoon->setTexCoords(FULL_TEXCOORD_BUFFER);
                mMoon->setVertices((game->getScreen()->width - (game->getScreen()->height >> 1)) >> 1, game->getScreen()->height,
                        game->getScreen()->width - ((game->getScreen()->width - (game->getScreen()->height >> 1)) >> 1),
                        game->getScreen()->height >> 1);

                //mMoon->setRed(1.f);
                mMoon->setGreen(MOON_GREEN);
                mMoon->setBlue(MOON_BLUE);
            }
            else
                mMoon->resume(TEXTURE_ID_MOON);
            break;
        }
        case 4: Textures::getInstance()->genTexture(Textures::getInstance()->loadTexture(Facebook::TEXTURE_ID)); break;
        case 5: Textures::getInstance()->genTexture(Textures::getInstance()->loadTexture(Twitter::TEXTURE_ID)); break;
        case 6: Textures::getInstance()->genTexture(Textures::getInstance()->loadTexture(Google::TEXTURE_ID)); break;
        case 7: {

            if (!mShare) {

                mShare = new Share;
                mShare->start(game);
            }
            else
                mShare->resume();
            break;
        }
        case 8: {

            if (!mHalloween) {

                mHalloween = new Scroll;
                mHalloween->assign(0, new Panel);
                mHalloween->assign(1, new Panel);
                mHalloween->assign(2, new Panel);
                mHalloween->initScroll(game2DVia(game));
                mHalloween->startScroll(TEXTURE_ID_HALLOWEEN, true);

                setPanelTexCoords(panelVia((*mHalloween)[0]), 0);
                setPanelTexCoords(panelVia((*mHalloween)[1]), 1);
                setPanelTexCoords(panelVia((*mHalloween)[2]), 2);
            }
            else
                mHalloween->resumeScroll(TEXTURE_ID_HALLOWEEN);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17: {

            if (!mWitch) {

                mWitch = new Witch;
                mWitch->initialize(game2DVia(game));
                mWitch->setTexCoords(FULL_TEXCOORD_BUFFER);
            }
            mWitch->load(mLoadStep - 9);
            break;
        }
        case 18: {

            if (!mPauseA) {

                mPauseA = new Element2D;
                mPauseA->initialize(game2DVia(game));
                mPauseA->start(TEXTURE_ID_PANEL);

                static const float texCoords[8] = { PAUSE_A_X0 / PANEL_TEXTURE_SIZE, PAUSE_A_Y0 / PANEL_TEXTURE_SIZE,
                        PAUSE_A_X0 / PANEL_TEXTURE_SIZE, (PAUSE_A_Y0 + PAUSE_SIZE) / PANEL_TEXTURE_SIZE,
                        (PAUSE_A_X0 + PAUSE_SIZE) / PANEL_TEXTURE_SIZE, (PAUSE_A_Y0 + PAUSE_SIZE) / PANEL_TEXTURE_SIZE,
                        (PAUSE_A_X0 + PAUSE_SIZE) / PANEL_TEXTURE_SIZE, PAUSE_A_Y0 / PANEL_TEXTURE_SIZE };
                mPauseA->setTexCoords(texCoords);

                short halfSize = ((game->getScreen()->width >> 4) * game->getScreen()->height *
                        REF_SCREEN_RATIO) / game->getScreen()->width;
                mPauseA->setVertices((game->getScreen()->width >> 1) - halfSize, (game->getScreen()->height >> 1) + halfSize,
                        (game->getScreen()->width >> 1) + halfSize, (game->getScreen()->height >> 1) - halfSize);

                mPauseY = (mPauseA->getBottom() - mPauseA->getTop()) - (game->getScreen()->top / 32.f);
            }
            else
                mPauseA->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 19: {

            if (!mPauseB) {

                mPauseB = new Element2D;
                mPauseB->initialize(game2DVia(game));
                mPauseB->start(TEXTURE_ID_PANEL);
                mPauseB->copyVertices(*mPauseA);
                mPauseB->scale(0.9f, 0.9f);

                static const float texCoords[8] = { PAUSE_B_X0 / PANEL_TEXTURE_SIZE, 0.f,
                        PAUSE_B_X0 / PANEL_TEXTURE_SIZE, PAUSE_SIZE / PANEL_TEXTURE_SIZE,
                        (PAUSE_B_X0 + PAUSE_SIZE) / PANEL_TEXTURE_SIZE, PAUSE_SIZE / PANEL_TEXTURE_SIZE,
                        (PAUSE_B_X0 + PAUSE_SIZE) / PANEL_TEXTURE_SIZE, 0.f };
                mPauseB->setTexCoords(texCoords);
            }
            else
                mPauseB->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 20: {

            if (!mScore) {

                mScore = new Text2D;
                mScore->initialize(game2DVia(game));
                mScore->start(L"0");

                float scale = ((static_cast<float>(game->getScreen()->width >> 3) / SCORE_FONT_HEIGHT) *
                               game->getScreen()->height * REF_SCREEN_RATIO) / game->getScreen()->width;
                mScore->scale(scale, scale);

                float halfWidth = (mScore->getRight() - mScore->getLeft()) / 2.f;
                mScore->position(-halfWidth, game->getScreen()->top + (mPauseA->getTop() - mPauseA->getBottom()) + mPauseY);
                mScoreX = game->getScreen()->left - halfWidth;
            }
            else
                mScore->resume();
            break;
        }
        case 21: {

            if (!mReady) {

                mReady = new Element2D;
                mReady->initialize(game2DVia(game));
                mReady->start(TEXTURE_ID_FONT);

                static const float texCoords[8] = { 0.f, READY_Y0 / FONT_TEXTURE_SIZE, 0.f, READY_Y2 / FONT_TEXTURE_SIZE,
                        READY_X2 / FONT_TEXTURE_SIZE, READY_Y2 / FONT_TEXTURE_SIZE,
                        READY_X2 / FONT_TEXTURE_SIZE, READY_Y0 / FONT_TEXTURE_SIZE };
                mReady->setTexCoords(texCoords);

                short halfWidth = game->getScreen()->width >> 2;
                short height = ((halfWidth << 1) * (READY_Y2 - READY_Y0) * FONT_HEIGHT_RATIO) / READY_X2;
                mReady->setVertices((game->getScreen()->width >> 1) - halfWidth, game->getScreen()->height + height,
                        (game->getScreen()->width >> 1) + halfWidth, game->getScreen()->height);

                mReadyY = (game->getScreen()->bottom / 3.f) - ((mReady->getTop() - mReady->getBottom()) / 2.f);
            }
            else
                mReady->resume(TEXTURE_ID_FONT);
            break;
        }
        case 22: {

            if (!mHand) {

                mHand = new Element2D;
                mHand->initialize(game2DVia(game));
                mHand->start(TEXTURE_ID_PANEL);

                static const float texCoords[8] = { HAND_X0 / PANEL_TEXTURE_SIZE, 0.f,
                        HAND_X0 / PANEL_TEXTURE_SIZE, HAND_Y2 / PANEL_TEXTURE_SIZE,
                        HAND_X2 / PANEL_TEXTURE_SIZE, HAND_Y2 / PANEL_TEXTURE_SIZE,
                        HAND_X2 / PANEL_TEXTURE_SIZE, 0.f };
                mHand->setTexCoords(texCoords);

                short height = (HAND_Y2 * (((game->getScreen()->width >> 3) * game->getScreen()->height *
                        REF_SCREEN_RATIO) / game->getScreen()->width)) / HAND_TOUCH_SIZE; // HAND_TOUCH_SIZE == 'mPauseA' height
                short halfWidth = ((HAND_X2 - HAND_X0) * (height >> 1)) / HAND_Y2;
                mHand->setVertices((game->getScreen()->width >> 1) - halfWidth, 0, (game->getScreen()->width >> 1) + halfWidth,
                        -height);

                mHandY = (game->getScreen()->top * 2.f) + mReadyY - ((mWitch->getTop() -
                        mWitch->getBottom()) * (WITCH_SCALE_RATIO * game->getScreen()->height / game->getScreen()->width));
            }
            else
                mHand->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 23: {

            if (!mTopA) {

                mTopA = new Element2D;
                mTopA->initialize(game2DVia(game));
                mTopA->start(TEXTURE_ID_PANEL);

                static const float texCoords[8] = { TOP_A_X0 / PANEL_TEXTURE_SIZE, 0.f,
                        TOP_A_X0 / PANEL_TEXTURE_SIZE, TOP_A_Y2 / PANEL_TEXTURE_SIZE,
                        TOP_A_X2 / PANEL_TEXTURE_SIZE, TOP_A_Y2 / PANEL_TEXTURE_SIZE,
                        TOP_A_X2 / PANEL_TEXTURE_SIZE, 0.f };
                mTopA->setTexCoords(texCoords);

                short halfHeight = static_cast<short>((game->getScreen()->width >> 4) * game->getScreen()->height *
                        REF_SCREEN_RATIO) / game->getScreen()->width;
                short width = ((TOP_A_X2 - TOP_A_X0) * (halfHeight << 1)) / TOP_A_Y2;
                mTopA->setVertices(game->getScreen()->width, game->getScreen()->height >> 1,
                        game->getScreen()->width + width, (game->getScreen()->height >> 1) - (halfHeight << 1));
            }
            else
                mTopA->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 24: {

            if (!mTopB) {

                mTopB = new Element2D;
                mTopB->initialize(game2DVia(game));
                mTopB->start(TEXTURE_ID_FONT);

                static const float texCoords[8] = { 0.f, TOP_B_Y0 / FONT_TEXTURE_SIZE, 0.f, TOP_B_Y2 / FONT_TEXTURE_SIZE,
                        TOP_B_X2 / FONT_TEXTURE_SIZE, TOP_B_Y2 / FONT_TEXTURE_SIZE,
                        TOP_B_X2 / FONT_TEXTURE_SIZE, TOP_B_Y0 / FONT_TEXTURE_SIZE };
                mTopB->setTexCoords(texCoords);

                short halfHeight = static_cast<short>((((game->getScreen()->width >> 4) * game->getScreen()->height *
                        REF_SCREEN_RATIO) / game->getScreen()->width) * TOP_SCALE);
                short halfWidth = (TOP_B_X2 * halfHeight) / (TOP_B_Y2 - TOP_B_Y0);
                mTopB->setVertices((game->getScreen()->width >> 1) - halfWidth, (game->getScreen()->height >> 1) + halfHeight,
                        (game->getScreen()->width >> 1) + halfWidth, (game->getScreen()->height >> 1) - halfHeight);

                mTopB->scale(TOP_SCALE, TOP_SCALE);
            }
            else
                mTopB->resume(TEXTURE_ID_FONT);
            break;
        }
        case 25: {

            if (!mTopC) {

                mTopC = new Element2D;
                mTopC->initialize(game2DVia(game));
                mTopC->start(TEXTURE_ID_PUMPKIN);
                mTopC->setTexCoords(FULL_TEXCOORD_BUFFER);

                short halfHeight = static_cast<short>((((game->getScreen()->width >> 4) * game->getScreen()->height *
                        REF_SCREEN_RATIO) / game->getScreen()->width) * TOP_SCALE);
                mTopC->setVertices((game->getScreen()->width >> 1) - halfHeight, (game->getScreen()->height >> 1) + halfHeight,
                        (game->getScreen()->width >> 1) + halfHeight, (game->getScreen()->height >> 1) - halfHeight);

                mTopC->scale(TOP_SCALE, TOP_SCALE);
            }
            else
                mTopC->resume(TEXTURE_ID_PUMPKIN);
            break;
        }
        case 26: {

            if (!mArrow) {

                mArrow = new Element2D;
                mArrow->initialize(game2DVia(game));
                mArrow->start(TEXTURE_ID_PANEL);

                static const float texCoords[8] = { ARROW_X0 / PANEL_TEXTURE_SIZE, 0.f,
                        ARROW_X0 / PANEL_TEXTURE_SIZE, ARROW_Y2 / PANEL_TEXTURE_SIZE, 1.f, ARROW_Y2 / PANEL_TEXTURE_SIZE,
                        1.f, 0.f };
                mArrow->setTexCoords(texCoords);

                short halfHeight = ((game->getScreen()->width >> 4) * game->getScreen()->height *
                        REF_SCREEN_RATIO) / game->getScreen()->width;
                short halfWidth = ((PANEL_TEXTURE_SIZE - ARROW_X0) * halfHeight) / ARROW_Y2;
                mArrow->setVertices((game->getScreen()->width >> 1) - halfWidth, (game->getScreen()->height >> 1) + halfHeight,
                        (game->getScreen()->width >> 1) + halfWidth, (game->getScreen()->height >> 1) - halfHeight);
            }
            else
                mArrow->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 27: {

            if (!mFloor) {

                mFloor = new Scroll;
                mFloor->assign(0, new Panel);
                mFloor->assign(1, new Panel);
                mFloor->assign(2, new Panel);
                mFloor->initScroll(game2DVia(game));
                mFloor->startScroll(TEXTURE_ID_FLOOR, true);

                reset(game->getScreen()->bottom, game->getScreen()->width);
                mFloorBound = new BorderBottom(game->getScreen()->bottom + (((mFloor->getScrollTop() -
                        mFloor->getScrollBottom()) * FLOOR_HEIGHT) / FLOOR_TEXTURE_HEIGHT));
            }
            else
                mFloor->resumeScroll(TEXTURE_ID_FLOOR);
            break;
        }
        case 28: {

            if (!mPlay) {

                mPlay = new Static2D;
                mPlay->initialize(game2DVia(game));
                mPlay->start(TEXTURE_ID_PANEL);
                mPlay->setAlpha(0.f);
                mPlayS = ((PLAY_SCALE * game->getScreen()->width) / (REF_SCREEN_RATIO * game->getScreen()->height));

                static const float texCoords[8] = { PLAY_X0 / PANEL_TEXTURE_SIZE, PLAY_Y0 / PANEL_TEXTURE_SIZE,
                        PLAY_X0 / PANEL_TEXTURE_SIZE, PLAY_Y2 / PANEL_TEXTURE_SIZE,
                        PLAY_X2 / PANEL_TEXTURE_SIZE, PLAY_Y2 / PANEL_TEXTURE_SIZE,
                        PLAY_X2 / PANEL_TEXTURE_SIZE, PLAY_Y0 / PANEL_TEXTURE_SIZE };
                mPlay->setTexCoords(texCoords);

                static const float verCoords[8] = { -0.45f, 0.45f, -0.45f, -0.45f, 0.45f, -0.45f, 0.45f, 0.45f };
                mPlay->setVertices(verCoords);
            }
            else
                mPlay->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 29: Player::loadSound(SOUND_ID_FLAP); break;
        case 30: Player::loadSound(SOUND_ID_TOUCHED); break;
        case 31: {

            Player::loadSound(SOUND_ID_SCREAM1);
            mPlayer->setVolume(2, 0.25f);
            break;
        }
        case 32: {

            Player::loadSound(SOUND_ID_SCREAM2);
            mPlayer->setVolume(3, 0.25f);
            break;
        }
        case 33: {

            Player::loadSound(SOUND_ID_SCREAM3);
            mPlayer->setVolume(4, 0.25f);
            break;
        }
        case 34: Player::loadSound(SOUND_ID_SCORE); break;
        case 35: {

            if (!mPanel) {

                mPanel = new Element2D;
                mPanel->initialize(game2DVia(game));
                mPanel->start(TEXTURE_ID_PANEL);

                short height = ((static_cast<short>(PANEL_TEXTURE_SIZE) - PANEL_Y0) *
                        game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE);
                mPanel->setVertices(0, 0, game->getScreen()->width, -height);

                mOkArea.left = game->getScreen()->width - static_cast<short>(((PANEL_TEXTURE_SIZE - OK_X0) *
                        game->getScreen()->width) / PANEL_TEXTURE_SIZE);
                mOkArea.right = game->getScreen()->width;
                mOkArea.top = (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) +
                        (((OK_Y0 - PANEL_Y0) * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE));
                mOkArea.bottom = (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) +
                        (((OK_Y2 - PANEL_Y0) * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE));

                static const float texCoords[8] = { 0.f, PANEL_Y0 / PANEL_TEXTURE_SIZE, 0.f, 1.f, 1.f, 1.f,
                        1.f, PANEL_Y0 / PANEL_TEXTURE_SIZE };
                mPanel->setTexCoords(texCoords);
            }
            else
                mPanel->resume(TEXTURE_ID_PANEL);
            break;
        }
        case 36: {

            if (!mBury) {

                mBury = new Bury;
                mBury->initialize(game2DVia(game));
                mBury->start();
            }
            else
                mBury->resume(TEXTURE_ID_FONT);
            break;
        }
        case 37: {

            if (!mGameOver) {

                mGameOver = new Element2D;
                mGameOver->initialize(game2DVia(game));
                mGameOver->start(TEXTURE_ID_FONT);

                short halfWidth = static_cast<short>(0.8f * game->getScreen()->width) >> 1; // 80% of width
                short halfHeight = static_cast<short>(((FONT_TEXTURE_SIZE - GAMEOVER_Y0) / FONT_TEXTURE_SIZE) *
                        (halfWidth << 1));
                mGameOver->setVertices((game->getScreen()->width >> 1) - halfWidth,
                        game->getScreen()->height + (halfHeight << 1), (game->getScreen()->width >> 1) + halfWidth,
                        game->getScreen()->height);

                static const float texCoords[8] = { 0.f, GAMEOVER_Y0 / FONT_TEXTURE_SIZE, 0.f, 1.f, 1.f, 1.f,
                        1.f, GAMEOVER_Y0 / FONT_TEXTURE_SIZE };
                mGameOver->setTexCoords(texCoords);
            }
            else {

                mGameOver->resume(TEXTURE_ID_FONT);

                if (mTowers)
                    mTowers->resume();
                if (mGrayWitch)
                    mGrayWitch->resume(TEXTURE_ID_GRAYWITCH);
                // -> Done here coz should be the last texure loaded
            }
#ifndef DEMO_VERSION
            reqAdDisplay = false;
#endif
            break;
        }
        case MAX_GAME_LOAD: {

            if ((mStep != GAME_INIT) && (mStep < GAME_SCREAMING))
                break;

#ifndef DEMO_VERSION
            unsigned char adStatus = Advertising::getStatus();
            assert(adStatus != Advertising::STATUS_NONE);
            assert(adStatus != Advertising::STATUS_READY);
            assert(adStatus != Advertising::STATUS_LOADING);
            if ((adStatus == Advertising::STATUS_FAILED) || (adStatus == Advertising::STATUS_DISPLAYED)) {

                mAdvertising->resetPubDelay();
                break;
            }

            --mLoadStep;
            if ((adStatus == Advertising::STATUS_DISPLAYING) || (reqAdDisplay))
                boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            else if (mStep > GAME_SCREAMING) {
                if (reqAdDisplay = Advertising::display(ADVERTISING_ID_SCORE))
                    adDisplayID = ADVERTISING_ID_SCORE;
            }
            else if (reqAdDisplay = Advertising::display(ADVERTISING_ID_TITLE))
                adDisplayID = ADVERTISING_ID_TITLE;
#endif
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown load step: %d"), __PRETTY_FUNCTION__, __LINE__, mLoadStep);
            assert(NULL);
            break;
        }
#endif
    }
    return (mLoadStep == MAX_GAME_LOAD);
}

void GameLevel::terminate() {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    delete mGrayWitch;
    Textures::getInstance()->rmvTextures(1);
    mGrayWitch = NULL;

    delete mTowers;
    Textures::getInstance()->rmvTextures(2);
    mTowers = NULL;

    mReady->setAlpha(1.f);
    mArrow->setAlpha(1.f);
    mHand->setAlpha(1.f);
    mTopA->setAlpha(1.f);
    mTopB->setAlpha(1.f);
    mTopC->setAlpha(1.f);

    mTwilight->resetScroll();
    setPanelTexCoords(panelVia((*mTwilight)[0]), panelVia((*mTwilight)[0])->getRank());
    setPanelTexCoords(panelVia((*mTwilight)[1]), panelVia((*mTwilight)[1])->getRank());
    setPanelTexCoords(panelVia((*mTwilight)[2]), panelVia((*mTwilight)[2])->getRank());
    mHalloween->resetScroll();
    setPanelTexCoords(panelVia((*mHalloween)[0]), panelVia((*mHalloween)[0])->getRank());
    setPanelTexCoords(panelVia((*mHalloween)[1]), panelVia((*mHalloween)[1])->getRank());
    setPanelTexCoords(panelVia((*mHalloween)[2]), panelVia((*mHalloween)[2])->getRank());
    mFloor->resetScroll();

    mScoreData->mDispTitle = true;
    mScoreData->update(); // Update scores for title level

#ifndef DEMO_VERSION
    unsigned char adStatus = Advertising::getStatus();
    if ((adStatus == Advertising::STATUS_DISPLAYED) || (adStatus == Advertising::STATUS_FAILED)) {

        Advertising::hide(ADVERTISING_ID_SCORE);
        if (Advertising::display(ADVERTISING_ID_TITLE))
            adDisplayID = ADVERTISING_ID_TITLE;
    }
#endif
    mWitch->terminate();
    mStep = GAME_INIT;
}

void GameLevel::increaseScore() {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - s:%d"), __PRETTY_FUNCTION__, __LINE__, (mScoreData->mScore + 1));
    assert(mPlayer->getIndex(SOUND_ID_SCORE) == 5);

    mScore->update(boost::str(boost::wformat(L"%d") % ++mScoreData->mScore));
    mPlayer->play(5);

    if ((mScoreData->mScore == 10) || (mScoreData->mScore == 100) || (mScoreData->mScore == 1000) ||
            (mScoreData->mScore == 10000))
        mScore->position(mScore->getLeft() + mScoreX + 1.f, mScore->getTop());
}

bool GameLevel::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_GAMELEVEL, (*game->getLog() % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
#endif
    static unsigned char updateCount = 0;
    static unsigned char screamIdx = 2;
    static bool checkScore = true;

#ifdef DEMO_VERSION
    static bool freeze = false;
    if (game->mTouchCount > 1)
        freeze = true;
    if (freeze)
        return true;
#endif

    mPlayer->resume();
    switch (mStep) {

        case GAME_INIT: {

            LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - GAME_INIT"), __PRETTY_FUNCTION__, __LINE__);
            assert(!mGrayWitch);
            assert(!mTowers);

            mTowers = new Towers;
            mTowers->start(game, mScore->getBottom(), mFloorBound->mBottom);

            mBury->next(static_cast<const BorderLeft*>(mTowers->get()->getRightBound())->mLeft + (TOWER_H_LAG / 2.f), true);
            mShare->reset();

            mGrayWitch = new Element2D;
            mGrayWitch->initialize(game2DVia(game));
            mGrayWitch->start(TEXTURE_ID_GRAYWITCH);
            mGrayWitch->setTexCoords(FULL_TEXCOORD_BUFFER);
            mGrayWitch->copyVertices(*mWitch);
            mGrayWitch->rotate(PI_F / 4.f);

            float scaleWitch = WITCH_SCALE_RATIO * game->getScreen()->height / game->getScreen()->width;
            float grayY = game->getScreen()->top + mReadyY - (0.5f * scaleWitch);
            mGrayWitch->scale(scaleWitch, scaleWitch);
            mGrayWitch->translate(game->getScreen()->right + 0.5f, grayY);
            mGrayWitch->setAlpha(1.f);

            float pauseSize = (mPauseA->getTop() - mPauseA->getBottom()) / 2.f; // Half size
            mPauseB->reset();
            mPauseB->translate(1.f - ((mPauseB->getRight() - mPauseB->getLeft()) / 2.f) - (game->getScreen()->top / 32.f),
                    game->getScreen()->top + pauseSize);
            mPauseA->reset();
            mPauseA->translate(1.f - ((mPauseA->getRight() - mPauseA->getLeft()) / 2.f) - (game->getScreen()->top / 32.f),
                    game->getScreen()->top + pauseSize);
            mReady->reset();
            mHand->reset();
            mArrow->reset();

            float witchY = grayY - ((0.5f * scaleWitch) / 2);
            mArrow->translate(game->getScreen()->right +
                    ((0.2f * game->getScreen()->height * REF_SCREEN_RATIO) / game->getScreen()->width) +
                              0.45f + ((mArrow->getRight() - mArrow->getLeft()) / 2.f), witchY);
            mTopA->reset();
            mTopA->translate(0.f, mHandY - game->getScreen()->top);
            mTopB->reset();

            float tipWidth = (TOPA_TIP_SIZE * (mPauseB->getTop() - mPauseB->getBottom())) / TOP_A_Y2;
            mTopB->translate(game->getScreen()->right + ((mTopB->getRight() - mTopB->getLeft()) / 2.f) + tipWidth,
                    mHandY - game->getScreen()->top - pauseSize);
            mTopC->reset();
            mTopC->translate(game->getScreen()->right + ((mTopB->getRight() - mTopB->getLeft()) / 2.f) + tipWidth,
                    mHandY - game->getScreen()->top - pauseSize);

            mWitch->prepare();
            mWitch->scale(scaleWitch, scaleWitch);
            mWitch->translate(game->getScreen()->left - 0.5f, grayY - ((0.5f * scaleWitch) / 2));

            if (mScoreData->mScore) {

                mScore->update(L"0");
                mScore->position(mScoreX + 1.f, mScore->getTop());
                mScoreData->mScore = 0;
            }
            mScore->translate(game->getScreen()->right + ((mScore->getRight() - mScore->getLeft()) / 2.f), 0.f);

            mGameOver->reset();
            mPanel->reset();
            mScoreData->mDispTitle = false;

            mStep = GAME_READY;
            updateCount = 0;
            return true;
        }
        case GAME_READY: {

            mWitch->update(game, NULL);

            if (updateCount != (DISP_UPD_COUNT - 1)) {
            //if (mPauseA->getTransform()[Dynamic2D::TRANS_Y] > mPauseY) {
            // -> Avoid precision error that was previously fixed by assigning: pauseVel = mPauseY / (DISP_UPD_COUNT - 2);

                static float pauseVel = mPauseY / (DISP_UPD_COUNT - 1); // "- 1" coz no move in GAME_INIT
                static float scoreVel = mScoreX / (DISP_UPD_COUNT - 1);
                static float readyVel = mReadyY / (DISP_UPD_COUNT - 1);
                static float grayVel = (1.f + ((mWitch->getRight() - mWitch->getLeft()) / 2.f)) / (1 - DISP_UPD_COUNT);
                static float witchVel = 1.f / (DISP_UPD_COUNT - 1);
                static float handVel = mHandY / (DISP_UPD_COUNT - 1);
                static float topVel = (((mPauseA->getRight() - mPauseA->getLeft()) / 2.f) - 1.f) / (DISP_UPD_COUNT - 1);

                assert(mGrayWitch);
                mPauseA->translate(0.f, pauseVel);
                mPauseB->translate(0.f, pauseVel);
                mScore->translate(scoreVel, 0.f);
                mReady->translate(0.f, readyVel);
                mGrayWitch->translate(grayVel, 0.f);
                mArrow->translate(grayVel, 0.f);
                mWitch->translate(witchVel, 0.f);
                mHand->translate(0.f, handVel);
                mTopA->translate(topVel, 0.f);
                mTopB->translate(topVel, 0.f);
                mTopC->translate(topVel, 0.f);

                ++updateCount;
                return true;
            }
            break; // Ready
        }
        case GAME_STARTING: {

            if (mReady->getAlpha() > 0.f) {

                mReady->setAlpha(mReady->getAlpha() - ALPHA_VELOCITY);
                mGrayWitch->setAlpha(mReady->getAlpha());
                mArrow->setAlpha(mReady->getAlpha());
                mHand->setAlpha(mReady->getAlpha());
                mTopA->setAlpha(mReady->getAlpha());
                mTopB->setAlpha(mReady->getAlpha());
                mTopC->setAlpha(mReady->getAlpha());

                mWitch->play(false);
            }
            else {

                mWitch->play(true);
                mStep = GAME_STARTED;
                checkScore = true;
            }
            //break;
        }
        case GAME_STARTED: {

            moveWorld();
            mWitch->update(game, NULL);
            if (mTowers->update(mScoreData->mScore) == 1)
                mBury->next(static_cast<const BorderLeft*>(mTowers->get()->getRightBound())->mLeft +
                        (3.f * TOWER_H_LAG / 2.f), false);

            // Check collisions
            if ((mWitch->checkCollision(mFloorBound, BOUND_ID_FLOOR)) || // Floor
                (mWitch->checkCollision(mTowers->get()->getTop()->getBounds(0), BOUND_ID_TOPHEAD)) || // Head of top tower
                (mWitch->checkCollision(mTowers->get()->getBottom()->getBounds(0), BOUND_ID_BOTHEAD)) || // Head of bottom tower
                    ((!mWitch->checkCollision(mTowers->get()->getEndBound(), BOUND_ID_SCORE)) && // Tower has not been passed yet
                     (mWitch->checkCollision(mTowers->get()->getLeftBound(), BOUND_ID_TOWER)) && // Tower (on top/on bottom)...
                            (mWitch->checkCollision(mTowers->get()->getTopBound(), BOUND_ID_TOP) ^ // ...not between
                             mWitch->checkCollision(mTowers->get()->getBottomBound(), BOUND_ID_BOTTOM)))) { // ...head of towers

                mPlayer->play(1);
                mStep = GAME_OVER;
                updateCount = 0;
                break;
            }

            if (!mWitch->checkCollision(mTowers->get()->getRightBound(), BOUND_ID_PASSED)) {
                mTowers->next(); // Witch has entire passed the current towers
                checkScore = true;
            }
            if ((checkScore) && (mWitch->checkCollision(mTowers->get()->getEndBound(), BOUND_ID_SCORE))) {
                increaseScore(); // Witch is now between current towers (increase score)
                checkScore = false;
            }
            break;
        }
        case GAME_PAUSE: {

            if ((mPlay->getAlpha() < 1.f) && (updateCount < DISP_UPD_COUNT)) {

                static float playVelX = (1.f - ((mPauseA->getRight() - mPauseA->getLeft()) / 2.f) -
                        (game->getScreen()->top / 32.f)) / -DISP_UPD_COUNT;
                static float playVelY = (game->getScreen()->top - (game->getScreen()->top / 32.f) + ((mPauseA->getBottom() -
                        mPauseA->getTop()) / 2.f)) / -DISP_UPD_COUNT;
                static float scaleVel = (mPlayS - 1.f) / DISP_UPD_COUNT;

                mPauseA->translate(playVelX, playVelY);
                mPauseA->scale(mPauseA->getTransform()[Dynamic2D::SCALE_X] + scaleVel,
                        mPauseA->getTransform()[Dynamic2D::SCALE_Y] + scaleVel);

                if (++updateCount == DISP_UPD_COUNT)
                    mPlay->setAlpha(1.f);
            }
            break;
        }
        case GAME_OVER: {

            if (++updateCount == DELAY_B4_SCREAMING) {

                if (++screamIdx > 4)
                    screamIdx = 2;
                mPlayer->play(screamIdx);
                mWitch->scream();
                mStep = GAME_SCREAMING;
                updateCount = 0;
            }
            return true;
        }
        case GAME_SCREAMING: {

            if (updateCount < DELAY_B4_SCREAMING)   // Scream sound duration must be > DELAY_B4_SCREAMING
                ++updateCount;              // -> Avoid to display score without having played this sound (see condition below)

            mWitch->update(game, NULL);
            if ((mPlayer->getStatus(screamIdx) == AL_STOPPED) && (updateCount == DELAY_B4_SCREAMING)) {

#ifndef DEMO_VERSION
                if (Advertising::getStatus() == Advertising::STATUS_LOADED)
                    if (Advertising::display(ADVERTISING_ID_SCORE))
                        adDisplayID = ADVERTISING_ID_SCORE;
#endif
                updateCount = 0;
                mScoreData->prepare();
                mStep = GAME_SCORING;
            }
            return true;
        }
        case GAME_SCORING: {

#ifndef DEMO_VERSION
            mAdvertising->refresh();
#endif
            if (updateCount < DISP_UPD_COUNT) { // Display score panel & GameOver

                static float panelVel = (game->getScreen()->top + 0.5f) / DISP_UPD_COUNT;
                static float overVel = (((0.5f - game->getScreen()->top) / 2.f) - ((mGameOver->getTop() -
                        mGameOver->getBottom()) / 2.f)) / DISP_UPD_COUNT;

                mPanel->translate(0.f, panelVel);
                mGameOver->translate(0.f, overVel);
                ++updateCount;
                return true;
            }

            mScoreData->show();
            if (mShare->update(game)) {

                updateCount = 0;
                mScoreData->hide();
                mShare->setScore(mScoreData->mScore);
                mStep = GAME_SHARE;
                break;
            }
            bool displayed = mScoreData->update();
            if (mScoreData->isBestScore())
                mShare->display(); // Display share button

            if (displayed)
                return true; // Do not manage 'ok' click yet when displaying game score

            break;
        }
        case GAME_SHARE: {

#ifndef DEMO_VERSION
            mAdvertising->refresh();
#endif
            if (mShare->update(game)) {

                updateCount = 0;
                mShare->display(); // Display share button
                mStep = GAME_SCORING;
                return true;
            }
            if (updateCount < DISP_UPD_COUNT) { // Hide score panel & GameOver

                static float panelVel = (game->getScreen()->top + 0.5f) / DISP_UPD_COUNT;
                static float overVel = (((0.5f - game->getScreen()->top) / 2.f) - ((mGameOver->getTop() -
                        mGameOver->getBottom()) / 2.f)) / DISP_UPD_COUNT;

                mPanel->translate(0.f, -panelVel);
                mGameOver->translate(0.f, -overVel);
                ++updateCount;
            }
            return true;
        }
    }

    //////
    unsigned char touchCount = game->mTouchCount;
    while (touchCount--) {

        if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
            continue;

        switch (mStep) {

            case GAME_READY: mStep = GAME_STARTING; break;
            case GAME_STARTED: {

                // Check pause
                static short top = game->getScreen()->height >> 6;
                static short left = game->getScreen()->width - (((game->getScreen()->width >> 3) *
                        game->getScreen()->height * REF_SCREEN_RATIO) / game->getScreen()->width) - top;
                static short right = game->getScreen()->width - top;
                static short bottom = top + (((game->getScreen()->width >> 3) * game->getScreen()->height *
                        REF_SCREEN_RATIO) / game->getScreen()->width);

                if ((game->mTouchData[touchCount].X > left) && (game->mTouchData[touchCount].X < right) &&
                        (game->mTouchData[touchCount].Y > top) && (game->mTouchData[touchCount].Y < bottom)) {

                    updateCount = 0;
                    mStep = GAME_PAUSE;
                }
                else
                    mPlayer->play(0);
                break;
            }
            case GAME_PAUSE: {

                // Check play
                static short left = game->getScreen()->width >> 2;
                static short right = 3 * left;
                static short bottom = (game->getScreen()->height >> 1) + left;
                static short top = (game->getScreen()->height >> 1) - left;

                if ((game->mTouchData[touchCount].X > left) && (game->mTouchData[touchCount].X < right) &&
                        (game->mTouchData[touchCount].Y > top) && (game->mTouchData[touchCount].Y < bottom)) {

                    mPauseA->reset();
                    mPauseA->scale(1.f, 1.f);
                    mPlay->setAlpha(0.f);
                    mStep = GAME_STARTED;

                    float halfSize = (mPauseA->getRight() - mPauseA->getLeft()) / 2.f;
                    mPauseA->translate(1.f - halfSize - (game->getScreen()->top / 32.f),
                            game->getScreen()->top + halfSize + mPauseY);
                }
                break;
            }
            case GAME_SCORING: {

#ifndef DEMO_VERSION
                unsigned char adStatus = Advertising::getStatus();
#ifdef DEBUG
                if ((adStatus == Advertising::STATUS_LOADING) || (adStatus == Advertising::STATUS_DISPLAYING)) {

                    LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Do not start when loading/displaying advertising"),
                            __PRETTY_FUNCTION__, __LINE__);
                    break; // Do not hide advertising if loading/displaying (especially if loading)
                }
#else
                if ((adStatus == Advertising::STATUS_LOADING) || (adStatus == Advertising::STATUS_DISPLAYING))
                    break;
#endif
#endif
                if (mShare->isDisplaying())
                    break;
                // Check ok
                if ((game->mTouchData[touchCount].X > mOkArea.left) && (game->mTouchData[touchCount].X < mOkArea.right) &&
                        (game->mTouchData[touchCount].Y > mOkArea.top) && (game->mTouchData[touchCount].Y < mOkArea.bottom)) {

                    terminate();
                    return false;
                }
            }
        }
    }
    return true;
}

void GameLevel::renderBackground() const {

    glDisable(GL_BLEND);
    mBack->render(false);
    glEnable(GL_BLEND);

    mMoon->render(false);
    mTwilight->renderScroll(true);
    mHalloween->renderScroll(true);
    if (mTowers)
        mTowers->render();
    mFloor->renderScroll(true);
}
void GameLevel::render() const {

    switch (mStep) {

        case GAME_READY:
        case GAME_STARTING: {

            renderBackground();
            mBury->render(true);

            mReady->render(true);

            mPauseA->render(true);
            mPauseB->render(true);
            mScore->render(true);

            assert(mGrayWitch);
            mGrayWitch->render(true);
            mArrow->render(true);
            mWitch->render(true);
            mHand->render(true);
            mTopA->render(true);
            mTopB->render(true);
            mTopC->render(true);
            break;
        }
        case GAME_STARTED:
        case GAME_OVER:
        case GAME_SCREAMING: {

            renderBackground();
            mBury->render(true);

            mPauseA->render(true);
            mPauseB->render(true);
            mScore->render(true);

            mWitch->render(true);
            break;
        }
        case GAME_PAUSE: {

            renderBackground();
            mBury->render(true);

            mScore->render(true);
            mWitch->render(true);

            mPauseA->render(true);
            mPlay->render(true);
            break;
        }
        case GAME_SCORING:
        case GAME_SHARE: {

            renderBackground();
            mBury->render(true);

            mWitch->render(true);

            mPanel->render(true);
            mGameOver->render(true);
            mScoreData->render();
            mShare->render();
            break;
        }
    }
}
