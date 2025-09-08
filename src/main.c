#include <gb/gb.h>
#include <rand.h>
#include "gen/conductor.h"
#include "gen/train_map_0.h"
#include "gen/train_player_logo_arrow.h"
#include "gen/train_player_logo_0.h"
#include "gen/train_player_logo_1.h"
#include "gen/train_player_logo_2.h"
#include "gen/train_player_logo_3.h"

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

#define MAX_PLAYERS 4
struct game_state
{
  uint8_t current_player;
  uint8_t cars;
  struct player_position player_positions[MAX_PLAYERS];
  struct animation_state player_animations[MAX_PLAYERS];
};

#define SP_CONDUCTOR_FRAME_TILE_COUNT 2
#define PLAYER_ANIMATION_CONST(player)                             \
  {                                                                \
    .sp_idx = 0 + player,                                          \
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

#define PLAYER_ARROW_SPRITE_START 8
#define PLAYER_MAP_SPRITE_START 9
void draw_players_map(struct game_state *state)
{
  // current player arrow
  move_sprite(PLAYER_ARROW_SPRITE_START, state->player_positions[state->current_player].x, 0);
  // all players logo
  for (uint8_t player = 0; player < MAX_PLAYERS; player++)
  {
    move_sprite(PLAYER_MAP_SPRITE_START + player, 160 - (state->player_positions[player].car * 3 * 8), 16 + 16);
  }
}

void draw_players(struct game_state *state)
{
  for (uint8_t player = 0; player < MAX_PLAYERS; player++)
  {
    // do not render players that are not in our car
    if (player != state->current_player && state->player_positions[player].car != state->player_positions[state->current_player].car)
    {
      hide_sprite(player_animations_const[player].sp_idx);
      continue;
    }
    // progress animation
    if (state->player_positions[player].direction < 0)
    {
      animation_progress(&player_animations_const[player], &state->player_animations[player]);
      move_sprite(player_animations_const[player].sp_idx, state->player_positions[player].x, state->player_positions[player].y);
      set_sprite_prop(player_animations_const[player].sp_idx, S_FLIPX);
    }
    else if (state->player_positions[player].direction > 0)
    {
      animation_progress(&player_animations_const[player], &state->player_animations[player]);
      move_sprite(player_animations_const[player].sp_idx, state->player_positions[player].x, state->player_positions[player].y);
      set_sprite_prop(player_animations_const[player].sp_idx, 0);
    }
    else
    {
      animation_idle(&player_animations_const[player], &state->player_animations[player]);
    }
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

void main(void)
{
  // init
  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_BKG;
  fill_bkg_rect(0, 0, 24, 16, 0);

  // bkg
  set_bkg_data(TRAIN_MAP_CAR_TILE_START, train_map_0_TILE_COUNT, train_map_0_tiles);

  // current player arrow
  set_sprite_data(8, train_player_logo_arrow_TILE_COUNT, train_player_logo_arrow_tiles);
  set_sprite_tile(PLAYER_ARROW_SPRITE_START, 8);
  // all player logos
  set_sprite_data(10, train_player_logo_0_TILE_COUNT, train_player_logo_0_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 0, 10);
  set_sprite_data(12, train_player_logo_1_TILE_COUNT, train_player_logo_1_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 1, 12);
  set_sprite_data(14, train_player_logo_2_TILE_COUNT, train_player_logo_2_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 2, 14);
  set_sprite_data(16, train_player_logo_3_TILE_COUNT, train_player_logo_3_tiles);
  set_sprite_tile(PLAYER_MAP_SPRITE_START + 3, 16);

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
      }};

  draw_train_map(&state);

  while (1)
  {
    wait_vbl_done();

    // handle input
    uint8_t joy = joypad();
    if (joy & J_LEFT)
    {
      if (state.player_positions[state.current_player].x > 8)
      {
        state.player_positions[state.current_player].x -= 1;
      }
      state.player_positions[state.current_player].direction = -1;
    }
    else if (joy & J_RIGHT)
    {
      if (state.player_positions[state.current_player].x < 160)
      {
        state.player_positions[state.current_player].x += 1;
      }
      state.player_positions[state.current_player].direction = 1;
    }
    else
    {
      state.player_positions[state.current_player].direction = 0;
    }
    if (state.player_positions[state.current_player].x <= 20)
    {

      if (state.player_positions[state.current_player].car < state.cars - 1)
      {
        state.player_positions[state.current_player].car += 1;
        state.player_positions[state.current_player].x = 160;
      }
    }
    else if (state.player_positions[state.current_player].x >= 160)
    {
      if (state.player_positions[state.current_player].car > 0)
      {
        state.player_positions[state.current_player].car -= 1;
        state.player_positions[state.current_player].x = 20;
      }
    }

    // npc input
    npc_action(&state, 1);
    npc_action(&state, 2);
    npc_action(&state, 3);

    // render
    draw_players(&state);
    draw_players_map(&state);
  }
}
