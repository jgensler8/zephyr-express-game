#pragma once
#include "types.h"

void queue_scene(enum scene next_scene);
void scene_main_menu_init(void);
void scene_main_menu_loop(void);
void scene_tutorial_init(void);
void scene_tutorial_loop(void);
void scene_difficulty_select_init(void);
void scene_difficulty_select_loop(void);
void scene_gameplay_init(void) BANKED;
void scene_gameplay_loop(void) BANKED;
void scene_upgrade_menu_init(void);
void scene_upgrade_menu_loop(void);