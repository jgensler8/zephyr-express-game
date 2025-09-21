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
  uint8_t speed;
  int8_t continued_direction;
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
  uint8_t happiness_reward;
};

enum difficulty
{
  DIFFICULTY_CASUAL,
  DIFFICULTY_EASY,
  DIFFICULTY_HARD,
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
#else
#define MAX_PLAYERS 1
// #define MAX_NPCS 3
#define MAX_NPCS 0
#endif
#define MAX_PLAYABLES (MAX_PLAYERS + MAX_NPCS)
#define MAX_CARS 4
#define TASK_SLOTS_PER_CAR 8
// this would force us to define several constants for struct init, lets just stick with 4
#define STRUCT_MAX_PLAYABLES 4
// fix the round distance but leave ticks per distance variable
#define ROUND_DISTANCE 100
struct game_state
{
  uint8_t tutorial;
  enum difficulty difficulty;
  uint8_t round;
  uint8_t round_tasks;
  uint8_t current_distance;
  uint16_t round_distance_ticks;
  uint16_t current_distance_tick;
#define WALK_SPEED_MAX 2
  uint8_t walk_speed;
#define RUN_SPEED_MAX 4
  uint8_t run_speed;
#define RUN_TICKS_MIN 6
  uint8_t run_ticks;
#define TASK_SPEED_MAX 3
  uint8_t task_speed;
  uint8_t cars;
  int16_t customer_happiness;
  uint8_t round_score;
  uint8_t unlocks_left;
  struct player_position player_positions[STRUCT_MAX_PLAYABLES];
  uint8_t player_car_changed[STRUCT_MAX_PLAYABLES];
  struct animation_state player_animations[STRUCT_MAX_PLAYABLES];
  struct tool tools[TOOL_COUNT];
#define MAX_OPEN_TASKS_UPGRADE_INTERVAL 4
#define MAX_OPEN_TASKS_MAX (2 + 3 * TASK_SLOTS_PER_CAR)
  uint8_t max_open_tasks;
  uint8_t open_task_count;
  struct task tasks[MAX_CARS][TASK_SLOTS_PER_CAR];
};

extern struct game_state default_state;
extern struct game_state starter_state;
extern struct game_state state;
void init_state(enum difficulty difficulty);
void advance_state(void);

enum scene
{
  SCENE_MAIN_MENU,
  SCENE_TUTORIAL,
  SCENE_DIFFICULTY_SELECT,
  SCENE_GAMEPLAY,
  SCENE_UPGRADE_MENU,
};

#ifdef GAMEBOY
#define FILL_BKG_EMPTY fill_bkg_rect(0, 0, 20, 18, 0)
#else
#define FILL_BKG_EMPTY fill_bkg_rect(0, 0, 32, 30, 0)
#endif

#define HIDE_SPRITES_RANGE hide_sprites_range(0, MAX_HARDWARE_SPRITES)