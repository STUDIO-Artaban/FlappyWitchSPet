#include "Main.h"
#include <libeng/Advertising/Advertising.h>

////// Languages
typedef enum {

    LANG_EN = 0,
    LANG_FR,
    LANG_DE,
    LANG_ES,
    LANG_IT,
    LANG_PT

} Language;
static const Language g_MainLang = LANG_EN;

////// Textures
#define NO_TEXTURE_LOADED       0xFF

BOOL engGetFontGrayscale() { return YES; }

#define TEXTURE_ID_PUMPKIN      2 // TEXTURE_ID_FONT + 1

#define TEXTURE_ID_BACK         3
#define TEXTURE_ID_MOON         4
#define TEXTURE_ID_HALLOWEEN    5
#define TEXTURE_ID_TWILIGHT     6
#define TEXTURE_ID_FLOOR        7

#define TEXTURE_ID_TITLE        8
#define TEXTURE_ID_START        9
#define TEXTURE_ID_SCORE        10
#define TEXTURE_ID_FACE         11

#define TEXTURE_ID_GRAYWITCH    12

#define TEXTURE_ID_PANEL        13
#define TEXTURE_ID_SCORES       14

#define TEXTURE_ID_WITCH0       15
#define TEXTURE_ID_WITCH1       16
#define TEXTURE_ID_WITCH2       17
#define TEXTURE_ID_WITCH3       18
#define TEXTURE_ID_WITCH4       19
#define TEXTURE_ID_WITCH5       20
#define TEXTURE_ID_WITCH6       21
#define TEXTURE_ID_WITCH7       22

#define TEXTURE_ID_FAILED       23

#define TEXTURE_ID_TOWER        24
#define TEXTURE_ID_TOWERS       25

#define TEXTURE_ID_SHARE        26

