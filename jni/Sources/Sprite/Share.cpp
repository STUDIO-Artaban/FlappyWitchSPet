#include "Share.h"
#include "Global.h"

#ifdef __ANDROID__
#include "Game/PanelCoords.h"
#include <boost/math/tr1.hpp>
#else
#include "PanelCoords.h"
#include <math.h>
#endif
#include <libeng/Game/2D/Game2D.h>
#include <libeng/Social/Session.h>
#include <libeng/Tools/Tools.h>

#include <string>
#include <time.h>

#ifndef __ANDROID__
#include "OpenGLES/ES2/gl.h"
#endif

#define SHARE_TEXTURE_SIZE      1024.f
#define SHARE_PANEL_HEIGHT      231 // Below the score panel (in the Score texture)
#define FONT_GREY               (85.f / 255.f)
#define FONT_SCALE_REF          (0.4f / 600.f) // Width equal 600.f on Samsung Galaxy Tab 2
#define EMPTY_BIRTHDAY          L",,, ,," // Will be replace by "___ __" characters (see font texture)

#define WITCHSPET_HOST          "www.studio-artaban.com"
#define WITCHSPET_PHP           "/WitchSPet.php?user="
#define URL_PHP_DATA            "&data="
#define URL_PHP_NAME            "&name="

#define DB_NAME_SIZE            250

#define STUDIO_ARTABAN_URL      "http://www.studio-artaban.com"
#define WITCHSPET_TITLE         "FLAPPY Witch's Pet"
#define WITCHSPET_PICTURE       "/Images/FlappyWitchSPet-"
#define WITCHSPET_PICTURE_EXT   ".png"
#define BEST_SCORE_CAPTION      "My new best score is now: "
#define BEST_SCORE_TWITTER      "My new best score on #FLAPPYwitchSpet is now: #"
#define BEST_SCORE_DESC         " ...do better!"

#define SOCIAL_VEL              (2.f / DISP_UPD_COUNT)
#ifndef __ANDROID__
#define GOOGLE_REQ_TIMEOUT      (7 * 60) // 7 seconds
#endif

// Texture IDs
#define TEXTURE_ID_SHARE        26

static const wchar_t* g_Month[12] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep",
        L"Oct", L"Nov", L"Dec" };

//////
Share::Share() : mStatus(ALL_HIDDEN), mPicture(false), mNetworkID(Network::NONE), mGender(2), mBirthday(2),
    mURL(NULL), mScore(0) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mLog = NULL;
#endif
    std::memset(&mFacebookArea, 0, sizeof(TouchArea));
    std::memset(&mTwitterArea, 0, sizeof(TouchArea));
    std::memset(&mGoogleArea, 0, sizeof(TouchArea));

    std::memset(&mShareArea, 0, sizeof(TouchArea));
    std::memset(&mCloseNetArea, 0, sizeof(TouchArea));
    std::memset(&mCloseLogArea, 0, sizeof(TouchArea));
    std::memset(&mLogShareArea, 0, sizeof(TouchArea));
    std::memset(&mLogoutArea, 0, sizeof(TouchArea));

    mSocial = Social::getInstance(true);
    mInternet = Internet::getInstance();

    mDispPic[Network::FACEBOOK] = false;
    mDispPic[Network::TWITTER] = false;
    mDispPic[Network::GOOGLE] = false;
}
Share::~Share() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Social::freeInstance();
    Internet::freeInstance();

    if (mURL)
        delete mURL;
}

