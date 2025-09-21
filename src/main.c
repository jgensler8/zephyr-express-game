#include <gbdk/platform.h>
#include <rand.h>
#include "scenes.h"
#include "input.h"
#include "font.h"
#include "sound.h"

struct game_state state;

enum scene current_scene = 254;
enum scene next_scene = 254;
void queue_scene(enum scene next_next_scene)
{
  next_scene = next_next_scene;
}

void main(void)
{
  // init
  SPRITES_8x16;
  SHOW_SPRITES;
  SHOW_BKG;
  FILL_BKG_EMPTY;
  sound_init();
  joypad_init(MAX_PLAYABLES, &joypads);
  SWITCH_ROM(1);

  queue_scene(SCENE_MAIN_MENU);
  // init_state(0);
  // queue_scene(&scene_gameplay);
  // queue_scene(&scene_upgrade_menu);
  // queue_scene(&scene_tutorial);
  scene_main_menu_init();
  while (1)
  {
    input_scan();
    if (next_scene != current_scene)
    {
      current_scene = next_scene;
      switch (current_scene)
      {
      case SCENE_MAIN_MENU:
        scene_main_menu_init();
        break;
      case SCENE_DIFFICULTY_SELECT:
        scene_difficulty_select_init();
        break;
      case SCENE_GAMEPLAY:
        scene_gameplay_init();
        break;
      case SCENE_UPGRADE_MENU:
        scene_upgrade_menu_init();
        break;
      case SCENE_TUTORIAL:
        scene_tutorial_init();
        break;
      }
    }
    switch (current_scene)
    {
    case SCENE_MAIN_MENU:
      scene_main_menu_loop();
      break;
    case SCENE_DIFFICULTY_SELECT:
      scene_difficulty_select_loop();
      break;
    case SCENE_GAMEPLAY:
      scene_gameplay_loop();
      break;
    case SCENE_UPGRADE_MENU:
      scene_upgrade_menu_loop();
      break;
    case SCENE_TUTORIAL:
      scene_tutorial_loop();
      break;
    }
  }
}
