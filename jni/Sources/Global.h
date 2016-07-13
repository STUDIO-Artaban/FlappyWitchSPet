#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifndef __ANDROID__
#define __ANDROID__
#endif
#ifdef __ANDROID__
////// DEBUG | RELEASE

// Debug
//#ifndef DEBUG
//#define DEBUG
//#endif
//#undef NDEBUG

// Relase
#ifndef NDEBUG
#define NDEBUG
#endif
#undef DEBUG

#endif // __ANDROID__


//#ifdef DEBUG
#define DEMO_VERSION
//#endif

#define DISP_UPD_COUNT              16 // Update count B4 having fully displayed the title/scores/game
#define REF_SCREEN_RATIO            (600.f / 976.f) // Samsung Galaxy Tab 2 (W:600 / H:976)

// Log levels (< 5 to log)
#define LOG_LEVEL_WITCHSPET         5
#define LOG_LEVEL_TITLELEVEL        5
#define LOG_LEVEL_GAMELEVEL         5
#define LOG_LEVEL_WITCH             5
#define LOG_LEVEL_SCORES            5
#define LOG_LEVEL_TOWERS            5
#define LOG_LEVEL_BURY              5
#define LOG_LEVEL_SHARE             4

#endif // GLOBAL_H_
