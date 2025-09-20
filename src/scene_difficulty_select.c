#include "types.h"
#include "font.h"
#include "input.h"
#include "scenes.h"
#include "sound.h"

#define MENU_START_TILE_X 4
#define MENU_START_TILE_Y 6
#define FOCUS_MAX 3
static uint8_t focus = 0;
static uint8_t menu_x[FOCUS_MAX] = {
    8 * MENU_START_TILE_X - 4,
    8 * MENU_START_TILE_X - 4,
    8 * MENU_START_TILE_X - 4,
};
static uint8_t menu_y[FOCUS_MAX] = {
    PLATFORM_Y_ADJUST + 8 * MENU_START_TILE_Y,
    PLATFORM_Y_ADJUST + 8 * (MENU_START_TILE_Y + 1),
    PLATFORM_Y_ADJUST + 8 * (MENU_START_TILE_Y + 2),
};
uint8_t last_focus;
void difficulty_select_init(void)
{
    FILL_BKG_EMPTY;
    // font
    font_set_bkg_data(1);
    font_print(4, 4, "DIFFICULTY");
    font_print(4, 6, "CASUAL");
    font_print(4, 7, "EASY");
    font_print(4, 8, "HARD");
    // cursor (this is initiailzed in scene_main_menu)
    last_focus = 1;
    focus = 0;
    move_sprite(0, menu_x[0], menu_y[0]);
}
void difficulty_select_loop(void)
{
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
        init_state(focus);
        queue_scene(&scene_gameplay);
        sound_on_menu_confirm();
    }
    else if (PRESSED(0, J_B))
    {
        queue_scene(&scene_main_menu);
        sound_on_menu_back();
    }

    // render
    if (focus != last_focus)
    {
        fill_bkg_rect(0, 10, 20, 10, 0);
        font_print(1, 10, "COLLISION");
        font_print(1, 12, "ANGRY CUSTOMERS");
        font_print(1, 14, "ROUND UNLOCKS");
        switch (focus)
        {
        case DIFFICULTY_CASUAL:
#if NINTENDO_NES
            font_print(2, 11, "NONE");
#else
            font_print(2, 11, "NOT SUPPORTED");
#endif
            font_print(2, 13, "NONE");
            font_print(2, 15, "TWO");
            break;
        case DIFFICULTY_EASY:
#if NINTENDO_NES
            font_print(2, 11, "ENABLED");
#else
            font_print(2, 11, "NOT SUPPORTED");
#endif
            font_print(2, 13, "FEW");
            font_print(2, 15, "TWO");
            break;
        case DIFFICULTY_HARD:
#if NINTENDO_NES
            font_print(2, 11, "ENABLED");
#else
            font_print(2, 11, "NOT SUPPORTED");
#endif
            font_print(2, 13, "MANY");
            font_print(2, 15, "ONE");
            break;
        }
    }
    last_focus = focus;
    move_sprite(0, menu_x[focus], menu_y[focus]);
}

struct scene scene_difficulty_select = {
    .init = difficulty_select_init,
    .loop = difficulty_select_loop,
};
