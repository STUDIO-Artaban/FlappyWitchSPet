#include "WitchSPet.h"
#include "Global.h"

#include <libeng/Log/Log.h>
#include <libeng/Storage/Storage.h>
#ifndef DEMO_VERSION
#include <libeng/Advertising/Advertising.h>
#endif
#include <boost/thread.hpp>

#ifndef __ANDROID__
#include "OpenGLES/ES2/gl.h"
#endif

#define MAX_LOAD_STEP               3
#define PROGRESS_TRANS              (1.6f / (MAX_TITLE_LOAD + MAX_GAME_LOAD))

#define WITCH_TRANS                 -1.5f
#define WITCH_VELOCITY              (-WITCH_TRANS / DISP_UPD_COUNT)

static const float g_backColors[MAX_COLOR_COUNT][3] = {

        {1.f, 0.f, 1.f}, // Purple
        {1.f, 0.f, 0.f}, // Red
        {0.f, 1.f, 0.f}, // Green
        {0.f, 0.f, 1.f}  // Blue
};

//////
#ifdef DEBUG
WitchSPet::WitchSPet() : Game2D(1), mLoadStep(0), mColorIdx(0), mStep(STEP_LOAD) {
#else
WitchSPet::WitchSPet() : Game2D(0), mLoadStep(0), mColorIdx(0), mStep(STEP_LOAD) {
#endif
    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (!mGameLevel)
        mGameIntro = new Intro(Intro::LANG_EN, true);

    mGameData = new StoreData;
    mScores = Scores::getInstance(mGameData);
    Dynamic2D::initIncreaseBuffer();

    mFonts->addFont(0, 100, SCORE_FONT_HEIGHT,
            static_cast<short>(FONT_TEXTURE_SIZE), static_cast<short>(FONT_TEXTURE_SIZE));
    mFonts->addFont(1, 46, 59, static_cast<short>(FONT_TEXTURE_SIZE), static_cast<short>(FONT_TEXTURE_SIZE));
    mFonts->addFont(2, 35, 46, static_cast<short>(FONT_TEXTURE_SIZE), static_cast<short>(FONT_TEXTURE_SIZE), 0.f);

    mPumpkin = NULL;
    mProgress = NULL;
    mTitle = NULL;
    mWitchSPet = NULL;
    mWaitNet = NULL;
}
WitchSPet::~WitchSPet() {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Dynamic2D::freeIncreaseBuffer();
    Scores::freeInstance();
}

bool WitchSPet::start() {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (Game2D::start()) {

        if (Storage::loadFile(FILE_NAME)) {

            Storage* store = Storage::getInstance();
            if (!store->isEmpty(FILE_NAME)) {
                if (!Storage::File2Data(mGameData, (*store)[FILE_NAME])) {

                    LOGW(LOG_FORMAT(" - Failed to get data from '%s' file"), __PRETTY_FUNCTION__, __LINE__, FILE_NAME);
                    mScores->create();
                    assert(NULL);
                }
                else
                    mScores->read();
            }
            else // No data stored: Create data
                mScores->create();
        }
#ifdef DEBUG
        else {

            LOGW(LOG_FORMAT(" - Failed to open existing '%s' file"), __PRETTY_FUNCTION__, __LINE__, FILE_NAME);
            assert(NULL);
        }
#endif
        return true;
    }
    else if (mGameLevel) {

        if (!(mGameLevel & 0x01))
            --mGameLevel;

#ifndef DEMO_VERSION
        if (Advertising::getStatus() == Advertising::STATUS_DISPLAYED)
            Advertising::hide(GameLevel::adDisplayID);
#endif
        // Check if still online
#ifdef __ANDROID__
        if (!Internet::getInstance()->isOnline(2000)) {
#else
        if (!Internet::getInstance()->isOnline()) {
#endif
            assert(!mWaitNet);
            mGameLevel = 0;

            mWaitNet = new WaitConn(true);
            mWaitNet->initialize(game2DVia(this));
            mWaitNet->start(mScreen);
        }
    }
    else if (mWaitNet) {

        // Check if still not online
#ifdef __ANDROID__
        if (!Internet::getInstance()->isOnline(2000))
#else
        if (!Internet::getInstance()->isOnline())
#endif
            mWaitNet->resume();

        else {

            delete mWaitNet;
            mWaitNet = NULL;

            ++mGameLevel;
        }
    }
    return false;
}
void WitchSPet::pause() {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Game2D::pause();

    mLoadStep = 0;

    if (mPumpkin) mPumpkin->pause();
    if (mProgress) mProgress->pause();
    if (mTitle) mTitle->pause();
    if (mWitchSPet) mWitchSPet->pause();
    if (mWaitNet) mWaitNet->pause();
}
void WitchSPet::destroy() {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Game2D::destroy();

    if (mPumpkin) delete mPumpkin;
    if (mProgress) delete mProgress;
    if (mTitle) delete mTitle;
    if (mWitchSPet) delete mWitchSPet;
    if (mWaitNet) delete mWaitNet;

#ifdef DEBUG
    // Check all texture objects has been deleted correctly
    if (!mTextures->chkTextures()) {
        LOGW(LOG_FORMAT(" - Bad textures management"), __PRETTY_FUNCTION__, __LINE__);
    }
    else {
        LOGI(LIBENG_LOG_LEVEL, 0, LOG_FORMAT(" - Good textures management"), __PRETTY_FUNCTION__, __LINE__);
    }
#endif
    mTextures->delTextures();
    mTextures->rmvTextures();

    Player* player = Player::getInstance();
    player->pause(ALL_TRACK);
    player->free();
    player->clean();
}

void WitchSPet::wait(float millis) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WITCHSPET, (mLog % 100), LOG_FORMAT(" - Delay: %f milliseconds"), __PRETTY_FUNCTION__, __LINE__, millis);
#endif
    boost::this_thread::sleep(boost::posix_time::microseconds(static_cast<unsigned long>(millis * 500)));
}
void WitchSPet::update() {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WITCHSPET, (mLog % 100), LOG_FORMAT(" - g:%d; l:%d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel, mLoadStep);
#endif
    switch (mGameLevel) {
        case 0: {

            if (!mWaitNet)
                updateIntro(); // Introduction

            else { // Wait Internet

                static unsigned char wait = 0;
                if (wait < (LIBENG_WAITCONN_DELAY << 2)) {

                    ++wait;
                    break;
                }
                mWaitNet->update();
            }
            break;
        }

        ////// Loading
        case 1: {

            switch (mLoadStep) {
                case 0: {

                    Textures::loadTexture(TEXTURE_ID_FONT);
                    Textures::loadTexture(TEXTURE_ID_FLOOR);
                    // Avoid user to wait too much time during the load progression

                    if (!mPumpkin) {

                        mPumpkin = new Static2D;
                        mPumpkin->initialize(game2DVia(this));
                        mPumpkin->start(TEXTURE_ID_PUMPKIN);
                        mPumpkin->setTexCoords(FULL_TEXCOORD_BUFFER);

                        static const float verCoords[8] = {-0.7f, 0.7f, -0.7f, -0.7f, 0.7f, -0.7f, 0.7f, 0.7f};
                        mPumpkin->setVertices(verCoords);
                    }
                    else
                        mPumpkin->resume(TEXTURE_ID_PUMPKIN);

                    if (!mProgress) {

                        mProgress = new Element2D;
                        mProgress->initialize(game2DVia(this));
                        mProgress->start(0x00, 0x00, 0x00);

                        mProgress->setTexCoords(FULL_TEXCOORD_BUFFER);
                        mProgress->copyVertices(*mPumpkin);
                    }
                    else
                        mProgress->resume(0x00, 0x00, 0x00);
                    mProgress->reset();

                    ++mLoadStep;
                    break;
                }
                case 1: {

                    if (!mTitle)
                        mTitle = new TitleLevel;

                    if (!mTitle->load(this))
                        break;

                    ++mLoadStep;
                    break;
                }
                case 2: {

                    if (!mWitchSPet)
                        mWitchSPet = new GameLevel;

                    if (!mWitchSPet->load(this))
                        break;

                    ++mLoadStep;
                    break;
                }
            }
            if (((!mTitle) || (mTitle->getLoadStep() != (MAX_TITLE_LOAD - 1))) &&
                    ((!mWitchSPet) || (mWitchSPet->getLoadStep() != (MAX_GAME_LOAD - 1))))
                mProgress->translate(PROGRESS_TRANS, 0.f);
            if (mLoadStep != MAX_LOAD_STEP)
                break;

            if (mStep == STEP_LOAD) {

                mWitchSPet->setColor(g_backColors, mColorIdx);
                mWitchSPet->mWitch->translate(WITCH_TRANS, mTitle->mWitchY);
                mWitchSPet->reset(mScreen->bottom, mScreen->width);

                mStep = STEP_TITLE;
            }
            delete mPumpkin;
            delete mProgress;

            mPumpkin = NULL;
            mProgress = NULL;

            resetInputs();
            Inputs::InputUse = USE_INPUT_TOUCH;
            Player::getInstance()->resume();

            ++mGameLevel;
            //break;
        }

        ////// Updating
        case 2: {

            static bool position = true;
            switch (mStep) {
                case STEP_TITLE: {

                    if (mWitchSPet->mWitch->getTransform()[Dynamic2D::TRANS_X] < 0.f) {

                        mWitchSPet->mWitch->translate(WITCH_VELOCITY, 0.f);
                        if (position)
                            mWitchSPet->setPosition();
                    }
                    mWitchSPet->mWitch->update(this, NULL);

                    if (!mTitle->update(this)) {
                        if (mTitle->mStartGame) {

                            mStep = STEP_GAME;
#ifndef DEMO_VERSION
                            unsigned char adStatus = Advertising::getStatus();
                            if ((adStatus == Advertising::STATUS_DISPLAYED) || (adStatus == Advertising::STATUS_FAILED))
                                Advertising::hide(ADVERTISING_ID_TITLE);
#endif
                        }
                        else
                            mStep = STEP_SCORES;
                    }
                    break;
                }
                case STEP_SCORES: {

                    if (mWitchSPet->mWitch->getTransform()[Dynamic2D::TRANS_X] < -WITCH_TRANS) {

                        mWitchSPet->mWitch->translate(WITCH_VELOCITY, 0.f);
                        mWitchSPet->mWitch->update(this, NULL);
                    }
                    if (mTitle->update(this))
                        break;

                    mWitchSPet->mWitch->translate((WITCH_TRANS * 2.f), 0.f);
                    mStep = STEP_TITLE;
                    position = false;
                    break;
                }
                case STEP_GAME: {

                    if (mWitchSPet->update(this))
                        break;

                    mTitle->reset();
                    mWitchSPet->reset(mScreen->bottom, mScreen->width);
                    mWitchSPet->mWitch->translate(WITCH_TRANS, mTitle->mWitchY);
                    mStep = STEP_TITLE;
                    position = true;

                    changeColor();
                    mWitchSPet->setColor(g_backColors, mColorIdx);
                    break;
                }
            }
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown game level to update: %d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel);
            assert(NULL);
            break;
        }
#endif
    }
}

void WitchSPet::render() const {
    switch (mGameLevel) {

        case 0: {

            if (!mWaitNet)
                renderIntro(); // Introduction
            else
                mWaitNet->render(); // Wait Internet
            break;
        }
        case 1: { // Loading

            mPumpkin->render(false);

            glDisable(GL_BLEND);
            mProgress->render(true);
            glEnable(GL_BLEND);
            break;
        }
        case 2: { // Updating

            switch (mStep) {
                case STEP_TITLE:
                case STEP_SCORES: {

                    mWitchSPet->renderBackground();
                    if (mWitchSPet->mWitch->getTransform()[Dynamic2D::TRANS_X] < -WITCH_TRANS)
                        mWitchSPet->mWitch->render(true);
                    mTitle->render();
                    break;
                }
                case STEP_GAME: {

                    mWitchSPet->render();
                    break;
                }
            }
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown game level to render: %d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel);
            assert(NULL);
            break;
        }
#endif
    }
}
