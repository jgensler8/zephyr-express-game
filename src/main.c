#include <gbdk/platform.h>
#include <rand.h>
#include "gen/conductor.h"
#include "gen/train_map_0.h"
#include "gen/train_player_logo_0.h"
#include "gen/train_player_logo_1.h"
#include "gen/train_player_logo_2.h"
#include "gen/train_player_logo_3.h"
#include "gen/tool_wifi.h"
#include "gen/tool_wrench.h"
#include "gen/tool_drink.h"
#include "gen/tool_cat.h"
#include "gen/tool_music.h"
#include "gen/bg_train_passenger.h"
#define TRAIN_CAR_LEN bg_train_passenger_WIDTH
#define TRAIN_CAR_HEIGHT bg_train_passenger_HEIGHT
#define TRAIN_DOOR_TELEPORT_MARGIN 4
// adjust for sprite height
#define TRAIN_FLOOR_BASELINE (32)

#ifdef NINTENDO_NES
#define PLATFORM_X_ADJUST 0
#define PLATFORM_Y_ADJUST 0
#else
#define PLATFORM_X_ADJUST 8
#define PLATFORM_Y_ADJUST 16
#endif

#ifdef NINTENDO_NES
#define GET_8x16_SPRITE_TILE(tile) (tile + 1)
#else
#define GET_8x16_SPRITE_TILE(tile) (tile)
#endif

#ifdef NINTENDO_NES
// #define PLAYER_X_ADJUST(current_player) ((current_player == 0 || current_player == 2) ? 0 : 120)
#define PLAYER_X_ADJUST(current_player) (0)
uint8_t _player_y_adjust[4] = {TRAIN_CAR_HEIGHT * 0, TRAIN_CAR_HEIGHT * 1, TRAIN_CAR_HEIGHT * 2, TRAIN_CAR_HEIGHT * 3};
#define PLAYER_Y_ADJUST(current_player) (_player_y_adjust[current_player])
#else
#define PLAYER_X_ADJUST(current_player) (0)
#define PLAYER_Y_ADJUST(current_player) (0)
#endif

#define RANDOM_X_POSITION (30 + (rand() % 50))

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

struct task
{
  uint8_t car;
  uint8_t x;
  uint8_t y;
  uint8_t progress;
};

// #define MAX_PLAYERS
#ifdef NINTENDO_NES
// #define MAX_PLAYERS 2
// #define MAX_NPCS 2
#define MAX_PLAYERS 4
#define MAX_NPCS 0
#define MAX_PLAYABLES (MAX_PLAYERS + MAX_NPCS)
#else
#define MAX_PLAYERS 1
#define MAX_NPCS 3
#define MAX_PLAYABLES (MAX_PLAYERS + MAX_NPCS)
#endif
#define MAX_TASKS_PER_TOOL 2
struct game_state
{
  uint8_t cars;
  struct player_position player_positions[MAX_PLAYABLES];
  struct animation_state player_animations[MAX_PLAYABLES];
  struct tool tools[TOOL_COUNT];
  uint8_t open_task_count;
  struct task tasks[TOOL_COUNT][MAX_TASKS_PER_TOOL];
};

joypads_t joypads;

