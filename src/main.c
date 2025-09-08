#include <gb/gb.h>
#include "gen/conductor.h"
#include "gen/train_map_0.h"
#include "gen/train_player_logo.h"

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
};

struct game_state
{
  uint8_t cars;
  struct player_position player_positions[4];
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

#define PLAYER_MAP_TILE_START 7
void draw_players_map(struct game_state *state)
{
  for (uint8_t player = 0; player < 1; player++)
  {
    set_bkg_tile_xy(17 - (state->player_positions[player].car * 3), 2, PLAYER_MAP_TILE_START);
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
  set_bkg_data(PLAYER_MAP_TILE_START, train_player_logo_TILE_COUNT, train_player_logo_tiles);

  // conductor sprite
#define SP_CONDUCTOR_FRAME_TILE_COUNT 2
  const struct animation_const sp_conductor_const = {
      .sp_idx = 0,
      .data_idx = 0,
      .frames = 3,
      .neutral_frame = 1,
      .data_tile_count = SP_CONDUCTOR_FRAME_TILE_COUNT,
      .frame_wait = 10,
      .frame_datas = {
          &conductor_tiles[0 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],
          &conductor_tiles[1 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],
          &conductor_tiles[2 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],
      },
  };
  struct animation_state sp_conductor_state = {
      .frame = 1,
      .direction = 1,
      .frame_tick = 0,
  };
  animation_idle(&sp_conductor_const, &sp_conductor_state);
  set_sprite_tile(sp_conductor_const.sp_idx, sp_conductor_const.data_idx);
  move_sprite(sp_conductor_const.sp_idx, 80, 80);

  struct game_state state = {
      .cars = 2,
      .player_positions = {
          {.car = 0, .x = 0, .y = 0},
          {.car = 0, .x = 0, .y = 0},
          {.car = 0, .x = 0, .y = 0},
          {.car = 0, .x = 0, .y = 0}},
  };

  draw_train_map(&state);
  draw_players_map(&state);

  while (1)
  {
    wait_vbl_done();
    uint8_t joy = joypad();
    if (joy & J_LEFT)
    {
      scroll_sprite(0, -1, 0);
      set_sprite_prop(0, S_FLIPX);
      animation_progress(&sp_conductor_const, &sp_conductor_state);
    }
    else if (joy & J_RIGHT)
    {
      scroll_sprite(0, 1, 0);
      set_sprite_prop(0, 0);
      animation_progress(&sp_conductor_const, &sp_conductor_state);
    }
    else
    {
      animation_idle(&sp_conductor_const, &sp_conductor_state);
    }
  }
}
