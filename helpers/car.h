#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

#include "object.h"

// Environmental values
#define GRASS_SLOW_EFFECT 0.13333333333
#define NO_ACCELERATE_EFFECT 0.05433333333
#define BREAK_EFFECT 0.73
#define WRONG_GEAR_EFFECT 0.00166666666
#define DISTANCE_VARIATION 0.147778
#define STARTING_DISTANCE 130
#define COLLISION_DISTANCE 50
#define COLLISION_CRITICAL_DISTACE 30

#define STANDARD_CAR_WIDTH 166.0
#define STANDARD_CAR_HEIGHT 120.0

// Car code
#define OCTANE 0
#define DOMINUS 1

struct CAR_TYPE {
  int lvl;
  char *name;
  int points;
  float width;
  float apparent_width;
  float height;
  float apparent_height;
  float speed;
  float position_x; // The car center x coordinate (on the road, where 0 is the center)
  float screen_position_x; // The car center x coordinate (on the screen, where 0 is the left border of the window)
  float position_y; // The car bottom y coordinate (on the road, a virtual environment)
  float screen_position_y; // The car center y coordinate (on the screen, where 0 is the top of the window)
  float fuel;
  int gear;
  int max_gear;
  bool will_colide;
  bool going_right;
  ALLEGRO_BITMAP* texture;
};
typedef struct CAR_TYPE CAR;

CAR new_car(ALLEGRO_BITMAP* texture);
CAR new_oponent(int lvl, ALLEGRO_BITMAP *texture);
void restart_positions(CAR* cars, int count);
void gear_up(CAR *car);
void gear_down(CAR *car);
void set_gear(CAR *car, int gear);
float max_speed(int gear);
float speed_increase(int gear, float speed);
int get_gear_progress(CAR car);
CAR** quick_sort_cars(CAR** cars, int size);
bool are_cars_aligned(CAR* a, CAR* b);
bool car_collided(CAR* car, CAR** cars, OBJECT* objects, int car_count, int object_count, bool play_sounds);
void control_ia(CAR* car, CAR** cars, OBJECT* objects, int car_count, int object_count, bool play_sounds);