void Share::start(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    mLog = game->getLog();
#endif
    mPanel.initialize(game2DVia(game));
    mNetworks.initialize(game2DVia(game));
    mShareA.initialize(game2DVia(game));
    mShareB.initialize(game2DVia(game));
    mPicture.initialize(game2DVia(game));
    mNetworkPic.initialize(game2DVia(game));

    mGender.initialize(game2DVia(game));
    mBirthday.initialize(game2DVia(game));

    mPanel.start(TEXTURE_ID_SHARE);

    float texCoords[8] = { SHARE_X0 / SHARE_TEXTURE_SIZE, 1.f, 1.f, 1.f, 1.f, SHARE_Y2 / SHARE_TEXTURE_SIZE,
            SHARE_X0 / SHARE_TEXTURE_SIZE, SHARE_Y2 / SHARE_TEXTURE_SIZE };
    mPanel.setTexCoords(texCoords);

    short height = (SHARE_PANEL_HEIGHT * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE);
    short width = (height * SHARE_WIDTH) / SHARE_HEIGHT;

    short scoreBottom = ((game->getScreen()->height >> 1) + (game->getScreen()->width >> 2)) - // Score panel bottom
            (((static_cast<short>(PANEL_TEXTURE_SIZE) - // position when it is displayed on the screen (Y0 = 0.5f)
                    PANEL_Y0) * game->getScreen()->width) / static_cast<short>(PANEL_TEXTURE_SIZE));
    mPanel.setVertices(0, scoreBottom + height, width, scoreBottom);
    mPanel.translate(mPanel.getLeft() - mPanel.getRight(), 0.f);

    mShareArea.left = SHARE_AREA_LEFT * width / SHARE_WIDTH;
    mShareArea.top = game->getScreen()->height - ((scoreBottom + height) - (SHARE_AREA_TOP * height / SHARE_HEIGHT));
    mShareArea.right = SHARE_AREA_RIGHT * width / SHARE_WIDTH;
    mShareArea.bottom = game->getScreen()->height - ((scoreBottom + height) - (SHARE_AREA_BOTTOM *
            height / SHARE_HEIGHT));

    //
    mNetworks.start(TEXTURE_ID_SHARE);

    texCoords[0] = 0.f;
    texCoords[1] = NETWORKS_Y0 / SHARE_TEXTURE_SIZE;
    texCoords[2] = 0.f;
    texCoords[3] = 1.f;
    texCoords[4] = NETWORKS_X2 / SHARE_TEXTURE_SIZE;
    texCoords[5] = 1.f;
    texCoords[6] = texCoords[4];
    texCoords[7] = texCoords[1];
    mNetworks.setTexCoords(texCoords);

    height = ((static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0) * game->getScreen()->width) / NETWORKS_X2;
    mNetworks.setVertices(0, (game->getScreen()->height >> 1) + (height >> 1), game->getScreen()->width,
            (game->getScreen()->height >> 1) - (height >> 1));
    mNetworks.translate(2.f, 0.f);

    mCloseNetArea.left = CLOSENET_AREA_LEFT * game->getScreen()->width / NETWORKS_X2;
    mCloseNetArea.top = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((CLOSENET_AREA_TOP - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));
    mCloseNetArea.right = CLOSENET_AREA_RIGHT * game->getScreen()->width / NETWORKS_X2;
    mCloseNetArea.bottom = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((CLOSENET_AREA_BOTTOM - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));

    mFacebookArea.left = FACEBOOK_PIC_X0 * game->getScreen()->width / NETWORKS_X2;
    mFacebookArea.top = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((FACEBOOK_PIC_Y0 - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));
    mFacebookArea.right = FACEBOOK_PIC_X2 * game->getScreen()->width / NETWORKS_X2;
    mFacebookArea.bottom = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((FACEBOOK_PIC_Y2 - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));

    mTwitterArea.left = TWITTER_PIC_X0 * game->getScreen()->width / NETWORKS_X2;
    mTwitterArea.top = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((TWITTER_PIC_Y0 - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));
    mTwitterArea.right = TWITTER_PIC_X2 * game->getScreen()->width / NETWORKS_X2;
    mTwitterArea.bottom = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((TWITTER_PIC_Y2 - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));

    mGoogleArea.left = GOOGLE_PIC_X0 * game->getScreen()->width / NETWORKS_X2;
    mGoogleArea.top = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((GOOGLE_PIC_Y0 - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));
    mGoogleArea.right = GOOGLE_PIC_X2 * game->getScreen()->width / NETWORKS_X2;
    mGoogleArea.bottom = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((GOOGLE_PIC_Y2 - NETWORKS_Y0) * height / (static_cast<short>(PANEL_TEXTURE_SIZE) - NETWORKS_Y0)));

    //
    mShareA.start(TEXTURE_ID_SHARE);

    texCoords[0] = 0.f;
    texCoords[1] = SHARE_A_Y0 / SHARE_TEXTURE_SIZE;
    texCoords[2] = 0.f;
    texCoords[3] = SHARE_A_Y2 / SHARE_TEXTURE_SIZE;
    texCoords[4] = 1.f;
    texCoords[5] = texCoords[3];
    texCoords[6] = 1.f;
    texCoords[7] = texCoords[1];
    mShareA.setTexCoords(texCoords);

    height = ((SHARE_A_Y2 - SHARE_A_Y0) * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE);
    mShareA.setVertices(0, (game->getScreen()->height >> 1) + (height >> 1), game->getScreen()->width,
            (game->getScreen()->height >> 1) - (height >> 1));
    mShareA.translate(-2.f, 0.f);

    //
    mShareB.start(TEXTURE_ID_SHARE);

    texCoords[0] = SHARE_B_X0 / SHARE_TEXTURE_SIZE;
    texCoords[1] = SHARE_B_Y0 / SHARE_TEXTURE_SIZE;
    texCoords[2] = texCoords[0];
    texCoords[3] = SHARE_B_Y2 / SHARE_TEXTURE_SIZE;
    texCoords[4] = 1.f;
    texCoords[5] = texCoords[3];
    texCoords[6] = 1.f;
    texCoords[7] = texCoords[1];
    mShareB.setTexCoords(texCoords);

    short heightB = ((SHARE_B_Y2 - SHARE_B_Y0) * height) / (SHARE_A_Y2 - SHARE_A_Y0);
    width = ((static_cast<short>(SHARE_TEXTURE_SIZE) - SHARE_B_X0) *
            game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE);
    mShareB.setVertices(game->getScreen()->width - width, (game->getScreen()->height >> 1) - (height >> 1),
            game->getScreen()->width, ((game->getScreen()->height >> 1) - (height >> 1)) - heightB);
    mShareB.translate(-2.f, 0.f);

    mCloseLogArea.left = CLOSELOG_AREA_LEFT * game->getScreen()->width / static_cast<short>(SHARE_TEXTURE_SIZE);
    mCloseLogArea.top = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((CLOSELOG_AREA_TOP - SHARE_A_Y0) * height / (SHARE_A_Y2 - SHARE_A_Y0)));
    mCloseLogArea.right = CLOSELOG_AREA_RIGHT * game->getScreen()->width / static_cast<short>(SHARE_TEXTURE_SIZE);
    mCloseLogArea.bottom = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((CLOSELOG_AREA_BOTTOM - SHARE_A_Y0) * height / (SHARE_A_Y2 - SHARE_A_Y0)));

    mLogShareArea.left = LOGSHARE_AREA_LEFT * game->getScreen()->width / static_cast<short>(SHARE_TEXTURE_SIZE);
    mLogShareArea.top = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((LOGSHARE_AREA_TOP - SHARE_A_Y0) * height / (SHARE_A_Y2 - SHARE_A_Y0)));
    mLogShareArea.right = LOGSHARE_AREA_RIGHT * game->getScreen()->width / static_cast<short>(SHARE_TEXTURE_SIZE);
    mLogShareArea.bottom = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((LOGSHARE_AREA_BOTTOM - SHARE_A_Y0) * height / (SHARE_A_Y2 - SHARE_A_Y0)));

    mLogoutArea.left = LOGOUT_AREA_LEFT * game->getScreen()->width / static_cast<short>(SHARE_TEXTURE_SIZE);
    mLogoutArea.top = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((LOGOUT_AREA_TOP - SHARE_A_Y0) * height / (SHARE_A_Y2 - SHARE_A_Y0)));
    mLogoutArea.right = LOGOUT_AREA_RIGHT * game->getScreen()->width / static_cast<short>(SHARE_TEXTURE_SIZE);
    mLogoutArea.bottom = game->getScreen()->height - (((game->getScreen()->height >> 1) + (height >> 1)) -
            ((LOGOUT_AREA_BOTTOM - SHARE_A_Y0) * height / (SHARE_A_Y2 - SHARE_A_Y0)));

    // Initialize 'mPicture' using Facebook network as default
    mPicture.start(Textures::getInstance()->getIndex(Facebook::TEXTURE_ID));
    mPicture.setTexCoords(FULL_TEXCOORD_BUFFER);

    width = (PICTURE_LEFT * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE); // X0
    heightB = ((PICTURE_TOP - SHARE_A_Y0) * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE); // Y0
    scoreBottom = (PICTURE_SIZE * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE); // Size
    mPicture.setVertices(width, ((game->getScreen()->height >> 1) + (height >> 1)) - heightB, width + scoreBottom,
            (((game->getScreen()->height >> 1) + (height >> 1)) - heightB) - scoreBottom);
    mPicture.translate(-2.f, 0.f);

    mNetworkPic.start(TEXTURE_ID_SHARE);
    mNetworkID = Network::FACEBOOK; // Using Facebook network as default
    setNetwork();
    mNetworkID = Network::NONE;

    width = (NETWORK_PIC_LEFT * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE); // X0
    heightB = ((NETWORK_PIC_TOP - SHARE_A_Y0) * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE); // Y0
    scoreBottom = (NETWORK_PIC_SIZE * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE); // Size
    mNetworkPic.setVertices(width, ((game->getScreen()->height >> 1) + (height >> 1)) - heightB, width + scoreBottom,
            (((game->getScreen()->height >> 1) + (height >> 1)) - heightB) - scoreBottom);
    mNetworkPic.translate(-2.f, 0.f);

    mGender.start(L","); // Will be replace by '_' character (see font texture)
    mGender.setColor(FONT_GREY, FONT_GREY, FONT_GREY);
    float fontScale = FONT_SCALE_REF * game->getScreen()->width;
    mGender.scale(fontScale, fontScale);
    mGender.setAlpha(0.f);

    heightB = ((GENDER_TOP - SHARE_A_Y0) * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE);
    mGender.position(((GENDER_LEFT << 1) / SHARE_TEXTURE_SIZE) - 1.f,
                        (((height << 1) / static_cast<float>(game->getScreen()->width)) / 2.f) -
                        ((heightB << 1) / static_cast<float>(game->getScreen()->width)));

    mBirthday.start(EMPTY_BIRTHDAY);
    mBirthday.setColor(FONT_GREY, FONT_GREY, FONT_GREY);
    mBirthday.scale(fontScale, fontScale);
    mBirthday.setAlpha(0.f);

    heightB = ((BIRTHDAY_TOP - SHARE_A_Y0) * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE);
    mBirthday.position(((BIRTHDAY_LEFT << 1) / SHARE_TEXTURE_SIZE) - 1.f,
                        (((height << 1) / static_cast<float>(game->getScreen()->width)) / 2.f) -
                        ((heightB << 1) / static_cast<float>(game->getScreen()->width)));
}
void Share::resume() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mPanel.resume(TEXTURE_ID_SHARE);
    mNetworks.resume(TEXTURE_ID_SHARE);
    mShareA.resume(TEXTURE_ID_SHARE);
    mShareB.resume(TEXTURE_ID_SHARE);

    switch (mNetworkID) {

        case Network::FACEBOOK: mPicture.resume(Textures::getInstance()->getIndex(Facebook::TEXTURE_ID)); break;
        case Network::TWITTER: mPicture.resume(Textures::getInstance()->getIndex(Twitter::TEXTURE_ID)); break;
        case Network::GOOGLE: mPicture.resume(Textures::getInstance()->getIndex(Google::TEXTURE_ID)); break;
    }
    if ((mSocial->_getSession(mNetworkID)) && (mSocial->_getSession(mNetworkID)->getUserPic()) && (mDispPic[mNetworkID]))
        setPicture();

    mNetworkPic.resume(TEXTURE_ID_SHARE);
    mGender.resume();
    mBirthday.resume();
    if (mURL)
        mURL->resume();
}

