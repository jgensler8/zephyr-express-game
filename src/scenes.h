#pragma once
#include "types.h"

void queue_scene(struct scene *next_scene);
extern struct scene scene_main_menu;
extern struct scene scene_tutorial;
extern struct scene scene_difficulty_select;
extern struct scene scene_gameplay;
extern struct scene scene_upgrade_menu;