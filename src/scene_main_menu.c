#include "types.h"
#include "input.h"
#include "font.h"
#include <stdio.h>
#include "scenes.h"
#include "gen/cursor.h"
#include "sound.h"

#define MENU_START_TILE_X 7
#define MENU_START_TILE_Y 6
#define FOCUS_MAX 2
static uint8_t focus = 0;
static const uint8_t cursor_menu_x[FOCUS_MAX] = {
    PLATFORM_X_ADJUST + 8 * (MENU_START_TILE_X - 1),
    PLATFORM_X_ADJUST + 8 * (MENU_START_TILE_X - 1),
};
static const uint8_t cursor_menu_y[FOCUS_MAX] = {
    PLATFORM_Y_ADJUST + 8 * MENU_START_TILE_Y,
    PLATFORM_Y_ADJUST + 8 * (MENU_START_TILE_Y + 1),
};
void scene_main_menu_init(void)
{
    FILL_BKG_EMPTY;
    // state
    focus = 0;
    // menu text
    font_set_bkg_data(1);
    font_print(MENU_START_TILE_X, MENU_START_TILE_Y, "PLAY");
    font_print(MENU_START_TILE_X, MENU_START_TILE_Y + 1, "TUTORIAL");
    // menu sprite
    set_sprite_data(0, cursor_TILE_COUNT, cursor_tiles);
    set_sprite_tile(0, GET_8x16_SPRITE_TILE_B0(0));
    hide_sprites_range(0, 32);
}
void scene_main_menu_loop(void)
{
    // input
    if (PRESSED(0, J_DOWN))
    {
        focus += 1;
        if (focus >= FOCUS_MAX)
        {
            focus = FOCUS_MAX - 1;
        }
        sound_on_menu_select();
    }
    else if (PRESSED(0, J_UP))
    {
        focus -= 1;
        if (focus >= FOCUS_MAX)
        {
            focus = 0;
        }
        sound_on_menu_select();
    }
    else if (PRESSED(0, J_A))
    {
        enum scene scenes_next[FOCUS_MAX] =
            {
                SCENE_DIFFICULTY_SELECT,
                SCENE_TUTORIAL,
            };
        queue_scene(scenes_next[focus]);
        sound_on_menu_confirm();
    }

    // render
    move_sprite(0, cursor_menu_x[focus], cursor_menu_y[focus]);
}
