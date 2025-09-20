#include <gbdk/platform.h>
#include <rand.h>
#include "scenes.h"
#include "input.h"
#include "font.h"
#include "sound.h"

struct game_state state;

struct scene *current_scene = 0;
struct scene *next_scene = 0;
void queue_scene(struct scene *next_next_scene)
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

  queue_scene(&scene_main_menu);
  init_state(0);
  // queue_scene(&scene_gameplay);
  // queue_scene(&scene_upgrade_menu);
  // queue_scene(&scene_tutorial);
  while (1)
  {
    input_scan();
    if (next_scene != current_scene)
    {
      current_scene = next_scene;
      current_scene->init();
    }
    current_scene->loop();
  }
}