void update_tool_cars(struct game_state *state)
{
  for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
  {
    if (state->tools[tool].player_holding != PLAYER_HOLDING_NONE)
    {
      state->tools[tool].car = state->player_positions[state->tools[tool].player_holding].car;
    }
  }
}
uint8_t tool_held(struct game_state *state, uint8_t player)
{
  for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
  {
    if (state->tools[tool].player_holding == player)
    {
      return tool;
    }
  }
  return TOOL_NONE;
}
uint8_t tool_near(struct game_state *state, enum TOOL tool, uint8_t player)
{
  uint8_t nearness_margin_x = 16;
  uint8_t nearness_margin_y = 16;
  return state->tools[tool].x >= state->player_positions[player].x - nearness_margin_x &&
         state->tools[tool].x <= state->player_positions[player].x + nearness_margin_x &&
         state->tools[tool].y >= state->player_positions[player].y - nearness_margin_y &&
         state->tools[tool].y <= state->player_positions[player].y + nearness_margin_y;
}
uint8_t last_joy[MAX_PLAYERS];
void maybe_interpolate_direction(struct game_state *state, uint8_t player)
{
#define PLAYER_X state->player_positions[player].x
#define PLAYER_Y state->player_positions[player].y
#define PLAYER_DIRECTION state->player_positions[player].direction
  // right ladder enter top
  if (PLAYER_X == 110 && PLAYER_DIRECTION == DIRECTION_LEFT)
  {
    PLAYER_X -= 1;
    PLAYER_Y -= 1;
  }
  else if ((110 - 15) < PLAYER_X && PLAYER_X < 110 && PLAYER_Y < TRAIN_FLOOR_BASELINE)
  {
    if (PLAYER_DIRECTION == DIRECTION_LEFT)
    {
      PLAYER_X -= 1;
      PLAYER_Y -= 1;
    }
    else if (PLAYER_DIRECTION == DIRECTION_RIGHT)
    {
      PLAYER_X += 1;
      PLAYER_Y += 1;
    }
  }
  // left ladder leaving tops
  else if (PLAYER_X == 25 && PLAYER_DIRECTION == DIRECTION_LEFT && PLAYER_Y < TRAIN_FLOOR_BASELINE)
  {
    PLAYER_X -= 1;
    PLAYER_Y += 1;
  }
  else if ((25 - 15) < PLAYER_X && PLAYER_X < 25 && PLAYER_Y < TRAIN_FLOOR_BASELINE)
  {
    if (PLAYER_DIRECTION == DIRECTION_LEFT)
    {
      PLAYER_X -= 1;
      PLAYER_Y += 1;
    }
    else if (PLAYER_DIRECTION == DIRECTION_RIGHT)
    {
      PLAYER_X += 1;
      PLAYER_Y -= 1;
    }
  }
  // left ladder enter bottom
  else if (PLAYER_X == 20 && PLAYER_DIRECTION == DIRECTION_RIGHT && PLAYER_Y == TRAIN_FLOOR_BASELINE)
  {
    PLAYER_X += 1;
    PLAYER_Y += 1;
  }
  else if (20 < PLAYER_X && PLAYER_X < (20 + 17) && PLAYER_Y > TRAIN_FLOOR_BASELINE)
  {
    if (PLAYER_DIRECTION == DIRECTION_LEFT)
    {
      PLAYER_X -= 1;
      PLAYER_Y -= 1;
    }
    else if (PLAYER_DIRECTION == DIRECTION_RIGHT)
    {
      PLAYER_X += 1;
      PLAYER_Y += 1;
    }
  }
  // right ladder leaving bottom
  else if (PLAYER_X == 88 && PLAYER_DIRECTION == DIRECTION_RIGHT && PLAYER_Y > TRAIN_FLOOR_BASELINE)
  {
    PLAYER_X += 1;
    PLAYER_Y -= 1;
  }
  else if (88 < PLAYER_X && PLAYER_X < (88 + 17) && PLAYER_Y >= TRAIN_FLOOR_BASELINE)
  {
    if (PLAYER_DIRECTION == DIRECTION_LEFT)
    {
      PLAYER_X -= 1;
      PLAYER_Y += 1;
    }
    else if (PLAYER_DIRECTION == DIRECTION_RIGHT)
    {
      PLAYER_X += 1;
      PLAYER_Y -= 1;
    }
  }
  // all other movement
  else
  {
    if (PLAYER_DIRECTION == DIRECTION_LEFT && PLAYER_X > 0)
    {
      PLAYER_X += DIRECTION_LEFT;
    }
    else if (PLAYER_DIRECTION == DIRECTION_RIGHT && PLAYER_X < TRAIN_CAR_LEN)
    {
      PLAYER_X += DIRECTION_RIGHT;
    }
  }
}
void handle_input(struct game_state *state, uint8_t player)
{
  // uint8_t joy = joypad();
  uint8_t joy = joypads.joypads[player];
  // directional movment
  if (joy & J_LEFT)
  {
    state->player_positions[player].direction = DIRECTION_LEFT;
  }
  else if (joy & J_RIGHT)
  {
    state->player_positions[player].direction = DIRECTION_RIGHT;
  }
  else
  {
    state->player_positions[player].direction = DIRECTION_NONE;
  }
  maybe_interpolate_direction(state, player);

  // car teleports
  if (state->player_positions[player].x < TRAIN_DOOR_TELEPORT_MARGIN)
  {

    if (state->player_positions[player].car > 0)
    {
      state->player_positions[player].car -= 1;
      state->player_positions[player].x = TRAIN_CAR_LEN - TRAIN_DOOR_TELEPORT_MARGIN;
      update_tool_cars(state);
    }
  }
  else if (state->player_positions[player].x > TRAIN_CAR_LEN - TRAIN_DOOR_TELEPORT_MARGIN)
  {
    if (state->player_positions[player].car < state->cars - 1)
    {
      state->player_positions[player].car += 1;
      state->player_positions[player].x = TRAIN_DOOR_TELEPORT_MARGIN;
      update_tool_cars(state);
    }
  }

  // handle tools + tasks
  uint8_t j_a_pressed = joy & J_A && !(last_joy[player] & J_A);
  if (j_a_pressed)
  {
    uint8_t held = tool_held(state, player);
    if (held != TOOL_NONE)
    {
      // TODO: if near task
      // drop tool
      state->tools[held].player_holding = PLAYER_HOLDING_NONE;
    }
    else
    {
      // maybe pick up tool
      for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
      {
        if (state->tools[tool].unlocked &&
            state->tools[tool].car == state->player_positions[player].car &&
            state->tools[tool].player_holding == PLAYER_HOLDING_NONE &&
            tool_near(state, tool, player))
        {
          if (state->tools[tool].player_holding == PLAYER_HOLDING_NONE)
          {
            state->tools[tool].player_holding = player;
            // state->tools[tool].y -= 16;
            break;
          }
          else if (state->tools[tool].player_holding == player)
          {
            state->tools[tool].player_holding = PLAYER_HOLDING_NONE;
            // state->tools[tool].y += 16;
            break;
          }
        }
      }
    }
  }

  // save state
  last_joy[player] = joy;
}

