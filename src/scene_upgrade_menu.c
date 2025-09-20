#include "types.h"
#include "scenes.h"
#include "font.h"
#include "input.h"
#include <gbdk/metasprites.h>
#include "gen/cursor.h"

enum upgrade_focus
{
    FOCUS_MAIN,
    FOCUS_UPGRADE_SELECT,
    FOCUS_UPGRADE_CONFIRM,
};

static enum upgrade_focus last_focus;
static enum upgrade_focus focus;
static uint8_t cursor_focus;
static uint8_t upgrade_cursor_focus;
uint8_t main_menu_x[] = {PLATFORM_X_ADJUST + 8, PLATFORM_X_ADJUST + 8};
uint8_t main_menu_y[] = {PLATFORM_Y_ADJUST + 8 * 5, PLATFORM_Y_ADJUST + 8 * 6};
#define NUM_UPGRADES 9
uint8_t upgrade_select_menu_x[NUM_UPGRADES] = {
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
    PLATFORM_X_ADJUST + 8,
};
uint8_t upgrade_select_menu_y[NUM_UPGRADES] = {
    PLATFORM_Y_ADJUST + 8 * 2,
    PLATFORM_Y_ADJUST + 8 * 3,
    PLATFORM_Y_ADJUST + 8 * 4,
    PLATFORM_Y_ADJUST + 8 * 5,
    PLATFORM_Y_ADJUST + 8 * 6,
    PLATFORM_Y_ADJUST + 8 * 7,
    PLATFORM_Y_ADJUST + 8 * 8,
    PLATFORM_Y_ADJUST + 8 * 9,
    PLATFORM_Y_ADJUST + 8 * 10,
};
uint8_t upgrade_confirm_menu_x[] = {PLATFORM_X_ADJUST + 8, PLATFORM_X_ADJUST + 8};
uint8_t upgrade_confirm_menu_y[] = {PLATFORM_Y_ADJUST + 8 * 2, PLATFORM_Y_ADJUST + 8 * 3};
static uint8_t *menu_x;
static uint8_t *menu_y;
void scene_upgrade_menu_init(void)
{
    hide_sprites_range(0, MAX_HARDWARE_SPRITES);
    font_set_bkg_data(1);
    font_set_bkg_data_numeric(28);

    focus = FOCUS_MAIN;
    last_focus = !FOCUS_MAIN;
    state.unlocks_left += 1;

    set_sprite_data(0, cursor_TILE_COUNT, cursor_tiles);
    set_sprite_tile(0, GET_8x16_SPRITE_TILE(0));
}

