#include <gb/gb.h>
#include "gen/conductor.h"

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

void main(void)
{

  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_BKG;

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