void animation_progress(const struct animation_const *data, struct animation_state *state)
{
  // handle frame wait
  if (state->frame_tick > 0)
  {
    state->frame_tick -= 1;
    return;
  }
  state->frame_tick = data->frame_wait;
  // advance frame
  if (state->frame == 0)
  {
    state->direction = 1;
  }
  else if (state->frame == data->frames - 1)
  {
    state->direction = -1;
  }
  state->frame = state->frame + state->direction;
  set_sprite_data(data->data_idx, data->data_tile_count, data->frame_datas[state->frame]);
}

void animation_idle(const struct animation_const *data, struct animation_state *state)
{
  state->frame = data->neutral_frame;
  state->frame_tick = 0;
  state->direction = 1;
  set_sprite_data(data->data_idx, data->data_tile_count, data->frame_datas[state->frame]);
}
#define TRAIN_MAP_BG_X_START 1
#define TRAIN_MAP_CAR_TILE_START 1
void initialize_train_map(void)
{
  set_bkg_data(TRAIN_MAP_CAR_TILE_START, train_map_0_TILE_COUNT, train_map_0_tiles);
}

void draw_train_map(struct game_state *state)
{
  // draw cars left to right
  for (uint8_t car = 0; car < state->cars; car++)
  {
    uint8_t tile = 0;
    for (uint8_t t_y = 0; t_y < (train_map_0_HEIGHT / 8); t_y++)
    {
      for (uint8_t t_x = 0; t_x < (train_map_0_WIDTH / 8); t_x++)
      {
        set_bkg_tile_xy(TRAIN_MAP_BG_X_START + t_x + car * 3, t_y, TRAIN_MAP_CAR_TILE_START + tile);
        tile++;
      }
    }
  }
}

