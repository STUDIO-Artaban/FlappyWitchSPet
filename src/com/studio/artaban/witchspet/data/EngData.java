package com.studio.artaban.witchspet.data;

import com.studio.artaban.witchspet.EngActivity;
import com.studio.artaban.witchspet.EngAdvertising;
import com.studio.artaban.witchspet.EngLibrary;
import com.studio.artaban.witchspet.EngResources;

import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.AnimationUtils;
import android.widget.RelativeLayout;

import com.studio.artaban.witchspet.R;

public class EngData {

    ////// JNI
    static public final String PROJECT_NAME_LIB = "WitchSPet";

    ////// Languages
    static private final short LANG_EN = 0;
    @SuppressWarnings("unused") static private final short LANG_FR = 1;
    @SuppressWarnings("unused") static private final short LANG_DE = 2;
    @SuppressWarnings("unused") static private final short LANG_ES = 3;
    @SuppressWarnings("unused") static private final short LANG_IT = 4;
    @SuppressWarnings("unused") static private final short LANG_PT = 5;

    @SuppressWarnings("unused") static private final short mLanguage = LANG_EN;

    ////// Permissions
    static public final boolean USES_PERMISSION_CAMERA = false; // android.permission.CAMERA
    static public final boolean USES_PERMISSION_MIC = false; // android.permission.RECORD_AUDIO
    static public final boolean USES_PERMISSION_INTERNET = true; // android.permission.INTERNET & ACCESS_NETWORK_STATE
    static public final boolean USES_PERMISSION_STORAGE = false; // android.permission.WRITE_INTERNAL_STORAGE & WRITE_EXTERNAL_STORAGE
    static public final boolean USES_PERMISSION_BLUETOOTH = false; // android.permission.BLUETOOTH & BLUETOOTH_ADMIN

    ////// Font
    static public final boolean FONT_TEXTURE_GRAYSCALE = true;

    ////// Textures
    static private final short TEXTURE_ID_PUMPKIN = 2; // EngActivity.TEXTURE_ID_FONT + 1

    static private final short TEXTURE_ID_BACK = 3;
    static private final short TEXTURE_ID_MOON = 4;
    static private final short TEXTURE_ID_HALLOWEEN = 5;
    static private final short TEXTURE_ID_TWILIGHT = 6;
    static private final short TEXTURE_ID_FLOOR = 7;

    static private final short TEXTURE_ID_TITLE = 8;
    static private final short TEXTURE_ID_START = 9;
    static private final short TEXTURE_ID_SCORE = 10;
    static private final short TEXTURE_ID_FACE = 11;

    static private final short TEXTURE_ID_GRAYWITCH = 12;

    static private final short TEXTURE_ID_PANEL = 13;
    static private final short TEXTURE_ID_SCORES = 14;

    static private final short TEXTURE_ID_WITCH0 = 15;
    static private final short TEXTURE_ID_WITCH1 = 16;
    static private final short TEXTURE_ID_WITCH2 = 17;
    static private final short TEXTURE_ID_WITCH3 = 18;
    static private final short TEXTURE_ID_WITCH4 = 19;
    static private final short TEXTURE_ID_WITCH5 = 20;
    static private final short TEXTURE_ID_WITCH6 = 21;
    static private final short TEXTURE_ID_WITCH7 = 22;

    static private final short TEXTURE_ID_FAILED = 23;

    static private final short TEXTURE_ID_TOWER = 24;
    static private final short TEXTURE_ID_TOWERS = 25;

    static private final short TEXTURE_ID_SHARE = 26;

