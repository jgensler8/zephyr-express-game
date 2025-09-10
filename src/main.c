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

#ifdef NINTENDO_NES
#define PLATFORM_Y_ADJUST 0
#else
#define PLATFORM_Y_ADJUST 16
#endif

#ifdef NINTENDO_NES
#define GET_8x16_SPRITE_TILE(tile) (tile + 1)
#else
#define GET_8x16_SPRITE_TILE(tile) (tile)
#endif

struct animation_const
{
  uint8_t sp_idx;
  uint8_t data_idx;
  uint8_t frames;
  uint8_t neutral_frame;
  uint8_t data_tile_count;
  uint8_t *frame_datas[3];
  uint8_t frame_wait;
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
};

struct tool
{
  uint8_t unlocked;
  uint8_t car;
  uint8_t x;
  uint8_t y;
  uint8_t player_holding;
};

#define MAX_PLAYERS 4
struct game_state
{
  uint8_t cars;
  struct player_position player_positions[MAX_PLAYERS];
  struct animation_state player_animations[MAX_PLAYERS];
  struct tool tools[TOOL_COUNT];
};

void handle_input(struct game_state *state, uint8_t player)
{
  uint8_t joy = joypad();
  if (joy & J_LEFT)
  {
    if (state->player_positions[player].x > 8)
    {
      state->player_positions[player].x -= 1;
    }
    state->player_positions[player].direction = -1;
  }
  else if (joy & J_RIGHT)
  {
    if (state->player_positions[player].x < 160)
    {
      state->player_positions[player].x += 1;
    }
    state->player_positions[player].direction = 1;
  }
  else
  {
    state->player_positions[player].direction = 0;
  }
  if (state->player_positions[player].x <= 20)
  {

    if (state->player_positions[player].car < state->cars - 1)
    {
      state->player_positions[player].car += 1;
      state->player_positions[player].x = 160;
    }
  }
  else if (state->player_positions[player].x >= 160)
  {
    if (state->player_positions[player].car > 0)
    {
      state->player_positions[player].car -= 1;
      state->player_positions[player].x = 20;
    }
  }
}

#define PLAYER_SPRITE_START 0
#define SP_CONDUCTOR_FRAME_TILE_COUNT 2
#define PLAYER_ANIMATION_CONST(player)                             \
  {                                                                \
    .sp_idx = PLAYER_SPRITE_START + player,                        \
    .data_idx = 0 + SP_CONDUCTOR_FRAME_TILE_COUNT * player,        \
    .frames = 3,                                                   \
    .neutral_frame = 1,                                            \
    .data_tile_count = SP_CONDUCTOR_FRAME_TILE_COUNT,              \
    .frame_wait = 8,                                               \
    .frame_datas = {                                               \
      &conductor_tiles[0 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4], \
      &conductor_tiles[1 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4], \
      &conductor_tiles[2 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4], \
    }                                                              \
  }

const struct animation_const player_animations_const[MAX_PLAYERS] = {
    PLAYER_ANIMATION_CONST(0),
    PLAYER_ANIMATION_CONST(1),
    PLAYER_ANIMATION_CONST(2),
    PLAYER_ANIMATION_CONST(3),
};

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

#define TRAIN_MAP_CAR_TILE_START 1
void draw_train_map(struct game_state *state)
{
  // draw cars right to left
  for (uint8_t cars = 0; cars < state->cars; cars++)
  {
    set_bkg_tile_xy(17 - (cars * 3), 0, TRAIN_MAP_CAR_TILE_START);
    set_bkg_tile_xy(18 - (cars * 3), 0, TRAIN_MAP_CAR_TILE_START + 2);
    set_bkg_tile_xy(19 - (cars * 3), 0, TRAIN_MAP_CAR_TILE_START + 4);
    set_bkg_tile_xy(17 - (cars * 3), 1, TRAIN_MAP_CAR_TILE_START + 1);
    set_bkg_tile_xy(18 - (cars * 3), 1, TRAIN_MAP_CAR_TILE_START + 3);
    set_bkg_tile_xy(19 - (cars * 3), 1, TRAIN_MAP_CAR_TILE_START + 5);
  }
}

#define PLAYER_MAP_SPRITE_START 8
void initialize_players_map(void)
{
  set_sprite_data(10, train_player_logo_0_TILE_COUNT, train_player_logo_0_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 0, GET_8x16_SPRITE_TILE(10));
  set_sprite_data(12, train_player_logo_1_TILE_COUNT, train_player_logo_1_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 1, GET_8x16_SPRITE_TILE(12));
  set_sprite_data(14, train_player_logo_2_TILE_COUNT, train_player_logo_2_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 2, GET_8x16_SPRITE_TILE(14));
  set_sprite_data(16, train_player_logo_3_TILE_COUNT, train_player_logo_3_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 3, GET_8x16_SPRITE_TILE(16));
}

