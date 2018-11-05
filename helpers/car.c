#include "car.h"
#include "utils.h"
#include "environment.h"

// Return a player car
CAR new_car(ALLEGRO_BITMAP* texture){
  CAR car;
  car.lvl = 0;
  car.name = "Player";
  car.points = 0;
  car.width = get_bitmap_width(texture);
  car.height = get_bitmap_height(texture);
  car.speed = 0.0;
  car.position_x = -425;
  car.position_y = 0.0;
  car.screen_position_x = sw/2;
  car.screen_position_y = 0;
  car.fuel = 100.0;
  car.gear = 1;
  car.max_gear = 6;
  car.will_colide = false;
  car.texture = texture;
  return car;
}

// Return a AI car
CAR new_oponent(int lvl, ALLEGRO_BITMAP *texture){
  CAR car;
  car.lvl = lvl;
  car.name = names[lvl];
  car.points = 0;
  car.width = get_bitmap_width(texture);
  car.height = get_bitmap_height(texture);
  car.speed = 0.0;
  car.fuel = 100.0;
  car.gear = 1;
  car.max_gear = 6;
  car.texture = texture;
  if(lvl%3 == 0) car.will_colide = false;
  else car.will_colide = true;
  car.going_right = false;
  if(lvl < 4){
    car.position_y = (3*STARTING_DISTANCE);
  }
  else if(lvl < 7){
    car.position_y = (2*STARTING_DISTANCE);
  }
  else if(lvl < 10){
    car.position_y = (1*STARTING_DISTANCE);
  }
  else if(lvl < 13){
    car.position_y = (0*STARTING_DISTANCE);
  }
  float x = ((lvl%3)-1);
  car.position_x = (x*25)+(x*400);
  return car;
}

// Gear up
void gear_up(CAR *car){
  if(car->gear < car->max_gear) car->gear++;
}

// Gear down
void gear_down(CAR *car){
  if(car->gear > 1) car->gear--;
}

// Set to specifc gear
void set_gear(CAR *car, int gear){
  if(gear > 0 && gear <= car->max_gear) car->gear = gear;
}

// Return the max speed for each gear
float max_speed(int gear){
  switch (gear) {
    case 1:
      return 20.0;
    case 2:
      return 50.0;
    case 3:
      return 85.0;
    case 4:
      return 140.0;
    case 5:
      return 210.0;
    case 6:
      return 290.0;
  }
}

/*
  Returns the variation rate of the car speed for each 1/fps seconds.
  If the current speed is higher than the current gear's max speed, returns the
  diference between them times a constant.
  If the gear is higher than 80% of the bellow gear's max speed, returns the best performance variation.
  Else, returns 30% to 40% of the bellow gear's max speed.
*/
float speed_increase(int gear, float speed){
  if(speed > max_speed(gear)) return (max_speed(gear)-speed)*WRONG_GEAR_EFFECT;
  switch (gear) {
    case 1:
      return 16.0/fps;
    case 2:
      if(speed > 0.8*max_speed(gear-1)) return 6.4/fps;
      else return 0.3*speed_increase(gear-1, speed);
    case 3:
      if(speed > 0.8*max_speed(gear-1)) return 7.0/fps;
      else return 0.4*speed_increase(gear-1, speed);
    case 4:
      if(speed > 0.8*max_speed(gear-1)) return 7.7/fps;
      else return 0.4*speed_increase(gear-1, speed);
    case 5:
      if(speed > 0.8*max_speed(gear-1)) return 8.0/fps;
      else return 0.4*speed_increase(gear-1, speed);
    case 6:
      if(speed > 0.8*max_speed(gear-1)) return 8.6/fps;
      else return 0.4*speed_increase(gear-1, speed);
  }
}

// Sort the cars array based on their Y position
CAR** quick_sort_cars(CAR** cars, int size){
  int wall = 0, pivot = size-1;
  CAR* aux;
  while(pivot > wall){
    for (int i = wall; i < pivot; i++) {
      if(cars[i]->position_y < cars[pivot]->position_y){
        aux = cars[wall];
        cars[wall] = cars[i];
        cars[i] = aux;
        wall++;
      }
    }
    aux = cars[wall];
    cars[wall] = cars[pivot];
    cars[pivot] = aux;
    wall++;
  }
  return cars;
}

