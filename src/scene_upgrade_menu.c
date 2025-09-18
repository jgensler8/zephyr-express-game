#include "types.h"
#include "scenes.h"
#include "font.h"
#include "input.h"
#include <gbdk/metasprites.h>

void scene_upgrade_menu_init(void)
{
    FILL_BKG_EMPTY;
    hide_sprites_range(0, MAX_HARDWARE_SPRITES);

    font_set_bkg_data(1);
    font_print(1, 1, "ROUND OVER");
    font_print(1, 2, "SCORE");
    font_print(2, 3, "GOOD");
    font_print(2, 5, "UPGRADES");
    font_print(2, 6, "NEXT ROUND");
}

void scene_upgrade_menu_loop(void)
{
    if (PRESSED(0, J_A))
    {
        state.round += 1;
        init_state(state.round);
        queue_scene(&scene_gameplay);
        return;
    }
}

struct scene scene_upgrade_menu = {
    .init = scene_upgrade_menu_init,
    .loop = scene_upgrade_menu_loop,
};