#define PLAYER_MAP_DATA_START 0
#define PLAYER_MAP_SPRITE_START 0
void initialize_players_map(void)
{
  set_sprite_data(PLAYER_MAP_DATA_START, train_player_logo_0_TILE_COUNT, train_player_logo_0_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 0, GET_8x16_SPRITE_TILE(0));
  set_sprite_data(PLAYER_MAP_DATA_START + 2, train_player_logo_1_TILE_COUNT, train_player_logo_1_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 1, GET_8x16_SPRITE_TILE(2));
  set_sprite_data(PLAYER_MAP_DATA_START + 4, train_player_logo_2_TILE_COUNT, train_player_logo_2_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 2, GET_8x16_SPRITE_TILE(4));
  set_sprite_data(PLAYER_MAP_DATA_START + 6, train_player_logo_3_TILE_COUNT, train_player_logo_3_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 3, GET_8x16_SPRITE_TILE(6));
}

// 4 pixels is the width of the map sprite so it can only travel 0-20
// >>> ",".join(map(str, [round(i*(24-4)/128) for i in range(128)] ))
const uint8_t car_ratio_lookup_table[128] = {
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20};

uint8_t approx_car_ratio_lookup(uint8_t x)
{
  if (x >= 128)
  {
    return car_ratio_lookup_table[127];
  }
  return car_ratio_lookup_table[x];
}

void draw_players_map(struct game_state *state)
{
  // all players logo
  for (uint8_t player = 0; player < MAX_PLAYABLES; player++)
  {
    uint8_t car_x_start = TRAIN_MAP_BG_X_START * 8 + (state->player_positions[player].car * 24);
    uint8_t car_x_ratio = approx_car_ratio_lookup(state->player_positions[player].x);
    // sprites were originally design to fit in a single 8x8 square
    // lets keep the sprites that way and adjust in code
    uint8_t shifted_y = player == 0 ? 0 : player == 1 ? 4
                                      : player == 2   ? 4
                                                      : 8;
    uint8_t shifted_x = (player == 0 || player == 2) ? 0 : -4;
    move_sprite(PLAYER_MAP_SPRITE_START + player, PLATFORM_X_ADJUST + car_x_start + car_x_ratio + shifted_x, PLATFORM_Y_ADJUST + shifted_y);
  }
}