unsigned char engLoadTexture(EngResources* resources, unsigned char Id) {
    switch (Id) {

        case TEXTURE_ID_PUMPKIN: {
            
            unsigned char* data = [resources getBufferPNG:@"pumpkin" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_PUMPKIN, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }

        case TEXTURE_ID_BACK: {
            
            unsigned char* data = [resources getBufferPNG:@"back" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_BACK, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_MOON: {

            unsigned char* data = [resources getBufferPNG:@"moon" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_MOON, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_HALLOWEEN: {
            
            unsigned char* data = [resources getBufferPNG:@"halloween" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_HALLOWEEN, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_TWILIGHT: {

            unsigned char* data = [resources getBufferPNG:@"twilight" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_TWILIGHT, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_FLOOR: {
            
            unsigned char* data = [resources getBufferPNG:@"floor" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_FLOOR, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }

        case TEXTURE_ID_TITLE: {
            
            unsigned char* data = [resources getBufferPNG:@"title" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_TITLE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_START: {
            
            unsigned char* data = [resources getBufferPNG:@"start" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_START, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_SCORE: {
            
            unsigned char* data = [resources getBufferPNG:@"score" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_SCORE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_FACE: {
            
            unsigned char* data = [resources getBufferPNG:@"face" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_FACE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }

        case TEXTURE_ID_GRAYWITCH: {
            
            unsigned char* data = [resources getBufferPNG:@"witch-3" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_GRAYWITCH, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }

        case TEXTURE_ID_PANEL: {
            
            unsigned char* data = [resources getBufferPNG:@"panel" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_PANEL, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_SCORES: {
            
            unsigned char* data = [resources getBufferPNG:@"scores" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_SCORES, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }

        case TEXTURE_ID_WITCH0:
        case TEXTURE_ID_WITCH1:
        case TEXTURE_ID_WITCH2:
        case TEXTURE_ID_WITCH3:
        case TEXTURE_ID_WITCH4:
        case TEXTURE_ID_WITCH5:
        case TEXTURE_ID_WITCH6:
        case TEXTURE_ID_WITCH7: {

            NSString* animImg = [[NSString alloc] initWithFormat:@"%@%d", @"witch-", (Id - TEXTURE_ID_WITCH0)];
            unsigned char* data = [resources getBufferPNG:animImg inGrayScale:NO];
            [animImg release];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(Id, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
            
        case TEXTURE_ID_FAILED: {
            
            unsigned char* data = [resources getBufferPNG:@"failed" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_FAILED, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
            
        case TEXTURE_ID_TOWER: {
            
            unsigned char* data = [resources getBufferPNG:@"tower" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_TOWER, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_TOWERS: {
            
            unsigned char* data = [resources getBufferPNG:@"towers" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_TOWERS, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }

        case TEXTURE_ID_SHARE: {

            unsigned char* data = [resources getBufferPNG:@"share" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_SHARE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }

        default: {
            
            NSLog(@"ERROR: Failed to load PNG ID %d", Id);
            break;
        }
    }
    return NO_TEXTURE_LOADED;
}

////// Sounds
#define SOUND_ID_FLAP       1 // SOUND_ID_LOGO + 1
#define SOUND_ID_TOUCHED    2
#define SOUND_ID_SCORE      3

#define SOUND_ID_SCREAM1    4
#define SOUND_ID_SCREAM2    5
#define SOUND_ID_SCREAM3    6
#define SOUND_ID_SCREAM4    7

void engLoadSound(EngResources* resources, unsigned char Id) {
    switch (Id) {

        case SOUND_ID_FLAP: {
            
            unsigned char* data = [resources getBufferOGG:@"flap"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_FLAP, resources.oggLength, data, false);
            break;
        }
        case SOUND_ID_TOUCHED: {

            unsigned char* data = [resources getBufferOGG:@"touched"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_TOUCHED, resources.oggLength, data, false);
            break;
        }
        case SOUND_ID_SCORE: {

            unsigned char* data = [resources getBufferOGG:@"score"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_SCORE, resources.oggLength, data, false);
            break;
        }

        case SOUND_ID_SCREAM1:
        case SOUND_ID_SCREAM2:
        case SOUND_ID_SCREAM3:
        case SOUND_ID_SCREAM4: {

            NSString* sndFile = [[NSString alloc] initWithFormat:@"%@%d", @"scream", (Id - SOUND_ID_SCREAM1 + 1)];
            unsigned char* data = [resources getBufferOGG:sndFile];
            [sndFile release];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(Id, resources.oggLength, data, true);
            break;
        }

        default: {
            
            NSLog(@"ERROR: Failed to load OGG ID %d", Id);
            break;
        }
    }
}

////// Advertising
#ifdef LIBENG_ENABLE_ADVERTISING

static NSString* ADV_UNIT_ID = @"ca-app-pub-1474300545363558/5596054624";
#ifdef DEBUG
static const NSString* IPAD_DEVICE_UID = @"655799b1c803de3417cbb36833b6c40c";
static const NSString* IPHONE_YACIN_UID = @"10053bb6983c6568b88812fbcfd7ab89";
#endif

#define ADV_ID_TITLE    0
#define ADV_ID_SCORE    1

BOOL engGetAdType() { return FALSE; } // TRUE: Interstitial; FALSE: Banner
void engLoadAd(EngAdvertising* ad, GADRequest* request) {

    static bool init = false;
    if (!init) {
        if ([[UIScreen mainScreen] bounds].size.width > 468)
            [ad getBanner].adSize = kGADAdSizeFullBanner;
        else
            [ad getBanner].adSize = kGADAdSizeBanner;
        [ad getBanner].adUnitID = ADV_UNIT_ID;
        init = true;
    }
#ifdef DEBUG
    request.testDevices = [NSArray arrayWithObjects: @"Simulator", IPAD_DEVICE_UID, IPHONE_YACIN_UID, nil];
#endif
    [[ad getBanner] loadRequest:request];
}
void engDisplayAd(EngAdvertising* ad, unsigned char Id) {

    CGFloat xPos = ([[UIScreen mainScreen] bounds].size.width - [ad getBanner].frame.size.width) / 2.0;
    [[ad getBanner] setHidden:NO];
    switch (Id) {
        case ADV_ID_TITLE: {

            [ad getBanner].frame =  CGRectMake(xPos, -[ad getBanner].frame.size.height,
                                        [ad getBanner].frame.size.width, [ad getBanner].frame.size.height);
            [UIView animateWithDuration:1.5 animations:^{
                [ad getBanner].frame =  CGRectMake(xPos, 0, [ad getBanner].frame.size.width,
                                                   [ad getBanner].frame.size.height);
            } completion:^(BOOL finished) {
                if (finished)
                    ad.status = static_cast<unsigned char>(Advertising::STATUS_DISPLAYED);
            }];
            break;
        }
        case ADV_ID_SCORE: {
            
            [ad getBanner].frame =  CGRectMake(xPos, [[UIScreen mainScreen] bounds].size.height,
                                        [ad getBanner].frame.size.width, [ad getBanner].frame.size.height);
            [UIView animateWithDuration:1.5 animations:^{
                [ad getBanner].frame =  CGRectMake(xPos, [[UIScreen mainScreen] bounds].size.height -
                                        [ad getBanner].frame.size.height, [ad getBanner].frame.size.width,
                                        [ad getBanner].frame.size.height);
            } completion:^(BOOL finished) {
                if (finished)
                    ad.status = static_cast<unsigned char>(Advertising::STATUS_DISPLAYED);
            }];
            break;
        }
    }
}
void engHideAd(EngAdvertising* ad, unsigned char Id) {
    [[ad getBanner] setHidden:YES];
}
#endif

////// Social
#ifdef LIBENG_ENABLE_SOCIAL
BOOL engReqInfoField(SocialField field, unsigned char socialID) {

    switch (socialID) {
        case Network::FACEBOOK: {

            switch (field) {
                //case FIELD_NAME: return YES;
                //case FIELD_GENDER: return YES;
                case FIELD_BIRTHDAY: return YES;
                case FIELD_LOCATION: return YES;
                default: return YES; // FIELD_NAME & FIELD_GENDER are always retrieved
            }
            break;
        }
        case Network::GOOGLE:
            return YES; // All fields are always retrieved (if any)

        default: {

            NSLog(@"ERROR: Wrong social identifier %d", socialID);
            break;
        }
    }
    return NO;
}
#endif
