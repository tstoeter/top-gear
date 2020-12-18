/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *    By Elias Pschernig and Trent Gamblin.
 *    Author: Luiz Philippe (https://github.com/luizppa & https://luizppa.com)
 */

#include "include/environment.h"
#include "include/menus.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/error.h>
#include <allegro5/events.h>
#include <allegro5/fullscreen_mode.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_video.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// Initialize environment
int init() {
  srand(time(NULL));
  // Instaling plugins
  init_environment();

  // Seting path
  ALLEGRO_PATH* p = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
  al_change_directory(al_path_cstr(p, '\\'));
  al_destroy_path(p);

  // Display settings
  setup_display();

  // Sound settings
  setup_sounds();

  // Music setings
  music = set_music(TITLE_MUSIC);
  al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
  start_music(music, true);

  // Events setings
  setup_events();
  if (!display || !queue || !timer || !music) {
    printf("Failed setup.\n");
    return -1;
  }

  // Presentation screen
  draw_text(DISKUN_60, BLUE, sw/2, (sh/2)-60, ALLEGRO_ALIGN_CENTRE, "LUIZ PHILIPPE", false);
  draw_text(PIXEL_28, WHITE, sw/2, sh/2, ALLEGRO_ALIGN_CENTRE, "presents", true);
  al_rest(3);
  al_flush_event_queue(queue);

  // Title screen
  draw_title();
  return 0;

}

// Clear environment
void deinit() {
  destroy_bitmaps();
  destroy_sounds();
  destroy_environment();
  clearenv();
}

int main() {
  init();
  int op;
  while (true) {
    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);
    // Events handling
    // Close window
    if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
      break;
    }
    else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      // Quit game
      if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
        deinit();
        return 0;
      }
      // Go to main menu
      else if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
        play_sample(MENU_SELECT_SOUND);
        while(true){
          op = main_menu();
          // Quit game
          if(op == 4){
            deinit();
            return 0;
          }
          // Back to title
          else if(op == -1){
            draw_title();
            break;
          }
        }
      }
    }
  }
  deinit();
  return 0;
}