    static public short loadTexture(EngResources resources, short id) {

        switch (id) {
            case TEXTURE_ID_PUMPKIN: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("pumpkin.png");
                return EngLibrary.loadTexture(TEXTURE_ID_PUMPKIN, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            case TEXTURE_ID_BACK: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("back.png");
                return EngLibrary.loadTexture(TEXTURE_ID_BACK, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_MOON: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("moon.png");
                return EngLibrary.loadTexture(TEXTURE_ID_MOON, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_HALLOWEEN: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("halloween.png");
                return EngLibrary.loadTexture(TEXTURE_ID_HALLOWEEN, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_TWILIGHT: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("twilight.png");
                return EngLibrary.loadTexture(TEXTURE_ID_TWILIGHT, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_FLOOR: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("floor.png");
                return EngLibrary.loadTexture(TEXTURE_ID_FLOOR, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            case TEXTURE_ID_TITLE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("title.png");
                return EngLibrary.loadTexture(TEXTURE_ID_TITLE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_START: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("start.png");
                return EngLibrary.loadTexture(TEXTURE_ID_START, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_SCORE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("score.png");
                return EngLibrary.loadTexture(TEXTURE_ID_SCORE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_FACE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("face.png");
                return EngLibrary.loadTexture(TEXTURE_ID_FACE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            case TEXTURE_ID_GRAYWITCH: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("witch-3.png");
                return EngLibrary.loadTexture(TEXTURE_ID_GRAYWITCH, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }

            case TEXTURE_ID_PANEL: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("panel.png");
                return EngLibrary.loadTexture(TEXTURE_ID_PANEL, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_SCORES: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("scores.png");
                return EngLibrary.loadTexture(TEXTURE_ID_SCORES, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }

            case TEXTURE_ID_WITCH0:
            case TEXTURE_ID_WITCH1:
            case TEXTURE_ID_WITCH2:
            case TEXTURE_ID_WITCH3:
            case TEXTURE_ID_WITCH4:
            case TEXTURE_ID_WITCH5:
            case TEXTURE_ID_WITCH6:
            case TEXTURE_ID_WITCH7: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("witch-" + (id - TEXTURE_ID_WITCH0) + ".png");
                return EngLibrary.loadTexture(id, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            case TEXTURE_ID_FAILED: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("failed.png");
                return EngLibrary.loadTexture(TEXTURE_ID_FAILED, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            case TEXTURE_ID_TOWER: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("tower.png");
                return EngLibrary.loadTexture(TEXTURE_ID_TOWER, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_TOWERS: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("towers.png");
                return EngLibrary.loadTexture(TEXTURE_ID_TOWERS, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            case TEXTURE_ID_SHARE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("share.png");
                return EngLibrary.loadTexture(TEXTURE_ID_SHARE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            default: {
                Log.e("EngData", "Failed to load PNG ID: " + Integer.toString(id));
                return EngActivity.NO_TEXTURE_LOADED;
            }
        }
    }

    ////// Sounds
    static private final short SOUND_ID_FLAP = 1; // EngActivity.SOUND_ID_LOGO + 1
    static private final short SOUND_ID_TOUCHED = 2;
    static private final short SOUND_ID_SCORE = 3;

    static private final short SOUND_ID_SCREAM1 = 4;
    static private final short SOUND_ID_SCREAM2 = 5;
    static private final short SOUND_ID_SCREAM3 = 6;

    static public void loadOgg(EngResources resources, short id) {

        switch (id) {
            case SOUND_ID_FLAP: {
                EngLibrary.loadOgg(SOUND_ID_FLAP, resources.getBufferOGG("flap.ogg"), false);
                break;
            }
            case SOUND_ID_TOUCHED: {
                EngLibrary.loadOgg(SOUND_ID_TOUCHED, resources.getBufferOGG("touched.ogg"), false);
                break;
            }
            case SOUND_ID_SCORE: {
                EngLibrary.loadOgg(SOUND_ID_SCORE, resources.getBufferOGG("score.ogg"), false);
                break;
            }
            case SOUND_ID_SCREAM1:
            case SOUND_ID_SCREAM2:
            case SOUND_ID_SCREAM3: {

                EngLibrary.loadOgg(id, resources.getBufferOGG("scream" + (id - SOUND_ID_SCREAM1 + 1) + ".ogg"), true);
                break;
            }
            default: {
                Log.e("EngData", "Failed to load OGG ID: " + Integer.toString(id));
                break;
            }
        }
    }

    ////// Advertising
    static public final boolean INTERSTITIAL_AD = false; // TRUE: InterstitialAd; FALSE: AdView
    static private final boolean TEST_ADVERTISING = false; // Set to 'false' in release mode
    static private final String ADV_UNIT_ID = "ca-app-pub-1474300545363558/8506153029";

    static private final short ADV_ID_TITLE = 0;
    static private final short ADV_ID_SCORE = 1;

    static private class AdAnimListener implements AnimationListener {

		public void onAnimationEnd(Animation animation) { EngAdvertising.mStatus = EngAdvertising.AD_STATUS_DISPLAYED; }
		public void onAnimationRepeat(Animation animation) { }
		public void onAnimationStart(Animation animation) { }
    };

    static public void loadAd(EngActivity activity) {

    	if (!USES_PERMISSION_INTERNET)
    		Log.e("EngData", "Missing INTERNET & ACCESS_NETWORK_STATE permissions");

    	//
    	DisplayMetrics metrics = new DisplayMetrics();
    	activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        if (((activity.getWindowManager().getDefaultDisplay().getWidth() * 160) / metrics.xdpi) > 468)
        	EngAdvertising.setBanner(EngAdvertising.BANNER_ID_FULL_BANNER);
        else
    		EngAdvertising.setBanner(EngAdvertising.BANNER_ID_BANNER);

		EngAdvertising.setUnitID(ADV_UNIT_ID);
		if (TEST_ADVERTISING)
			EngAdvertising.load(activity.getContentResolver());
		else
			EngAdvertising.load(null);
    }
    static public void displayAd(short id, EngActivity activity) {

		if (activity.mSurfaceLayout.getChildCount() == 2)
			activity.mSurfaceLayout.removeView(EngAdvertising.getView());
		EngAdvertising.getView().setVisibility(View.VISIBLE);

    	switch (id) {
	    	case ADV_ID_TITLE: {

	            RelativeLayout.LayoutParams adParams =
	            		new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT,
	            				RelativeLayout.LayoutParams.WRAP_CONTENT);
	            adParams.addRule(RelativeLayout.ALIGN_PARENT_TOP);
	            adParams.addRule(RelativeLayout.CENTER_HORIZONTAL);

	            activity.mSurfaceLayout.addView(EngAdvertising.getView(), adParams);

	            Animation animAds = (Animation)AnimationUtils.loadAnimation(activity, R.anim.disp_ad_top);
	            animAds.setAnimationListener(new AdAnimListener());
	            EngAdvertising.getView().startAnimation(animAds);

	            EngAdvertising.mStatus = EngAdvertising.AD_STATUS_DISPLAYING;
	            break;
	    	}
	    	case ADV_ID_SCORE: {

	            RelativeLayout.LayoutParams adParams =
	            		new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT,
	            				RelativeLayout.LayoutParams.WRAP_CONTENT);
	            adParams.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
	            adParams.addRule(RelativeLayout.CENTER_HORIZONTAL);

	            activity.mSurfaceLayout.addView(EngAdvertising.getView(), adParams);

	            Animation animAds = (Animation)AnimationUtils.loadAnimation(activity, R.anim.disp_ad_bottom);
	            animAds.setAnimationListener(new AdAnimListener());
	            EngAdvertising.getView().startAnimation(animAds);

	            EngAdvertising.mStatus = EngAdvertising.AD_STATUS_DISPLAYING;
	            break;
	    	}
    	}
    }
    static public void hideAd(short id, EngActivity activity) {

    	switch (id) {
    		case ADV_ID_TITLE:
	    	case ADV_ID_SCORE: {

    			EngAdvertising.getView().clearAnimation();

	            EngAdvertising.mStatus = EngAdvertising.AD_STATUS_LOADED;
	    		EngAdvertising.getView().setVisibility(View.INVISIBLE);
	    		// -> Always set invisible when hidden (see 'onAdLoaded' method in 'EngAdvertising')
	            break;
	    	}
    	}
    }

    ////// Social
    static public final boolean INFO_FIELD_FACEBOOK_BIRTHDAY = true;
    static public final boolean INFO_FIELD_FACEBOOK_LOCATION = true;
    // NB: INFO_FIELD_FACEBOOK_NAME & INFO_FIELD_FACEBOOK_GENDER are always 'true'

}
