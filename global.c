#include "global.h"
#include "shapes/Shape.h"
#include <stdbool.h>
// variables for global usage
const double FPS = 60.0;
const int WIDTH = 900;
const int HEIGHT = 672;
int window = 0;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT event;
ALLEGRO_TIMER *fps = NULL;
bool key_state[ALLEGRO_KEY_MAX] = {false};
bool mouse_state[ALLEGRO_MOUSE_MAX_EXTRA_AXES] = {false};
Point mouse;
bool debug_mode = true;
ALLEGRO_DISPLAY *display = NULL;
int brightness = 100;

float player_x     = 0.0f;      // start at world‐origin (0,0)
float player_y     = 0.0f;
float player_speed = 200.0f;    // 200 pixels/second

float cam_x = 0.0f;             // camera top-left in world coords
float cam_y = 0.0f;