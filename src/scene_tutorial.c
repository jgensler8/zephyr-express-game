#include "types.h"
#include "scenes.h"
#include "input.h"
#include <gbdk/font.h>

#define FONT_LOAD_TILES 186
#define ALPHABET_CHARS 26
uint8_t unpacked_font[ALPHABET_CHARS * 4 * 4];

void print(uint8_t x, uint8_t y, char *str)
{
    uint8_t *next = str;
    while (*next != 0)
    {
        uint8_t offset = *next - 'A';
        uint8_t tile = FONT_LOAD_TILES + offset;
        if (offset >= ALPHABET_CHARS)
        {
            tile = 0;
        }
        set_bkg_tile_xy(x, y, tile);
        next++;
        x++;
    }
}

uint8_t tutorial_step;
uint8_t tutorial_step_init;

void tutorial_init(void)
{
    // copy font tiles
    font_init();
    font_load(font_min);
    get_bkg_data(11, ALPHABET_CHARS, unpacked_font);
    set_bkg_data(FONT_LOAD_TILES, ALPHABET_CHARS, unpacked_font);

    // set state
    tutorial_step = 0;
    tutorial_step_init = 0;
    state = default_state();
    state.cars = 1;
    state.open_task_count = 1;
    state.max_open_tasks = 1;
    for (uint8_t tool = 1; tool < TOOL_COUNT; tool++)
    {
        state.tools[tool].unlocked = 0;
    }
    for (uint8_t task = 1; task < TASK_SLOTS_PER_CAR; task++)
    {
        state.tasks[0][task].progress = 0;
    }

    scene_gameplay.init();
}

void tutorial_loop(void)
{
    // init step
    if (tutorial_step_init == 0)
    {
        fill_bkg_rect(0, 12, 20, 6, 0);
        switch (tutorial_step)
        {
        case 0:
            print(1, 12, "WELCOME TO THE");
            print(2, 13, "TUTORIAL");
            print(1, 15, "TRY TO MOVE");
            print(2, 16, "LEFT AND RIGHT");
            break;
        case 1:
            print(1, 12, "GREAT");
            print(1, 13, "MOVE TO THE TOOL");
            print(2, 14, "ON THE RIGHT");
            print(1, 15, "PRESS A TO");
            print(2, 16, "PICK IT UP");
            break;
        case 2:
            print(1, 12, "WONDERFUL");
            print(1, 13, "WALK TO THE TASK");
            print(1, 15, "TRY TO MATCH THE");
            print(2, 16, "SYMBOL OF THE TOOL");
            break;
        case 3:
            print(1, 12, "AWESOME");
            print(1, 13, "COMPLETE TWO");
            print(2, 14, "MORE TASKS");
            break;
        case 4:
            print(1, 12, "CONGRATULATIONS");
            print(1, 13, "YOUVE COMPLETED THE");
            print(2, 14, "TUTORIAL");
            print(1, 16, "PRESS A TO LEAVE");
            break;
        }
        tutorial_step_init = 1;
    }
    else
    {
        switch (tutorial_step)
        {
        case 0:
            if (PRESSED(0, J_RIGHT) || PRESSED(0, J_LEFT))
            {
                tutorial_step++;
                tutorial_step_init = 0;
            }
            break;
        case 1:
            if (state.tools[0].player_holding == 0)
            {
                tutorial_step++;
                tutorial_step_init = 0;
            }
            break;
        case 2:
            if (state.round_score > 0)
            {
                tutorial_step++;
                tutorial_step_init = 0;
            }
            break;
        case 3:
            if (state.round_score > 3)
            {
                tutorial_step++;
                tutorial_step_init = 0;
            }
        case 4:
            if (PRESSED(0, J_A))
            {
                queue_scene(&scene_main_menu);
            }
            break;
        }
    }

    scene_gameplay.loop();
}

struct scene scene_tutorial = {
    .init = tutorial_init,
    .loop = tutorial_loop,
};