#define TOOL_SPRITE_START 4
#define TOOL_DATA_START 8
#define LOAD_TOOL(name, num)                                                                   \
  set_sprite_data(TOOL_DATA_START + (num * 2), tool_##name##_TILE_COUNT, tool_##name##_tiles); \
  set_sprite_tile(TOOL_SPRITE_START + num, GET_8x16_SPRITE_TILE(TOOL_DATA_START + (num * 2)));

void initialize_tools(void)
{
  LOAD_TOOL(wifi, TOOL_WIFI);
  LOAD_TOOL(wrench, TOOL_WRENCH);
  LOAD_TOOL(drink, TOOL_DRINK);
  LOAD_TOOL(cat, TOOL_CAT);
  LOAD_TOOL(music, TOOL_MUSIC);
}

void draw_tools(struct game_state *state, uint8_t current_player)
{
  uint8_t current_car = state->player_positions[current_player].car;
  for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
  {
    if (!state->tools[tool].unlocked || state->tools[tool].car != current_car)
    {
      hide_sprite(TOOL_SPRITE_START + tool);
      continue;
    }
    if (state->tools[tool].player_holding != PLAYER_HOLDING_NONE)
    {
      state->tools[tool].x = state->player_positions[state->tools[tool].player_holding].x;
      state->tools[tool].y = state->player_positions[state->tools[tool].player_holding].y + 16;
    }
    move_sprite(TOOL_SPRITE_START + tool, state->tools[tool].x, PLATFORM_Y_ADJUST + state->tools[tool].y);
  }
}

#define PLAYER_DATA_START (TOOL_DATA_START + TOOL_COUNT * 2)
#define PLAYER_SPRITE_START (TOOL_SPRITE_START + TOOL_COUNT)
void initialize_players(void)
{
#ifdef NINTENDO_NES
  for (uint8_t current_player = 0; current_player < MAX_PLAYABLES; current_player++)
  {
    for (uint8_t player_of = 0; player_of < MAX_PLAYABLES; player_of++)
    {
      set_sprite_tile(PLAYER_SPRITE_START + current_player * 4 + player_of, GET_8x16_SPRITE_TILE(PLAYER_DATA_START + player_of * 2));
    }
  }
#else
  uint8_t current_player = 0;
  for (uint8_t player_of = 0; player_of < MAX_PLAYABLES; player_of++)
  {
    set_sprite_tile(PLAYER_SPRITE_START + current_player * 4 + player_of, GET_8x16_SPRITE_TILE(PLAYER_DATA_START + player_of * 2));
  }
#endif
}

#define SP_CONDUCTOR_FRAME_TILE_COUNT 2
#define PLAYER_ANIMATION_CONST(current_player, player_of)                       \
  {                                                                             \
    .sp_idx = PLAYER_SPRITE_START + current_player * MAX_PLAYABLES + player_of, \
    .data_idx = PLAYER_DATA_START + SP_CONDUCTOR_FRAME_TILE_COUNT * player_of,  \
    .frames = 3,                                                                \
    .neutral_frame = 1,                                                         \
    .data_tile_count = SP_CONDUCTOR_FRAME_TILE_COUNT,                           \
    .frame_wait = 8,                                                            \
    .frame_datas = {                                                            \
      &conductor_tiles[0 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],              \
      &conductor_tiles[1 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],              \
      &conductor_tiles[2 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],              \
    }                                                                           \
  }

// TODO: adjust this only require MAX_PLAYERS
const struct animation_const player_animations_const[MAX_PLAYABLES][MAX_PLAYABLES] = {
    {
        PLAYER_ANIMATION_CONST(0, 0),
        PLAYER_ANIMATION_CONST(0, 1),
        PLAYER_ANIMATION_CONST(0, 2),
        PLAYER_ANIMATION_CONST(0, 3),
    },
    {
        PLAYER_ANIMATION_CONST(1, 0),
        PLAYER_ANIMATION_CONST(1, 1),
        PLAYER_ANIMATION_CONST(1, 2),
        PLAYER_ANIMATION_CONST(1, 3),
    },
    {
        PLAYER_ANIMATION_CONST(2, 0),
        PLAYER_ANIMATION_CONST(2, 1),
        PLAYER_ANIMATION_CONST(2, 2),
        PLAYER_ANIMATION_CONST(2, 3),
    },
    {
        PLAYER_ANIMATION_CONST(3, 0),
        PLAYER_ANIMATION_CONST(3, 1),
        PLAYER_ANIMATION_CONST(3, 2),
        PLAYER_ANIMATION_CONST(3, 3),
    },
};

void draw_players(struct game_state *state, uint8_t current_player)
{
  for (uint8_t player = 0; player < MAX_PLAYABLES; player++)
  {
    // do not render players that are not in our car
    if (state->player_positions[player].car != state->player_positions[current_player].car)
    {
      hide_sprite(player_animations_const[current_player][player].sp_idx);
      continue;
    }
    // always move sprite to make it visible
    move_sprite(
        player_animations_const[current_player][player].sp_idx,
        PLAYER_X_ADJUST(current_player) + PLATFORM_X_ADJUST + state->player_positions[player].x,
        PLAYER_Y_ADJUST(current_player) + PLATFORM_Y_ADJUST + state->player_positions[player].y);
    // progress animation
    if (state->player_positions[player].direction < 0)
    {
      animation_progress(&player_animations_const[current_player][player], &state->player_animations[player]);
      set_sprite_prop(player_animations_const[current_player][player].sp_idx, S_FLIPX);
    }
    else if (state->player_positions[player].direction > 0)
    {
      animation_progress(&player_animations_const[current_player][player], &state->player_animations[player]);
      set_sprite_prop(player_animations_const[current_player][player].sp_idx, 0);
    }
    else
    {
      animation_idle(&player_animations_const[current_player][player], &state->player_animations[player]);
    }
  }
}

#define TASK_DATA_START (PLAYER_DATA_START + MAX_PLAYABLES * 2)
// fewer players == fewer copies of sprites
#define TASK_SPRITE_START (PLAYER_SPRITE_START + MAX_PLAYERS * MAX_PLAYABLES)
void intitialize_tasks(void)
{
  for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
  {
    uint8_t data_start = TASK_DATA_START + tool * 2;
    // TODO: swap tool sprite with task sprite
    switch (tool)
    {
    case TOOL_WIFI:
      set_sprite_data(data_start, tool_wifi_TILE_COUNT, tool_wifi_tiles);
      break;
    case TOOL_WRENCH:
      set_sprite_data(data_start, tool_wrench_TILE_COUNT, tool_wrench_tiles);
      break;
    case TOOL_DRINK:
      set_sprite_data(data_start, tool_drink_TILE_COUNT, tool_drink_tiles);
      break;
    case TOOL_CAT:
      set_sprite_data(data_start, tool_cat_TILE_COUNT, tool_cat_tiles);
      break;
    case TOOL_MUSIC:
      set_sprite_data(data_start, tool_music_TILE_COUNT, tool_music_tiles);
      break;
    }
    for (uint8_t task = 0; task < MAX_TASKS_PER_TOOL; task++)
    {
      uint8_t sprite_start = TASK_SPRITE_START + tool * MAX_TASKS_PER_TOOL + task;
      set_sprite_tile(sprite_start, GET_8x16_SPRITE_TILE(data_start));
    }
  }
}

void draw_tasks(struct game_state *state, uint8_t current_player)
{
  for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
  {
    for (uint8_t task = 0; task < MAX_TASKS_PER_TOOL; task++)
    {
      if (state->tasks[tool][task].progress > 0)
      {
        uint8_t sprite = TASK_SPRITE_START + tool * MAX_TASKS_PER_TOOL + task;
        if (state->tasks[tool][task].car != state->player_positions[current_player].car)
        {
          hide_sprite(sprite);
        }
        else
        {
          move_sprite(sprite, state->tasks[tool][task].x, PLATFORM_Y_ADJUST + state->tasks[tool][task].y);
        }
      }
    }
  }
}

#define BG_TRAIN_START 8
#define BG_START_TILE_X 0
#define BG_START_TILE_Y 2
#define BG_TILE_PLAYER_X(player) ((player == 0 || player == 2) ? 0 : 0)
#define BG_TILE_PLAYER_Y(player) (player == 0 ? 0 : player == 1 ? 7  \
                                                : player == 2   ? 14 \
                                                                : 21)
