#include "types.h"
#include "scenes.h"
#include "input.h"
#include "font.h"

uint8_t tutorial_step;
uint8_t tutorial_step_init;

void scene_tutorial_init(void)
{
    // copy font tiles
    font_set_bkg_data(FONT_GAMEPLAY_SAFE_TILE);

    // set state
    tutorial_step = 0;
    tutorial_step_init = 0;
    state = default_state;
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

    scene_gameplay_init();
}

void scene_tutorial_loop(void)
{
    // init step
    if (tutorial_step_init == 0)
    {
        fill_bkg_rect(0, 12, 20, 6, 0);
        switch (tutorial_step)
        {
        case 0:
            font_print(1, 12, "WELCOME TO THE");
            font_print(2, 13, "TUTORIAL");
            font_print(1, 15, "TRY TO MOVE");
            font_print(2, 16, "LEFT AND RIGHT");
            break;
        case 1:
            font_print(1, 12, "GREAT");
            font_print(1, 13, "MOVE TO THE TOOL");
            font_print(2, 14, "ON THE RIGHT");
            font_print(1, 15, "PRESS A TO");
            font_print(2, 16, "PICK IT UP");
            break;
        case 2:
            font_print(1, 12, "WONDERFUL");
            font_print(1, 13, "WALK TO THE TASK");
            font_print(1, 15, "TRY TO MATCH THE");
            font_print(2, 16, "SYMBOL OF THE TOOL");
            break;
        case 3:
            font_print(1, 12, "AWESOME");
            font_print(1, 13, "COMPLETE THREE");
            font_print(2, 14, "MORE TASKS");
            break;
        case 4:
            font_print(1, 12, "CONGRATULATIONS");
            font_print(1, 13, "YOUVE COMPLETED THE");
            font_print(2, 14, "TUTORIAL");
            font_print(1, 16, "PRESS A TO LEAVE");
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
                queue_scene(SCENE_MAIN_MENU);
            }
            break;
        }
    }

    scene_gameplay_loop();
}