bool are_cars_aligned(CAR* a, CAR* b){
  float a_x0 = a->screen_position_x - (a->width/2);
  float a_xf = a->screen_position_x + (a->width/2);
  float b_x0 = b->screen_position_x - (a->width/2);
  float b_xf = b->screen_position_x + (b->width/2);
  if(debug){
    al_draw_line(a_x0, 0, a_x0, sh, RED, 1);
    al_draw_line(a_xf, 0, a_xf, sh, YELLOW, 1);
    al_draw_line(b_x0, 0, b_x0, sh, BLUE, 1);
    al_draw_line(b_xf, 0, b_xf, sh, GREEN, 1);
  }
  if ((a_xf - a_x0 >= b_x0 - a_x0 && b_x0 - a_x0 >= 0) || (b_xf - b_x0 >= a_x0 - b_x0 && a_x0 - b_x0 >= 0)) {
    if(debug) printf("aligned\n");
    return true;
  }
  else {
    if(debug) printf("not aligned\n");
    return false;
  }
}

bool car_colided(CAR* car, CAR** cars, int car_count){
  float relative_speed, distance;
  bool aligned;
  car->will_colide = false;
  for(int i = 0; i < car_count; i++){
    relative_speed = car->speed - cars[i]->speed;
    distance = cars[i]->position_y - car->position_y;
    aligned = are_cars_aligned(car, cars[i]);
    // Don't verify colisions of a car with itslef
    if(cars[i] != car){
      // If the cars are aligned and "car" is getting closer to "cars[i]"
      if(aligned && relative_speed > 0){
        // If the car is about to colide, set a warn to steer
        if (distance <= COLISION_DISTANCE*1.8 && distance > COLISION_DISTANCE*0.5){
          car->will_colide = true;
        }
        // If the car is less than COLISION_DISTANCE meters away from cars[i]
        if (distance <= COLISION_DISTANCE && distance > 0) {
          car->speed = max(0, car->speed-(relative_speed*2.3));
          cars[i]->speed += relative_speed;
          return true;
        }
      }
      // If "car" is not getting closer to "cars[i]" but has already touched it
      else if(aligned && (distance <= COLISION_CRITICAL_DISTACE && distance >= 0)){
        car->speed *= 0.4;
        return true;
      }
    }
  }
  return false;
}

// Returns the speed in which the AI controlled car sould raise its gear
float ai_gear_up_point(int gear, int lvl){
  // The closer the miss_rate gets to 0.8, the more precise the gear transition is
  float skill_rate = (0.1/12.0)*lvl;
  float miss_rate;
  switch (gear) {
    case 1:
      miss_rate = 0.65+skill_rate;
      break;
    case 2:
      miss_rate = 0.66+skill_rate;
      break;
    case 3:
      miss_rate = 0.67+skill_rate;
      break;
    case 4:
      miss_rate = 0.685+skill_rate;
      break;
    case 5:
      miss_rate = 0.69+skill_rate;
      break;
  }
  return max_speed(gear)*miss_rate;
}

void control_ia(CAR* car, CAR** cars, int car_count){
  float skill_rate = (0.05/12.0)*car->lvl;
  float delta_speed = speed_increase(car->gear, car->speed);
  if(colisions) car_colided(car, cars, car_count);
  if(ai_pilots){
    // Verify colision
    if (car->will_colide) {
      // Go left
      if(!car->going_right){
        if(car->position_x > -440) car->position_x -= 20;
        else car->going_right = true;
      }
      // Go right
      if(car->going_right){
        if(car->position_x < 440) car->position_x += 20;
        else car->going_right = false;
      }
    }
    else {
      if(car->position_x >= 0) car->going_right = false;
      else car->going_right = true;
    }
    // Accelerating
    if(delta_speed < 0){
      if(car->speed != 0.8*max_speed(car->max_gear-1)) gear_down(car);
      delta_speed = speed_increase(car->gear, car->speed);
      car->speed = max(0, car->speed + delta_speed);
    }
    else {
      if(car->gear > 1 && car->speed < 0.8*max_speed(car->gear-1)){
        gear_down(car);
      }
      else if(car->speed >= ai_gear_up_point(car->gear, car->lvl) && car->gear < car->max_gear) {
        gear_up(car);
      }
      car->speed += delta_speed*(1-skill_rate);
    }
  }
  else {
    car->speed = max(0, car->speed - NO_ACCELERATE_EFFECT);
  }
  car->position_y += car->speed*DISTANCE_VARIATION;
}
