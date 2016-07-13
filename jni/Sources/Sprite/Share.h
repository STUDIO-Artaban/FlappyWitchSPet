#ifndef SHARE_H_
#define SHARE_H_

#include "TouchArea.h"

#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Graphic/Text/2D/Text2D.h>

#include <libeng/Social/Social.h>
#include <libeng/Social/Network.h>
#include <libeng/Features/Internet/Internet.h>

#include <string>
#include <map>
#include <assert.h>

using namespace eng;

//////
class Share {

private:
#ifdef DEBUG
    const unsigned int* mLog;
#endif
    Social* mSocial;
    Internet* mInternet;
    Network::ID mNetworkID;
    short mScore;

    // External profile URL
    std::string mFacebookURL;
    std::string mTwitterURL;
    std::string mGoogleURL;

    std::map<Network::ID, bool> mDispPic;

    void setNetwork();
    void setPicture();
    bool select();
    void displayInfo(const Game* game);

    std::string getUserURL() const;
    std::string getRequestURL() const;

    enum {

        ALL_HIDDEN = 0,
        PANEL_DISPLAYING,
        PANEL_DISPLAYED,
        PANEL_HIDDING,
        NETWORKS_DISPLAYING,
        NETWORKS_DISPLAYED,
        NETWORKS_HIDDING,
        SHARE_DISPLAYING,
        SHARE_DISPLAYED,
        SHARE_HIDDING
    };
    unsigned char mStatus;

    Element2D mPanel;
    Element2D mNetworks;

    Element2D mShareA;
    Element2D mShareB;
    Element2D mPicture;
    Element2D mNetworkPic;
    Text2D mGender;
    Text2D mBirthday;
    Text2D* mURL;

    TouchArea mFacebookArea;
    TouchArea mTwitterArea;
    TouchArea mGoogleArea;

    TouchArea mShareArea;
    TouchArea mCloseNetArea;
    TouchArea mCloseLogArea;
    TouchArea mLogShareArea;
    TouchArea mLogoutArea;

public:
    Share();
    virtual ~Share();

    inline void display() {

        if (mStatus != PANEL_DISPLAYED)
            mStatus = PANEL_DISPLAYING;
    }
    inline void reset() {

        mPanel.reset();
        mPanel.translate(mPanel.getLeft() - mPanel.getRight(), 0.f);

        mStatus = ALL_HIDDEN;
    }
    inline bool isDisplaying() const { return (mStatus == PANEL_DISPLAYING); }
    inline void setScore(short score) { mScore = score; }

    //////
    void start(const Game* game);
    inline void pause() {

        mPanel.pause();
        mNetworks.pause();
        mShareA.pause();
        mShareB.pause();
        mPicture.pause();
        mNetworkPic.pause();

        mGender.pause();
        mBirthday.pause();
        if (mURL)
            mURL->pause();
    }
    void resume();

    bool update(const Game* game);
    void render() const;

};

#endif // SHARE_H_
