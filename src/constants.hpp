/*!
  TinyDarius

  @file constants.hpp
  @brief Constants definition
*/
#ifndef TD_CONSTANTS_HPP
#define TD_CONSTANTS_HPP

#include <cstdint>
#include "typedef.hpp"

// Screen width/height
constexpr std::int32_t SCREEN_WIDTH = 320;
constexpr std::int32_t SCREEN_HEIGHT = 240;

// Palette definition using RGB888.
constexpr std::uint32_t CLR_BLACK =     0x00000000U;
constexpr std::uint32_t CLR_WHITE =     0x00FFFFFFU;
constexpr std::uint32_t CLR_GRAY =      0x00808080U;
constexpr std::uint32_t CLR_RED =       0x00FF0000U;
constexpr std::uint32_t CLR_YELLOW =    0x00FFFF00U;
constexpr std::uint32_t CLR_MAGENTA =   0x00FF00FFU;
constexpr std::uint32_t CLR_GREEN =     0x0000FF00U;

// Transpalent palette index
constexpr std::int32_t TRANPARENT_PALETTE_INDEX = 0;

// Task priority
constexpr std::uint32_t PRIORITY_DEBUG = 0;
constexpr std::uint32_t PRIORITY_SCENE_MAANGER = 0;
constexpr std::uint32_t PRIORITY_ADVERTISE = 100;
constexpr std::uint32_t PRIORITY_GAME = 100;
constexpr std::uint32_t PRIORITY_STAGE = 100;
constexpr std::uint32_t PRIORITY_WAVESPOT = 200;
constexpr std::uint32_t PRIORITY_ROCKSURFACE = 210;
constexpr std::uint32_t PRIORITY_BRANCHROCK = 210;
constexpr std::uint32_t PRIORITY_SILVERHAWK = 1000;
constexpr std::uint32_t PRIORITY_BULLET = 1001;
constexpr std::uint32_t PRIORITY_BOSS = 1100;
constexpr std::uint32_t PRIORITY_BOSS_BULLET = 1110;
constexpr std::uint32_t PRIORITY_ENEMY = 1200;
constexpr std::uint32_t PRIORITY_ENEMY_BULLET = 1210;
constexpr std::uint32_t PRIORITY_EXPLOSION = 2000;
constexpr std::uint32_t PRIORITY_MESSAGE = 2100;
constexpr std::uint32_t PRIORITY_INFORMATION = 2200;
constexpr std::uint32_t PRIORITY_MASK = 2300;

// Task category(for Hitcheck)
constexpr std::uint32_t CATEGORY_BIT_BULLET =   0X80000000;
constexpr std::uint32_t CATEGORY_BIT_BOSS = 0X40000000;
constexpr std::uint32_t CATEGORY_BIT_ENEMY =    0X20000000;
constexpr std::uint32_t CATEGORY_BIT_OBSTACLE=  0X10000000;
constexpr std::uint32_t CATEGORY_BIT_PLAYER =   0X08000000;
//
constexpr std::uint32_t CATEGORY_NONE =     0X00000000;
constexpr std::uint32_t CATEGORY_PLAYER =   CATEGORY_BIT_PLAYER;;
constexpr std::uint32_t CATEGORY_BULLET =   CATEGORY_BIT_PLAYER | CATEGORY_BIT_BULLET;
constexpr std::uint32_t CATEGORY_BOSS =     CATEGORY_BIT_BOSS;
constexpr std::uint32_t CATEGORY_ENEMY =    CATEGORY_BIT_ENEMY;
constexpr std::uint32_t CATEGORY_ENEMY_BULLET = CATEGORY_BIT_ENEMY | CATEGORY_BIT_BULLET;
constexpr std::uint32_t CATEGORY_WALL =     CATEGORY_BIT_OBSTACLE;


// Rendering order (like the Z length)
constexpr std::uint32_t ORDER_DEBUG = 0;
constexpr std::uint32_t ORDER_ADVERTISEE = 1000;
constexpr std::uint32_t ORDER_MESSAGE = 40;
constexpr std::uint32_t ORDER_INFORMATION = 50;
constexpr std::uint32_t ORDER_MASK = 75;;
constexpr std::uint32_t ORDER_EXPLOSION = 100;
constexpr std::uint32_t ORDER_ROCKSURFACE = 200;
constexpr std::uint32_t ORDER_BRANCHROCK = 200;
constexpr std::uint32_t ORDER_EXPLOSION_BOSS = 250;
constexpr std::uint32_t ORDER_EXPLOSION_ENEMY = 260;
constexpr std::uint32_t ORDER_BOSS_BULLET = 500;
constexpr std::uint32_t ORDER_ENEMY = 510;
constexpr std::uint32_t ORDER_ENEMY_BULLET = 520;
constexpr std::uint32_t ORDER_BULLET = 900;
constexpr std::uint32_t ORDER_SILVERHAWK = 1000;
constexpr std::uint32_t ORDER_BOSS_BODY = 2000;
constexpr std::uint32_t ORDER_WAVESPOT = 5000;

// Clipping rect
constexpr Rect2 SCREEN_RECT = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
constexpr Rect2 FIELD_RECT = { 0, 24, SCREEN_WIDTH, SCREEN_HEIGHT - 24*2 };

// Task message
constexpr std::uint32_t MSG_DEFEAT_BOSS = 52;
constexpr std::uint32_t MSG_VANISH_ENEMY = 53;
constexpr std::uint32_t MSG_HIDE_EFFECT = 54;
constexpr std::uint32_t MSG_ROUND_CLEAR = 55;
constexpr std::uint32_t MSG_GAME_OVER = 56;

// Scene ID
constexpr std::uint32_t SCENE_DEBUG             = 1;
constexpr std::uint32_t SCENE_DEBUG_SOUND       = 2;
constexpr std::uint32_t SCENE_ADVERTISE         = 10;
constexpr std::uint32_t SCENE_GAME              = 100;
constexpr std::uint32_t SCENE_STAGE             = 101;

// BGM ID (Streaming by SD file)
enum BGM : std::uint8_t
{
    InsertCoin,
    InsertCoinB,
    // for stage
    CaptainNeo,
    ChaosMainTheme,
    CosmicAirWay,
    InorganicBeat,
    TheSea,
    // for boss
    Warning,
    BOSS_1,
    BOSS_2,
    BOSS_3,
    BOSS_4,
    BOSS_5,
    BOSS_6,
    BOSS_7,
    RoundClear,
    // for gameover
    Ending,
    Name,
    Gameover,
    Max
};

// SFX ID
enum class SFX : std::uint8_t
{
    InsertCoin,
    InsertCoinB,
    Shhot,
    Explosion,
    Max
};


// Score
constexpr std::uint32_t PTS_REMAIN_BONUS = 100000;
constexpr std::uint32_t PTS_TIME_BONUS = 100;
constexpr std::uint32_t PTS_MINE = 100;
constexpr std::uint32_t PTS_YAZUKA = 200;
constexpr std::uint32_t PTS_BOSS_PARTIAL = 5000;
constexpr std::uint32_t PTS_BOSS_BASE = 10000;

#endif