void initialize_bg_train()
{
  set_bkg_data(BG_TRAIN_START, bg_train_passenger_TILE_COUNT, bg_train_passenger_tiles);
  for (uint8_t player = 0; player < MAX_PLAYERS; player++)
  {
    uint8_t tile = BG_TRAIN_START;
    for (uint8_t t_y = 0; t_y < (bg_train_passenger_HEIGHT / 8); t_y++)
    {
      for (uint8_t t_x = 0; t_x < (bg_train_passenger_WIDTH / 8); t_x++)
      {
        set_bkg_tile_xy(
            BG_TILE_PLAYER_X(player) + BG_START_TILE_X + t_x,
            BG_TILE_PLAYER_Y(player) + BG_START_TILE_Y + t_y,
            tile);
        tile++;
      }
    }
  }
}

void npc_replace_input(struct game_state *state, uint8_t npc)
{
  // change directions
  if (state->player_positions[npc].x >= 80)
  {
    state->player_positions[npc].direction = DIRECTION_LEFT;
  }
  else if (state->player_positions[npc].x <= 40)
  {
    state->player_positions[npc].direction = DIRECTION_RIGHT;
    // state->player_positions[npc].direction = 0;
  }

  // simulate directional movement
  if (state->player_positions[npc].direction == DIRECTION_RIGHT)
  {
    joypads.joypads[npc] = J_RIGHT;
  }
  else if (state->player_positions[npc].direction == DIRECTION_LEFT)
  {
    joypads.joypads[npc] = J_LEFT;
  }
  else
  {
    joypads.joypads[npc] = 0;
  }
}

