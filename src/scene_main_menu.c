#include "types.h"
#include "input.h"
#include <gbdk/font.h>
#include <stdio.h>
#include "scenes.h"
#include "gen/cursor.h"

#define MENU_START_TILE_X 7
#define MENU_START_TILE_Y 6
#define FOCUS_MAX 2
uint8_t focus = 0;
uint8_t menu_x[FOCUS_MAX] = {8 * MENU_START_TILE_X - 4, 8 * MENU_START_TILE_X - 4};
uint8_t menu_y[FOCUS_MAX] = {PLATFORM_Y_ADJUST + 8 * MENU_START_TILE_Y, PLATFORM_Y_ADJUST + 8 * (MENU_START_TILE_Y + 1)};
void main_menu_init(void)
{
    // state
    focus = 0;
    // menu text
    font_init();
    font_load(font_min);
    printf("\n\n\n\n\n\n");
    printf("       Play\n");
    printf("       Tutorial\n");
    // menu sprite
    set_sprite_data(0, cursor_TILE_COUNT, cursor_tiles);
    set_sprite_tile(0, GET_8x16_SPRITE_TILE(0));
}
void main_menu_loop(void)
{
    // input
    if (PRESSED(0, J_DOWN))
    {
        focus += 1;
        if (focus >= FOCUS_MAX)
        {
            focus = FOCUS_MAX - 1;
        }
    }
    else if (PRESSED(0, J_UP))
    {
        focus -= 1;
        if (focus >= FOCUS_MAX)
        {
            focus = 0;
        }
    }
    else if (PRESSED(0, J_A))
    {
        struct scene *scenes_next[FOCUS_MAX] =
            {
                &scene_gameplay,
                &scene_tutorial,
            };
        queue_scene(scenes_next[focus]);
    }

    // render
    move_sprite(0, menu_x[focus], menu_y[focus]);
}

struct scene scene_main_menu = {
    .init = main_menu_init,
    .loop = main_menu_loop,
};
