#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#define GAME_TERMINATE -1
#include "shapes/Point.h"
#define debug(x)                                                            \
    if (x)                                                                  \
    {                                                                       \
        printf("check at line number %d in file %s\n", __LINE__, __FILE__); \
    }
#define MAX_ELEMENT 100
#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) < (y) ? (y) : (x))
// note that you can't assign initial value for extern variable!
extern const double FPS;
extern const int WIDTH;
extern const int HEIGHT;
extern int window;
extern bool key_state[ALLEGRO_KEY_MAX];
extern bool mouse_state[ALLEGRO_MOUSE_MAX_EXTRA_AXES];
extern Point mouse;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_EVENT event;
extern ALLEGRO_TIMER *fps;
extern bool debug_mode;
extern int brightness;
extern ALLEGRO_DISPLAY *display;

extern ALLEGRO_FONT  *health_font;

extern float player_x;      // player’s world X position
extern float player_y;      // player’s world Y position
extern float player_speed;  // pixels per second

extern int  player_health; // player’s health (starts at 150)
extern bool player_invincible; 
extern int game_score;

extern float cam_x;         // camera’s top-left X in world coords
extern float cam_y;         // camera’s top-left Y in world coords

extern ALLEGRO_BITMAP *DEATH_FRAME_COUNT;
extern ALLEGRO_BITMAP *death_frames[];
extern bool player_is_dead;
extern float death_timer;
extern int death_current_frame;
extern const float death_duration;
extern const float death_frame_interval;

// Tile / chunk sizes:
#define CHUNK_SIZE    512    // each “chunk” is 512×512 pixels

// Number of object types (fences, pillars, plants, trees):
#define MAX_OBJ_TYPES 10
#endif