void handle_input_up_down(uint8_t max)
{
    if (PRESSED(0, J_DOWN))
    {
        cursor_focus += 1;
        if (cursor_focus >= max)
        {
            cursor_focus = max - 1;
        }
    }
    else if (PRESSED(0, J_UP))
    {
        cursor_focus -= 1;
        if (cursor_focus >= max)
        {
            cursor_focus = 0;
        }
    }
}
// 1 meets criteria
// 0 does not meet criteria
uint8_t can_upgrade(uint8_t cursor_focus)
{
    switch (cursor_focus)
    {
    case 0:
        return state.cars <= MAX_CARS - 1;
    case 1:
        return state.cars >= 2 && state.tools[TOOL_WRENCH].unlocked == 0;
    case 2:
        return state.cars >= 2 && state.tools[TOOL_DRINK].unlocked == 0;
    case 3:
        return state.cars >= 3 && state.tools[TOOL_CAT].unlocked == 0;
    case 4:
        return state.cars >= 3 && state.tools[TOOL_MUSIC].unlocked == 0;
    case 5:
        return state.run_speed == 3 && state.walk_speed < 2;
    case 6:
        return state.run_ticks > 6;
    case 7:
        return state.run_speed < 3;
    case 8:
        return state.task_speed < 3;
    default:
        break;
    }
    return 0;
}
void do_upgrade(uint8_t cursor_focus)
{
    switch (cursor_focus)
    {
    case 0:
        state.cars += 1;
        break;
    case 1:
        state.tools[TOOL_WRENCH].unlocked = 1;
        break;
    case 2:
        state.tools[TOOL_DRINK].unlocked = 1;
        break;
    case 3:
        state.tools[TOOL_CAT].unlocked = 1;
        break;
    case 4:
        state.tools[TOOL_MUSIC].unlocked = 1;
        break;
    case 5:
        state.walk_speed += 1;
        break;
    case 6:
        state.run_ticks -= 2;
        break;
    case 7:
        state.run_speed += 1;
        break;
    case 8:
        state.task_speed += 1;
        break;
    default:
        break;
    }
}
void scene_upgrade_menu_loop(void)
{
    // focus change
    if (last_focus != focus)
    {
        FILL_BKG_EMPTY;
        cursor_focus = 0;
        switch (focus)
        {
        case FOCUS_MAIN:
            menu_x = main_menu_x;
            menu_y = main_menu_y;
            font_print(1, 1, "ROUND OVER");
            font_print(1, 2, "SCORE");
            font_print_numeric(10, 2, state.round_score);
            font_print(2, 3, "GOOD");
            if (state.unlocks_left > 0)
            {
                font_print(2, 5, "UPGRADES");
            }
            else
            {
                font_print(2, 5, "NO UNLOCKS LEFT");
            }
            font_print(2, 6, "NEXT ROUND");
            break;
        case FOCUS_UPGRADE_SELECT:
            menu_x = upgrade_select_menu_x;
            menu_y = upgrade_select_menu_y;
            font_print(1, 1, "UNLOCKS LEFT");
            font_print_numeric(16, 1, state.unlocks_left);
            // 0
            if (can_upgrade(0))
            {
                font_print(2, 2, "ADD CAR");
            }
            else
            {
                font_print(2, 2, "ALL CARS PURCHASED");
            }
            // 1
            if (state.cars < 2)
            {
                font_print(2, 3, "NEED 2 CARS");
                font_print_numeric(7, 3, 2);
            }
            else if (can_upgrade(1))
            {
                font_print(2, 3, "UNLOCK WRENCH");
            }
            else
            {
                font_print(2, 3, "WRENCH UNLOCKED");
            }
            // 2
            if (state.cars < 2)
            {
                font_print(2, 4, "NEED 2 CARS");
                font_print_numeric(7, 4, 2);
            }
            else if (can_upgrade(2))
            {
                font_print(2, 4, "UNLOCK DRINK");
            }
            else
            {
                font_print(2, 4, "DRINK  UNLOCKED");
            }
            // 3
            if (state.cars < 3)
            {
                font_print(2, 5, "NEED 3 CARS");
                font_print_numeric(7, 5, 3);
            }
            else if (can_upgrade(3))
            {
                font_print(2, 5, "UNLOCK CAT");
            }
            else
            {
                font_print(2, 5, "CAT    UNLOCKED");
            }
            // 4
            if (state.cars < 3)
            {
                font_print(2, 6, "NEED 3 CARS");
                font_print_numeric(7, 6, 3);
            }
            else if (can_upgrade(4))
            {
                font_print(2, 6, "UNLOCK MUSIC");
            }
            else
            {
                font_print(2, 6, "MUSIC  UNLOCKED");
            }
            // 5
            if (state.run_speed < 3)
            {
                font_print(2, 7, "NEED MAX RUN SPEED");
            }
            else
            {
                font_print(2, 7, "WALK SPEED UP");
            }
            // 6
            font_print(2, 8, "RUN SOONER");
            // 7
            font_print(2, 9, "RUN SPEED UP");
            // 8
            font_print(2, 10, "TASK SPEED UP");
            break;
        case FOCUS_UPGRADE_CONFIRM:
            menu_x = upgrade_confirm_menu_x;
            menu_y = upgrade_confirm_menu_y;
            font_print(1, 1, "CONFIRM");
            font_print(2, 2, "NO");
            font_print(2, 3, "YES");
            break;
        }
        last_focus = focus;
    }
    // input
    switch (focus)
    {
    case FOCUS_MAIN:
        if (PRESSED(0, J_A))
        {
            switch (cursor_focus)
            {
            case 0:
                if (state.unlocks_left > 0)
                {
                    focus = FOCUS_UPGRADE_SELECT;
                }
                break;
            case 1:
                advance_state();
                queue_scene(&scene_gameplay);
                return;
            }
        }
        else
        {
            handle_input_up_down(2);
        }
        break;
    case FOCUS_UPGRADE_SELECT:
        if (PRESSED(0, J_B))
        {
            focus = FOCUS_MAIN;
        }
        else if (PRESSED(0, J_A))
        {
            if (can_upgrade(cursor_focus))
            {
                upgrade_cursor_focus = cursor_focus;
                focus = FOCUS_UPGRADE_CONFIRM;
            }
        }
        else
        {
            handle_input_up_down(NUM_UPGRADES);
        }
        break;
    case FOCUS_UPGRADE_CONFIRM:
        if (PRESSED(0, J_B))
        {
            focus = FOCUS_UPGRADE_SELECT;
        }
        else if (PRESSED(0, J_A))
        {
            do_upgrade(upgrade_cursor_focus);
            state.unlocks_left -= 1;
            focus = FOCUS_MAIN;
        }
        else
        {
            handle_input_up_down(2);
        }
        break;
    }

    // render
    move_sprite(0, menu_x[cursor_focus], menu_y[cursor_focus]);
}

struct scene scene_upgrade_menu = {
    .init = scene_upgrade_menu_init,
    .loop = scene_upgrade_menu_loop,
};