void Share::setNetwork() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    assert(mNetworkID != Network::NONE);

#ifdef DEBUG
    mPicture.pause(); // Reset texture index to avoid assert when 'resume' method is called
#endif
    switch (mNetworkID) {

        case Network::FACEBOOK: {
            mPicture.resume(Textures::getInstance()->getIndex(Facebook::TEXTURE_ID));
            static const float texCoords[8] = { FACEBOOK_PIC_X0 / SHARE_TEXTURE_SIZE,
                    FACEBOOK_PIC_Y0 / SHARE_TEXTURE_SIZE, FACEBOOK_PIC_X0 / SHARE_TEXTURE_SIZE,
                    FACEBOOK_PIC_Y2 / SHARE_TEXTURE_SIZE, FACEBOOK_PIC_X2 / SHARE_TEXTURE_SIZE,
                    FACEBOOK_PIC_Y2 / SHARE_TEXTURE_SIZE, FACEBOOK_PIC_X2 / SHARE_TEXTURE_SIZE,
                    FACEBOOK_PIC_Y0 / SHARE_TEXTURE_SIZE };
            mNetworkPic.setTexCoords(texCoords);
            break;
        }
        case Network::TWITTER: {
            mPicture.resume(Textures::getInstance()->getIndex(Twitter::TEXTURE_ID));
            static const float texCoords[8] = { TWITTER_PIC_X0 / SHARE_TEXTURE_SIZE,
                    TWITTER_PIC_Y0 / SHARE_TEXTURE_SIZE, TWITTER_PIC_X0 / SHARE_TEXTURE_SIZE,
                    TWITTER_PIC_Y2 / SHARE_TEXTURE_SIZE, TWITTER_PIC_X2 / SHARE_TEXTURE_SIZE,
                    TWITTER_PIC_Y2 / SHARE_TEXTURE_SIZE, TWITTER_PIC_X2 / SHARE_TEXTURE_SIZE,
                    TWITTER_PIC_Y0 / SHARE_TEXTURE_SIZE };
            mNetworkPic.setTexCoords(texCoords);
            break;
        }
        case Network::GOOGLE: {
            mPicture.resume(Textures::getInstance()->getIndex(Google::TEXTURE_ID));
            static const float texCoords[8] = { GOOGLE_PIC_X0 / SHARE_TEXTURE_SIZE,
                    GOOGLE_PIC_Y0 / SHARE_TEXTURE_SIZE, GOOGLE_PIC_X0 / SHARE_TEXTURE_SIZE,
                    GOOGLE_PIC_Y2 / SHARE_TEXTURE_SIZE, GOOGLE_PIC_X2 / SHARE_TEXTURE_SIZE,
                    GOOGLE_PIC_Y2 / SHARE_TEXTURE_SIZE, GOOGLE_PIC_X2 / SHARE_TEXTURE_SIZE,
                    GOOGLE_PIC_Y0 / SHARE_TEXTURE_SIZE };
            mNetworkPic.setTexCoords(texCoords);
            break;
        }
    }
}
void Share::setPicture() {

    assert(mNetworkID != Network::NONE);

    Textures* textures = Textures::getInstance();
    unsigned char textureIdx = textures->addTexPic(mNetworkID);
    if (textureIdx != TEXTURE_IDX_INVALID) {

        float texCoords[8] = {0};
        texCoords[3] = static_cast<float>(mSocial->_getSession(mNetworkID)->getPicHeight()) /
                (*textures)[textureIdx]->height;
        texCoords[4] = static_cast<float>(mSocial->_getSession(mNetworkID)->getPicWidth()) /
                (*textures)[textureIdx]->width;
        texCoords[5] = texCoords[3];
        texCoords[6] = texCoords[4];
        mPicture.setTexCoords(texCoords);

        mDispPic[mNetworkID] = true;
    }
    //else // Picture buffer has changed (== NULL)
}
bool Share::select() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (!mSocial->_getSession(mNetworkID))
        mSocial->addSession(mNetworkID, false);

    assert(mSocial->_getSession(mNetworkID));
    if (!mSocial->_getSession(mNetworkID)->isOpened())
        return mSocial->request(mNetworkID, Session::REQUEST_LOGIN, NULL);

    else {

        bool idEmpty = false;
        switch (mNetworkID) {

            case Network::FACEBOOK: idEmpty = mFacebookURL.empty(); break;
            case Network::TWITTER: idEmpty = mTwitterURL.empty(); break;
            case Network::GOOGLE: idEmpty = mGoogleURL.empty(); break;
        }
        if (idEmpty)
            return mSocial->request(mNetworkID, Session::REQUEST_INFO, NULL);

        mStatus = NETWORKS_HIDDING; // Display share dialog
    }
    return false;
}
void Share::displayInfo(const Game* game) {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    assert(mNetworkID != Network::NONE);
    assert(mSocial->_getSession(mNetworkID));
    assert(!mURL);

    // Gender
    switch (mSocial->_getSession(mNetworkID)->getUserGender()) {
        case GENDER_MALE: if (mGender.getText() != L"M") mGender.update(L"M"); break;
        case GENDER_FEMALE: if (mGender.getText() != L"F") mGender.update(L"F"); break;
    }
    mGender.setAlpha(1.f);

    // Birthday
    std::string userBirthday(mSocial->_getSession(mNetworkID)->getUserBirthday());
    if (userBirthday.length() > 9) { // MM/dd/yyyy

        std::wstring birthday(g_Month[strToNum<short>(userBirthday.substr(0, 2)) - 1]);
        birthday += L' ';
        birthday.append(numToWStr<unsigned char>(strToNum<short>(userBirthday.substr(3, 2))));

        if (mBirthday.getText() != birthday)
            mBirthday.update(birthday);
    }
    else if (mBirthday.getText() != EMPTY_BIRTHDAY)
        mBirthday.update(EMPTY_BIRTHDAY);
    mBirthday.setAlpha(1.f);

    // URL
    std::wstring url;
    switch (mNetworkID) {

        case Network::FACEBOOK: url.assign(mFacebookURL.begin(), mFacebookURL.end()); break;
        case Network::TWITTER: url.assign(mTwitterURL.begin(), mTwitterURL.end()); break;
        case Network::GOOGLE: url.assign(mGoogleURL.begin(), mGoogleURL.end()); break;
    }
    std::replace(url.begin(), url.end(), L'*', L'à');
    std::replace(url.begin(), url.end(), L'#', L'â');
    std::replace(url.begin(), url.end(), L'^', L'€');
    std::replace(url.begin(), url.end(), L'}', L'>');
    std::replace(url.begin(), url.end(), L']', L'}');
    std::replace(url.begin(), url.end(), L'{', L'<');
    std::replace(url.begin(), url.end(), L'[', L'{');
    std::replace(url.begin(), url.end(), L'~', L'\'');
    std::replace(url.begin(), url.end(), L'§', L'"');
    std::replace(url.begin(), url.end(), L'_', L',');

#ifdef DEBUG
    if (url.length() > 255) {
        LOGE(LOG_FORMAT(" - External profile URL too long"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
    }
#endif
    for (unsigned char i = (static_cast<unsigned char>(url.length()) / 27); i; --i)
        url.insert(27 * i, 1, '\n');

    mURL = new Text2D(2);
    mURL->initialize(game2DVia(game));
    mURL->start(url);
    mURL->setColor(FONT_GREY, FONT_GREY, FONT_GREY);
    float scale = FONT_SCALE_REF * game->getScreen()->width;
    mURL->scale(scale, scale);

    short panelH = ((SHARE_A_Y2 - SHARE_A_Y0) * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE);
    short urlH = ((URL_TOP - SHARE_A_Y0) * game->getScreen()->width) / static_cast<short>(SHARE_TEXTURE_SIZE);
    mURL->position(((URL_LEFT << 1) / SHARE_TEXTURE_SIZE) - 1.f,
            (((panelH << 1) / static_cast<float>(game->getScreen()->width)) / 2.f) -
            ((urlH << 1) / static_cast<float>(game->getScreen()->width)));
}

std::string Share::getUserURL() const {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    assert(mSocial->_getSession(mNetworkID));

    std::string user(WITCHSPET_PHP);
    user.append(encodeURL(encodeB64(numToHex<int>(static_cast<int>(mNetworkID)) +
            mSocial->_getSession(mNetworkID)->getUserID())));
    return user;
}
std::string Share::getRequestURL() const {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    assert(mSocial->_getSession(mNetworkID));
    assert(mScore > 0);

    int score = static_cast<int>(mScore) * 10;
    switch (mSocial->_getSession(mNetworkID)->getUserGender()) {

        case GENDER_NONE:
        case GENDER_MALE: score += 1; //break;
        case GENDER_FEMALE: break;
        //case GENDER_NONE: {
        //
        //    LOGW(LOG_FORMAT(" - Failed to get user gender"), __PRETTY_FUNCTION__, __LINE__);
        //    assert(NULL);
        //    break;
        //}
    }
    score = 32766 - score;

    time_t now = time(NULL);
    struct tm* date = gmtime(&now);
    if (((date->tm_min + date->tm_hour) % ((date->tm_sec)? date->tm_sec:1)) & 0x01)
        score *= (date->tm_sec + date->tm_hour)? (date->tm_sec + date->tm_hour):1;
    else
        score *= (date->tm_min + date->tm_hour)? (date->tm_min + date->tm_hour):1;

    std::string url(getUserURL()); // user

    std::string data(numToHex<int>(std::rand()));
    data += '_';
    data.append(numToHex<int>(score));
    data += '_';
    std::string userID(mSocial->_getSession(mNetworkID)->getUserID());
    int userCheckSum = 0;
    for (unsigned char i = 0; i < static_cast<unsigned char>(userID.length()); ++i)
        userCheckSum += static_cast<unsigned char>(userID.at(i));
    userCheckSum *= 32766;
    if ((date->tm_min + date->tm_hour) > date->tm_sec)
        userCheckSum += 32;
    else
        userCheckSum -= 23;
    userCheckSum /= (date->tm_sec)? date->tm_sec:1;
    data.append(numToHex<int>(userCheckSum));
    data += '_';
    unsigned char randChar = static_cast<unsigned char>(std::rand() % 16); // 16 == 0xF
    data.append(numToHex<int>((randChar)? randChar:1));
    if (date->tm_sec < 16) data += '0';
    data.append(numToHex<int>(date->tm_sec));
    data.append(numToHex<int>(static_cast<unsigned char>(std::rand() % 16)));
    if (date->tm_hour < 16) data += '0';
    data.append(numToHex<int>(date->tm_hour));
    data.append(numToHex<int>(static_cast<unsigned char>(std::rand() % 16)));
    if (date->tm_min < 16) data += '0';
    data.append(numToHex<int>(date->tm_min));
    data.append(numToHex<int>(static_cast<unsigned char>(std::rand() % 16)));
    url.append(URL_PHP_DATA);
    url.append(encodeURL(encodeB64(data))); // data

    url.append(URL_PHP_NAME); // name
    std::wstring userName(mSocial->_getSession(mNetworkID)->getUserName());
    std::string name;
    for (std::wstring::const_iterator iter = userName.begin(); iter != userName.end(); ++iter) {
        name += '%';
        name.append(numToHex<unsigned int>(static_cast<unsigned int>(*iter)));
        if (name.length() > (DB_NAME_SIZE - 6)) {
            name += '.'; // ...
            break;
        }
    }
    if (name.empty())
        name += '.'; // ...
    url.append(encodeURL(encodeB64(name)));

    return url;
}

bool Share::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, (*mLog % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
#endif
    static unsigned char count = 0;
    static bool wait = false;
#ifndef __ANDROID__
    static short signInCount = 0;
#endif

    switch (mStatus) {
        case PANEL_DISPLAYING: {

            static float panelVel = (mPanel.getRight() - mPanel.getLeft()) / DISP_UPD_COUNT;
            if (++count != (DISP_UPD_COUNT + 1))
                mPanel.translate(panelVel, 0.f);
            else {

                count = 0;
                mStatus = PANEL_DISPLAYED;
            }
            break;
        }
        case PANEL_DISPLAYED: {

            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue;

                // Check share
                if ((game->mTouchData[touchCount].X > mShareArea.left) &&
                        (game->mTouchData[touchCount].X < mShareArea.right) &&
                        (game->mTouchData[touchCount].Y > mShareArea.top) &&
                        (game->mTouchData[touchCount].Y < mShareArea.bottom)) {

                    count = 0;
                    mStatus = PANEL_HIDDING;
                    return true;
                }
            }
            break;
        }
        case PANEL_HIDDING: {

            static float panelVel = (mPanel.getLeft()- mPanel.getRight()) / DISP_UPD_COUNT;
            if (++count != (DISP_UPD_COUNT + 1))
                mPanel.translate(panelVel, 0.f);
            else {

                count = 0;
                mStatus = NETWORKS_DISPLAYING;
            }
            break;
        }
        case NETWORKS_DISPLAYING: {

            if (++count != (DISP_UPD_COUNT + 1))
                mNetworks.translate(-SOCIAL_VEL, 0.f);
            else
                mStatus = NETWORKS_DISPLAYED;
            break;
        }
        case NETWORKS_DISPLAYED: {

            if (wait) {

                switch (mSocial->_getSession(mNetworkID)->getResult()) {
                    case Request::RESULT_SUCCEEDED: {

                        LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - RESULT_SUCCEEDED"), __PRETTY_FUNCTION__, __LINE__);
                        switch (mSocial->_getSession(mNetworkID)->getRequest()) {

                            case Session::REQUEST_LOGIN: {
#ifndef __ANDROID__
                                signInCount = 0;
#endif
                                wait = mSocial->request(mNetworkID, Session::REQUEST_INFO, NULL);
                                if (!wait) mSocial->_getSession(mNetworkID)->close();
                                break;
                            }
                            case Session::REQUEST_INFO: {
                                switch (mNetworkID) {
                                    case Network::FACEBOOK: {

                                        mFacebookURL = LIBENG_FACEBOOK_URL;
                                        mFacebookURL.append(mSocial->_getSession(Network::FACEBOOK)->getUserID());
                                        break;
                                    }
                                    case Network::TWITTER: {

                                        mTwitterURL = LIBENG_TWITTER_URL;
                                        mTwitterURL.append(mSocial->_getSession(Network::TWITTER)->getUserID());
                                        break;
                                    }
                                    case Network::GOOGLE: {

                                        mGoogleURL = LIBENG_GOOGLE_URL;
                                        mGoogleURL.append(mSocial->_getSession(Network::GOOGLE)->getUserID());
                                        break;
                                    }
                                }
                                mStatus = NETWORKS_HIDDING; // Display share dialog
                                //break;
                            }
                            case Session::REQUEST_SHARE_LINK: {
                                wait = false;
                                break;
                            }
                        }
                        break;
                    }
                    case Request::RESULT_EXPIRED: {

                        LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - RESULT_EXPIRED"), __PRETTY_FUNCTION__, __LINE__);
                        mSocial->_getSession(mNetworkID)->close();
                        mDispPic[mNetworkID] = false;
                        switch (mNetworkID) {

                            case Network::FACEBOOK: mFacebookURL.clear(); break;
                            case Network::TWITTER: mTwitterURL.clear(); break;
                            case Network::GOOGLE: mGoogleURL.clear(); break;
                        }
                        wait = mSocial->request(mNetworkID, Session::REQUEST_LOGIN, NULL);
                        break;
                    }
                    case Request::RESULT_WAITING: { // Wait

                        // Time out control for Goolge+ login request on iOS only coz there is no way to catch
                        // a cancel event when no Google+ application is installed (login managed in Safari)
                        // and user goes back to this application without logged
#ifndef __ANDROID__
                        if ((mNetworkID == Network::GOOGLE) &&
                            ((mSocial->_getSession(mNetworkID)->getRequest() == Session::REQUEST_LOGIN) ||
                             (mSocial->_getSession(mNetworkID)->getRequest() == Session::REQUEST_SHARE_LINK)) &&
                            (++signInCount > GOOGLE_REQ_TIMEOUT)) {

                            signInCount = 0;
                            count = 0;
                            mNetworkID = Network::NONE;
                            mStatus = NETWORKS_HIDDING;
                            wait = false;
                            return false;
                        }
#endif
                        break;
                    }
                    default: {
                    //case Request::RESULT_NONE: // Request sent error (!wait)
                    //case Request::RESULT_CANCELED:
                    //case Request::RESULT_FAILED: {

                        LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - RESULT_CANCELED/FAILED"), __PRETTY_FUNCTION__, __LINE__);
                        if ((mSocial->_getSession(mNetworkID)->getRequest() == Session::REQUEST_INFO) &&
                            (mSocial->_getSession(mNetworkID)->getResult() == Request::RESULT_FAILED))
                            mSocial->_getSession(mNetworkID)->close();
                        mNetworkID = Network::NONE;
                        wait = false;
#ifndef __ANDROID__
                        signInCount = 0;
#endif
                        break;
                    }
                }
                break; // Stop touch management
            }

            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue;

                // Check close
                if ((game->mTouchData[touchCount].X > mCloseNetArea.left) &&
                        (game->mTouchData[touchCount].X < mCloseNetArea.right) &&
                        (game->mTouchData[touchCount].Y > mCloseNetArea.top) &&
                        (game->mTouchData[touchCount].Y < mCloseNetArea.bottom)) {

                    count = 0;
                    mNetworkID = Network::NONE;
                    mStatus = NETWORKS_HIDDING;
                    return false;
                }

                // Check Facebook
                if ((game->mTouchData[touchCount].X > mFacebookArea.left) &&
                        (game->mTouchData[touchCount].X < mFacebookArea.right) &&
                        (game->mTouchData[touchCount].Y > mFacebookArea.top) &&
                        (game->mTouchData[touchCount].Y < mFacebookArea.bottom)) {

                    count = 0;
                    mNetworkID = Network::FACEBOOK;
                    wait = select();
                    break;
                }

                // Check Twitter
                if ((game->mTouchData[touchCount].X > mTwitterArea.left) &&
                        (game->mTouchData[touchCount].X < mTwitterArea.right) &&
                        (game->mTouchData[touchCount].Y > mTwitterArea.top) &&
                        (game->mTouchData[touchCount].Y < mTwitterArea.bottom)) {

                    count = 0;
                    mNetworkID = Network::TWITTER;
                    wait = select();
                    break;
                }

                // Check Google+
                if ((game->mTouchData[touchCount].X > mGoogleArea.left) &&
                        (game->mTouchData[touchCount].X < mGoogleArea.right) &&
                        (game->mTouchData[touchCount].Y > mGoogleArea.top) &&
                        (game->mTouchData[touchCount].Y < mGoogleArea.bottom)) {

                    count = 0;
                    mNetworkID = Network::GOOGLE;
                    wait = select();
                    break;
                }
            }
            break;
        }
        case NETWORKS_HIDDING: {

            if (++count != (DISP_UPD_COUNT + 1))
                mNetworks.translate(SOCIAL_VEL, 0.f);
            else {

                count = 0;
                if (mNetworkID == Network::NONE) {

                    mStatus = ALL_HIDDEN;
                    return true;
                }
                mStatus = SHARE_DISPLAYING;
                setNetwork();
#ifdef DEBUG
                LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
                assert(mSocial->_getSession(mNetworkID));
                switch (mNetworkID) {

                    case Network::FACEBOOK: assert(!mFacebookURL.empty()); break;
                    case Network::TWITTER: assert(!mTwitterURL.empty()); break;
                    case Network::GOOGLE: assert(!mGoogleURL.empty()); break;
                }
#endif
                mSocial->setAdReqInfo(mNetworkID);
            }
            break;
        }
        case SHARE_DISPLAYING: {

            if (++count != (DISP_UPD_COUNT + 1)) {

                mShareA.translate(SOCIAL_VEL, 0.f);
                mShareB.translate(SOCIAL_VEL, 0.f);
                mPicture.translate(SOCIAL_VEL, 0.f);
                mNetworkPic.translate(SOCIAL_VEL, 0.f);
            }
            else {

                count = 0;
                mStatus = SHARE_DISPLAYED;
                if ((!mSocial->_getSession(mNetworkID)->getUserPic()) &&
                        ((mSocial->_getSession(mNetworkID)->getRequest() != Session::REQUEST_PICTURE) ||
                        (mSocial->_getSession(mNetworkID)->getResult() != Request::RESULT_WAITING)))
                    mSocial->request(mNetworkID, Session::REQUEST_PICTURE, NULL);
                displayInfo(game);
            }
            break;
        }
        case SHARE_DISPLAYED: {

            if (mSocial->_getSession(mNetworkID)->getUserPic()) //&& (!mDispPic[mNetworkID])) // ...for resume
                setPicture();

            static bool httpSent = false;
            if (httpSent) {
                if (!mInternet->waitHTTP()) {
                    if (mInternet->getHTTP().find("FWSP-:)") != std::string::npos) {

                        ShareData* link = NULL;
                        switch (mNetworkID) {
                            case Network::FACEBOOK: {

                                link = new Facebook::LinkData;
                                static_cast<Facebook::LinkData*>(link)->name = WITCHSPET_TITLE;
                                static_cast<Facebook::LinkData*>(link)->caption = WITCHSPET_TITLE;
                                static_cast<Facebook::LinkData*>(link)->description = BEST_SCORE_CAPTION;
                                static_cast<Facebook::LinkData*>(link)->description.append(numToStr<short>(mScore));
                                static_cast<Facebook::LinkData*>(link)->description.append(BEST_SCORE_DESC);
                                static_cast<Facebook::LinkData*>(link)->link = STUDIO_ARTABAN_URL;
                                static_cast<Facebook::LinkData*>(link)->link.append(getUserURL());
                                static_cast<Facebook::LinkData*>(link)->picture = STUDIO_ARTABAN_URL;
                                static_cast<Facebook::LinkData*>(link)->picture.append(WITCHSPET_PICTURE);
                                time_t now = time(NULL);
                                struct tm* date = gmtime(&now);
                                switch (date->tm_wday) {
                                    case 0: static_cast<Facebook::LinkData*>(link)->picture += 'P'; break; // Sunday
                                    case 1:
                                    case 4: static_cast<Facebook::LinkData*>(link)->picture += 'R'; break;
                                    case 2:
                                    case 5: static_cast<Facebook::LinkData*>(link)->picture += 'G'; break;
                                    default: static_cast<Facebook::LinkData*>(link)->picture += 'B'; break;
                                }
                                static_cast<Facebook::LinkData*>(link)->picture.append(WITCHSPET_PICTURE_EXT);
                                break;
                            }
                            case Network::GOOGLE: {

                                link = new Google::LinkData;
                                static_cast<Google::LinkData*>(link)->url = STUDIO_ARTABAN_URL;
                                static_cast<Google::LinkData*>(link)->url.append(getUserURL());
                                break;
                            }
                            case Network::TWITTER: {

                                link = new Twitter::LinkData;
                                static_cast<Twitter::LinkData*>(link)->tweet = BEST_SCORE_TWITTER;
                                static_cast<Twitter::LinkData*>(link)->tweet.append(numToStr<short>(mScore));
                                static_cast<Twitter::LinkData*>(link)->tweet.append(BEST_SCORE_DESC);
                                static_cast<Twitter::LinkData*>(link)->tweet += ' ';
                                static_cast<Twitter::LinkData*>(link)->tweet.append(STUDIO_ARTABAN_URL);
                                static_cast<Twitter::LinkData*>(link)->tweet.append(getUserURL());
                                break;
                            }
                        }
                        wait = mSocial->request(mNetworkID, Session::REQUEST_SHARE_LINK, link);
                        delete link;
                        count = 0;
                        mStatus = SHARE_HIDDING;
                    }
                    else {

                        LOGW(LOG_FORMAT(" - Failed to update score"), __PRETTY_FUNCTION__, __LINE__);
                        mSocial->request(mNetworkID, Session::REQUEST_SHARE_LINK, NULL); // Display alert message
                    }
                    httpSent = false;
                }
                break; // Stop touch management
            }

            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue;

                // Check close
                if ((game->mTouchData[touchCount].X > mCloseLogArea.left) &&
                        (game->mTouchData[touchCount].X < mCloseLogArea.right) &&
                        (game->mTouchData[touchCount].Y > mCloseLogArea.top) &&
                        (game->mTouchData[touchCount].Y < mCloseLogArea.bottom)) {

                    count = 0;
                    mNetworkID = Network::NONE;
                    mStatus = SHARE_HIDDING;
                    return false;
                }

                // Check logout
                if ((game->mTouchData[touchCount].X > mLogoutArea.left) &&
                        (game->mTouchData[touchCount].X < mLogoutArea.right) &&
                        (game->mTouchData[touchCount].Y > mLogoutArea.top) &&
                        (game->mTouchData[touchCount].Y < mLogoutArea.bottom)) {

                    mSocial->_getSession(mNetworkID)->close();
                    mDispPic[mNetworkID] = false;
                    switch (mNetworkID) {

                        case Network::FACEBOOK: mFacebookURL.clear(); break;
                        case Network::TWITTER: mTwitterURL.clear(); break;
                        case Network::GOOGLE: mGoogleURL.clear(); break;
                    }
                    count = 0;
                    mNetworkID = Network::NONE;
                    mStatus = SHARE_HIDDING;
                    break;
                }

                // Check share
                if ((game->mTouchData[touchCount].X > mLogShareArea.left) &&
                        (game->mTouchData[touchCount].X < mLogShareArea.right) &&
                        (game->mTouchData[touchCount].Y > mLogShareArea.top) &&
                        (game->mTouchData[touchCount].Y < mLogShareArea.bottom)) {

                    if (!mInternet->sendHTTP(WITCHSPET_HOST, getRequestURL())) {

                        LOGW(LOG_FORMAT(" - Failed to send HTTP request"), __PRETTY_FUNCTION__, __LINE__);
                        mSocial->request(mNetworkID, Session::REQUEST_SHARE_LINK, NULL); // Display alert message
                        break;
                    }
                    httpSent = true;
                    break;
                }
            }
            break;
        }
        case SHARE_HIDDING: {

            if (++count != (DISP_UPD_COUNT + 1)) {

                mShareA.translate(-SOCIAL_VEL, 0.f);
                mShareB.translate(-SOCIAL_VEL, 0.f);
                mPicture.translate(-SOCIAL_VEL, 0.f);
                mNetworkPic.translate(-SOCIAL_VEL, 0.f);
                if (mURL) {

                    mGender.setAlpha(0.f);
                    mBirthday.setAlpha(0.f);

                    delete mURL;
                    mURL = NULL;
                }
            }
            else {

                count = 0;
                mStatus = NETWORKS_DISPLAYING;
            }
            break;
        }
    }
    return false;
}
void Share::render() const {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, (*mLog % 100), LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#endif
    switch (mStatus) {
        case PANEL_DISPLAYING:
        case PANEL_DISPLAYED:
        case PANEL_HIDDING: {

            mPanel.render(true);
            break;
        }
        case NETWORKS_DISPLAYING:
        case NETWORKS_DISPLAYED:
        case NETWORKS_HIDDING: {

            mNetworks.render(true);
            break;
        }
        case SHARE_DISPLAYING:
        case SHARE_DISPLAYED:
        case SHARE_HIDDING: {

            glDisable(GL_BLEND);
            mPicture.render(true);
            glEnable(GL_BLEND);

            mShareA.render(true);
            mShareB.render(true);
            mNetworkPic.render(true);

            mGender.render(true);
            mBirthday.render(true);
            if (mURL)
                mURL->render(true);
            break;
        }
    }
}