void draw_players_map(struct game_state *state)
{
  // all players logo
  for (uint8_t player = 0; player < MAX_PLAYERS; player++)
  {
    move_sprite(PLAYER_MAP_SPRITE_START + player, 160 - (state->player_positions[player].car * 3 * 8), PLATFORM_Y_ADJUST + 16);
  }
}

void initialize_players(void)
{
  set_sprite_tile(PLAYER_SPRITE_START, GET_8x16_SPRITE_TILE(0));
  set_sprite_tile(PLAYER_SPRITE_START + 1, GET_8x16_SPRITE_TILE(0));
  set_sprite_tile(PLAYER_SPRITE_START + 2, GET_8x16_SPRITE_TILE(0));
  set_sprite_tile(PLAYER_SPRITE_START + 3, GET_8x16_SPRITE_TILE(0));
}

void draw_players(struct game_state *state, uint8_t current_player)
{
  for (uint8_t player = 0; player < MAX_PLAYERS; player++)
  {
    // do not render players that are not in our car
    if (player != current_player && state->player_positions[player].car != state->player_positions[current_player].car)
    {
      hide_sprite(player_animations_const[player].sp_idx);
      continue;
    }
    // progress animation
    if (state->player_positions[player].direction < 0)
    {
      animation_progress(&player_animations_const[player], &state->player_animations[player]);
      move_sprite(player_animations_const[player].sp_idx, state->player_positions[player].x, PLATFORM_Y_ADJUST + state->player_positions[player].y);
      set_sprite_prop(player_animations_const[player].sp_idx, S_FLIPX);
    }
    else if (state->player_positions[player].direction > 0)
    {
      animation_progress(&player_animations_const[player], &state->player_animations[player]);
      move_sprite(player_animations_const[player].sp_idx, state->player_positions[player].x, PLATFORM_Y_ADJUST + state->player_positions[player].y);
      set_sprite_prop(player_animations_const[player].sp_idx, 0);
    }
    else
    {
      animation_idle(&player_animations_const[player], &state->player_animations[player]);
    }
  }
}

#define TOOL_SPRITE_START 12
#define TOOL_DATA_START 18
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
    move_sprite(TOOL_SPRITE_START + tool, state->tools[tool].x, PLATFORM_Y_ADJUST + state->tools[tool].y);
  }
}

void npc_action(struct game_state *state, uint8_t npc)
{
  // move npc
  if (state->player_positions[npc].direction == 1)
  {
    state->player_positions[npc].x += 1;
  }
  else if (state->player_positions[npc].direction == -1)
  {
    state->player_positions[npc].x -= 1;
  }

  // change directions
  if (state->player_positions[npc].x >= 140)
  {
    state->player_positions[npc].direction = -1;
  }
  else if (state->player_positions[npc].x <= 30)
  {
    state->player_positions[npc].direction = 1;
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

  // train map
  SWITCH_ROM(BANK(train_map_0));
  set_bkg_data(TRAIN_MAP_CAR_TILE_START, train_map_0_TILE_COUNT, train_map_0_tiles);
  // player map logo
  initialize_players_map();
  // player sprite
  initialize_players();
  // tools
  initialize_tools();

#define RANDOM_START_POSITION (30 + (rand() % 100))
#define RANDOM_DIRECTION (rand() < 128 ? -1 : 1)
  struct game_state state = {
      .cars = 2,
      .player_positions = {
          {.car = 0, .x = 80, .y = 80, .direction = 0},
          {.car = 1, .x = RANDOM_START_POSITION, .y = 80, .direction = RANDOM_DIRECTION},
          {.car = 1, .x = RANDOM_START_POSITION, .y = 80, .direction = RANDOM_DIRECTION},
          {.car = 1, .x = RANDOM_START_POSITION, .y = 80, .direction = RANDOM_DIRECTION},
      },
      .player_animations = {
          {.frame = 1, .direction = 1, .frame_tick = 0},
          {.frame = 1, .direction = 1, .frame_tick = 0},
          {.frame = 1, .direction = 1, .frame_tick = 0},
          {.frame = 1, .direction = 1, .frame_tick = 0},
      },
      .tools = {
          // TOOL_WIFI
          {
              .unlocked = 1,
              .car = 0,
              .x = 100,
              .y = 80,
              .player_holding = 0,
          },
          // TOOL_WRENCH
          {
              .unlocked = 1,
              .car = 1,
              .x = 100,
              .y = 80,
              .player_holding = 0,
          },
      },
  };

  draw_train_map(&state);

  while (1)
  {
    // handle input
    handle_input(&state, 0);

    // npc input
    npc_action(&state, 1);
    npc_action(&state, 2);
    npc_action(&state, 3);

    // render
    vsync();
    draw_players(&state, 0);
    draw_players_map(&state);
    draw_tools(&state, 0);
  }
}
