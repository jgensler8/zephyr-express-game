#include "types.h"
#include "scenes.h"
#include "input.h"
#include "font.h"

uint8_t tutorial_step;
uint8_t tutorial_step_init;

#ifdef NINTENDO_NES
#define TUTORIAL_TEXT_Y 18
#else
#define TUTORIAL_TEXT_Y 11
#endif

void scene_tutorial_init(void)
{
    // copy font tiles
    font_set_bkg_data(FONT_GAMEPLAY_SAFE_TILE);

    // set state
    tutorial_step = 0;
    tutorial_step_init = 0;
    init_state(0);
    state.tutorial = 1;

    scene_gameplay_init();
}

void scene_tutorial_loop(void)
{
    // init step
    if (tutorial_step_init == 0)
    {
        fill_bkg_rect(0, TUTORIAL_TEXT_Y, 20, 7, 0);
        switch (tutorial_step)
        {
        case 0:
            font_print(1, TUTORIAL_TEXT_Y, "WELCOME TO THE");
            font_print(2, TUTORIAL_TEXT_Y + 1, "TUTORIAL");
            font_print(1, TUTORIAL_TEXT_Y + 3, "TRY TO MOVE");
            font_print(2, TUTORIAL_TEXT_Y + 4, "LEFT AND RIGHT");
            break;
        case 1:
            font_print(1, TUTORIAL_TEXT_Y, "GREAT");
            font_print(1, TUTORIAL_TEXT_Y + 2, "MOVE TO THE TOOL");
            font_print(2, TUTORIAL_TEXT_Y + 3, "ON THE RIGHT");
            font_print(1, TUTORIAL_TEXT_Y + 5, "PRESS A TO");
            font_print(2, TUTORIAL_TEXT_Y + 6, "PICK IT UP");
            break;
        case 2:
            font_print(1, TUTORIAL_TEXT_Y, "WONDERFUL");
            font_print(1, TUTORIAL_TEXT_Y + 1, "WALK TO THE TASK");
            font_print(1, TUTORIAL_TEXT_Y + 3, "TRY TO MATCH THE");
            font_print(2, TUTORIAL_TEXT_Y + 4, "SYMBOL OF THE TOOL");
            break;
        case 3:
            font_print(1, TUTORIAL_TEXT_Y, "AWESOME");
            font_print(1, TUTORIAL_TEXT_Y + 2, "COMPLETE THREE");
            font_print(2, TUTORIAL_TEXT_Y + 3, "MORE TASKS");
            break;
        case 4:
            font_print(1, TUTORIAL_TEXT_Y, "CONGRATULATIONS");
            font_print(1, TUTORIAL_TEXT_Y + 1, "YOUVE COMPLETED THE");
            font_print(2, TUTORIAL_TEXT_Y + 2, "TUTORIAL");
            font_print(1, TUTORIAL_TEXT_Y + 4, "PRESS A TO LEAVE");
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
