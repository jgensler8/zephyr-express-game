#pragma once
#include <gbdk/platform.h>

#ifdef NINTENDO_NES
#define GET_8x16_SPRITE_TILE(tile) (tile + 1)
#else
#define GET_8x16_SPRITE_TILE(tile) (tile)
#endif

#ifdef NINTENDO_NES
#define PLATFORM_X_ADJUST 0
#define PLATFORM_Y_ADJUST 0
#else
#define PLATFORM_X_ADJUST 8
#define PLATFORM_Y_ADJUST 16
#endif

#define ANIMATION_MAX_FRAMES 5
struct animation_const
{
  uint8_t sp_idx;
  uint8_t data_idx;
  uint8_t frames;
  uint8_t neutral_frame;
  uint8_t data_tile_count;
  uint8_t *frame_datas[ANIMATION_MAX_FRAMES];
  uint8_t frame_wait;
};

enum direction
{
  DIRECTION_LEFT = -1,
  DIRECTION_NONE = 0,
  DIRECTION_RIGHT = 1,
};

struct animation_state
{
  uint8_t frame;
  int8_t direction;
  uint8_t frame_tick;
};

struct player_position
{
  uint8_t car;
  uint8_t x;
  int8_t y;
  int8_t direction;
};

enum TOOL
{
  TOOL_WIFI,
  TOOL_WRENCH,
  TOOL_DRINK,
  TOOL_CAT,
  TOOL_MUSIC,
  TOOL_COUNT,
  TOOL_NONE = 254,
};

#define PLAYER_HOLDING_NONE 254
struct tool
{
  uint8_t unlocked;
  uint8_t car;
  uint8_t x;
  uint8_t y;
  uint8_t player_holding;
};

#define TASK_PROGRESS_INIT 120
struct task
{
  uint8_t tool;
  uint8_t progress;
};

// 4 player sprites
// 5 tools
// 10 tasks
#define SPRITES_PER_PLAYER (19)

// #define MAX_PLAYERS
#ifdef NINTENDO_NES
#define MAX_PLAYERS 2
#define MAX_NPCS 0
// #define MAX_NPCS 0
// #define MAX_PLAYERS 4
// #define MAX_NPCS 0
#define MAX_PLAYABLES (MAX_PLAYERS + MAX_NPCS)
#else
#define MAX_PLAYERS 1
#define MAX_NPCS 3
#define MAX_PLAYABLES (MAX_PLAYERS + MAX_NPCS)
#endif
#define MAX_CARS 6
#define MAX_TASKS_PER_TOOL 2
#define TASK_SLOTS_PER_CAR 8
struct game_state
{
  uint8_t cars;
  uint8_t round_score;
  struct player_position player_positions[MAX_PLAYABLES];
  uint8_t player_car_changed[MAX_PLAYERS];
  struct animation_state player_animations[MAX_PLAYABLES];
  struct tool tools[TOOL_COUNT];
  uint8_t open_task_count;
  struct task tasks[MAX_CARS][TASK_SLOTS_PER_CAR];
};

struct scene
{
    void (*init)(void);
    void (*loop)(void);
};