void maybe_create_tasks(struct game_state *state)
{
  if (state->open_task_count > 3)
  {
    return;
  }
  // pick random unlocked tool
  uint8_t tool = rand() % TOOL_COUNT;
  uint8_t iters = 0;
  while (!state->tools[tool].unlocked && iters < TOOL_COUNT)
  {
    tool = (tool + 1) % TOOL_COUNT;
    iters++;
  }

  for (uint8_t task = 0; task < MAX_TASKS_PER_TOOL; task++)
  {
    if (state->tasks[tool][task].progress == 0)
    {
      // create task
      uint8_t not_same_car = rand() % state->cars;
      if (not_same_car == state->tools[tool].car)
      {
        not_same_car = (not_same_car + 1) % state->cars;
      }
      state->tasks[tool][task].car = not_same_car;
      state->tasks[tool][task].x = RANDOM_X_POSITION;
      state->tasks[tool][task].y = 80 - 16;
      state->tasks[tool][task].progress = 10;
      state->open_task_count++;
      return;
    }
  }
}

BANKREF_EXTERN(train_map_0)

void main(void)
{
  // init
  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_BKG;
  fill_bkg_rect(0, 0, 24, 16, 0);
  joypad_init(MAX_PLAYABLES, &joypads);

  // train map
  SWITCH_ROM(BANK(train_map_0));
  initialize_train_map();
  // player map logo
  initialize_players_map();
  // player sprite
  initialize_players();
  // tools
  initialize_tools();
  // tasks
  intitialize_tasks();
  // bg
  initialize_bg_train();

  struct game_state state = {
      .cars = 2,
      .player_positions = {
          {.car = 0, .x = 80, .y = TRAIN_FLOOR_BASELINE, .direction = 0},
          {.car = 1, .x = RANDOM_X_POSITION, .y = TRAIN_FLOOR_BASELINE, .direction = 0},
          {.car = 1, .x = RANDOM_X_POSITION, .y = TRAIN_FLOOR_BASELINE, .direction = -1},
          {.car = 0, .x = RANDOM_X_POSITION, .y = TRAIN_FLOOR_BASELINE, .direction = 1},
      },
#define PLAYER_ANIMATION_INIT {.frame = 1, .direction = 1, .frame_tick = 0}
#define PLAYER_ANIMATIONS_INIT {PLAYER_ANIMATION_INIT, PLAYER_ANIMATION_INIT, PLAYER_ANIMATION_INIT, PLAYER_ANIMATION_INIT}
      .player_animations = PLAYER_ANIMATIONS_INIT,
      .tools = {
          // TOOL_WIFI
          {
              .unlocked = 1,
              .car = 0,
              .x = 100,
              .y = TRAIN_FLOOR_BASELINE,
              .player_holding = PLAYER_HOLDING_NONE,
          },
          // TOOL_WRENCH
          {
              .unlocked = 1,
              .car = 1,
              .x = 100,
              .y = TRAIN_FLOOR_BASELINE,
              .player_holding = PLAYER_HOLDING_NONE,
          },
      },
  };

  draw_train_map(&state);

  while (1)
  {
    // handle input
    joypad_ex(&joypads);
    for (uint8_t npc = MAX_PLAYERS; npc < MAX_PLAYABLES; npc++)
    {
      npc_replace_input(&state, npc);
    }
    for (uint8_t player = 0; player < MAX_PLAYABLES; player++)
    {
      handle_input(&state, player);
    }

    maybe_create_tasks(&state);

    // render
    vsync();
    for (uint8_t player = 0; player < MAX_PLAYERS; player++)
    {
      draw_players(&state, player);
    }
    draw_players_map(&state);
    draw_tools(&state, 0);
    draw_tasks(&state, 0);
  }
}
