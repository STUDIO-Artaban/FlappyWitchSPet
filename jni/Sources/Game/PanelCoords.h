#ifndef PANELCOORDS_H_
#define PANELCOORDS_H_

#define PANEL_TEXTURE_SIZE      1024.f

// Pause
#define PAUSE_SIZE              128
#define PAUSE_A_X0              414
#define PAUSE_A_Y0              128
#define PAUSE_B_X0              PAUSE_A_X0

// Play
#define PLAY_X0                 (PAUSE_A_X0 + PAUSE_SIZE)
#define PLAY_Y0                 PAUSE_A_Y0
#define PLAY_X2                 683
#define PLAY_Y2                 277

// Arrow
#define ARROW_X0                877
#define ARROW_Y2                263

// Top back
#define TOP_A_X0                542
#define TOP_A_X2                ARROW_X0
#define TOP_A_Y2                128

// Hand
#define HAND_X0                 277
#define HAND_X2                 PAUSE_A_X0
#define HAND_Y2                 277

// Medal
#define MEDAL_X2                HAND_X0
#define MEDAL_Y2                HAND_Y2

// Panel
#define PANEL_Y0                277

#define SCORE_X_POS             584
#define SCORE_Y_POS             526
#define BEST_X_POS              SCORE_X_POS
#define BEST_Y_POS              657

#define MEDAL_X_POS             133
#define MEDAL_Y_POS             536
#define MEDAL_SIZE_POS          210

#define NEW_A_X0                381
#define NEW_A_Y0                594
#define NEW_A_W                 109
#define NEW_A_H                 52

#define OK_X0                   815
#define OK_Y0                   767
#define OK_Y2                   927

// GetReady (Font texture)
#define READY_Y0                656
#define READY_Y2                872
#define READY_X2                934

// ToP (Font texture)
#define TOP_B_Y0                568
#define TOP_B_Y2                READY_Y0
#define TOP_B_X2                234

// GAME OVER (Font texture)
#define GAMEOVER_Y0             READY_Y2

// New (Font texture)
#define NEW_B_X0                234
#define NEW_B_Y0                568
#define NEW_B_X2                449
#define NEW_B_Y2                657

// Dinosaur picture (Font texture)
#define DINO_X0                 449
#define DINO_Y0                 569
#define DINO_X2                 502
#define DINO_Y2                 614

// Ghost picture (Font texture)
#define GHOST_X0                503
#define GHOST_Y0                569
#define GHOST_X2                596
#define GHOST_Y2                615

// Cemetery picture (Font texture)
#define CEMETERY_X0             597
#define CEMETERY_Y0             569
#define CEMETERY_X2             725
#define CEMETERY_Y2             621

// Computer picture (Font texture)
#define COMPUTER_X0             726
#define COMPUTER_Y0             569
#define COMPUTER_X2             787
#define COMPUTER_Y2             628

// Skull picture (Font texture)
#define SKULL_X0                788
#define SKULL_Y0                569
#define SKULL_X2                826
#define SKULL_Y2                623

// Flappy picture (Font texture)
#define FLAPPY_X0               827
#define FLAPPY_Y0               569
#define FLAPPY_X2               901
#define FLAPPY_Y2               621

// Share panel (Share texture)
#define SHARE_HEIGHT            256
#define SHARE_WIDTH             368

#define SHARE_X0                (1024 - SHARE_HEIGHT)
#define SHARE_Y2                (1024 - SHARE_WIDTH)

#define SHARE_AREA_LEFT         99
#define SHARE_AREA_TOP          43
#define SHARE_AREA_RIGHT        330
#define SHARE_AREA_BOTTOM       213

// Social networks (Share texture)
#define NETWORKS_Y0             552
#define NETWORKS_X2             653

#define CLOSENET_AREA_LEFT      479
#define CLOSENET_AREA_TOP       923
#define CLOSENET_AREA_RIGHT     639
#define CLOSENET_AREA_BOTTOM    1015

// Network picture position (Share texture)
#define NETWORK_PIC_LEFT        848
#define NETWORK_PIC_TOP         370
#define NETWORK_PIC_SIZE        112

// Facebook (Share texture)
#define FACEBOOK_PIC_X0         67
#define FACEBOOK_PIC_Y0         683
#define FACEBOOK_PIC_X2         216
#define FACEBOOK_PIC_Y2         849

// Twitter (Share texture)
#define TWITTER_PIC_X0          245
#define TWITTER_PIC_Y0          FACEBOOK_PIC_Y0
#define TWITTER_PIC_X2          394
#define TWITTER_PIC_Y2          FACEBOOK_PIC_Y2

// Google+ (Share texture)
#define GOOGLE_PIC_X0           422
#define GOOGLE_PIC_Y0           FACEBOOK_PIC_Y0
#define GOOGLE_PIC_X2           571
#define GOOGLE_PIC_Y2           FACEBOOK_PIC_Y2

// Share dialog (Share texture)
#define SHARE_A_Y0              76
#define SHARE_A_Y2              552

#define SHARE_B_X0              789
#define SHARE_B_Y0              SHARE_A_Y2
#define SHARE_B_Y2              657

#define CLOSELOG_AREA_LEFT      815
#define CLOSELOG_AREA_TOP       528
#define CLOSELOG_AREA_RIGHT     973
#define CLOSELOG_AREA_BOTTOM    635

#define LOGSHARE_AREA_LEFT      294
#define LOGSHARE_AREA_TOP       450
#define LOGSHARE_AREA_RIGHT     457
#define LOGSHARE_AREA_BOTTOM    545

#define LOGOUT_AREA_LEFT        565
#define LOGOUT_AREA_TOP         LOGSHARE_AREA_TOP
#define LOGOUT_AREA_RIGHT       728
#define LOGOUT_AREA_BOTTOM      LOGSHARE_AREA_BOTTOM

#define GENDER_LEFT             484
#define GENDER_TOP              211

#define BIRTHDAY_LEFT           801
#define BIRTHDAY_TOP            211

#define URL_LEFT                308
#define URL_TOP                 301

// Picture position (Share texture)
#define PICTURE_LEFT            64
#define PICTURE_TOP             208
#define PICTURE_SIZE            230

#endif // PANELCOORDS_H_
