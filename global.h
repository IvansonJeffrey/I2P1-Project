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

extern ALLEGRO_FONT *health_font;

extern float player_x;
extern float player_y; 
extern float player_speed; 

extern int  player_health;
extern bool player_invincible; 
extern int game_score;

extern float cam_x;
extern float cam_y;

extern ALLEGRO_BITMAP *death_frames[];
extern bool player_is_dead;
extern float death_timer;
extern int death_current_frame;
extern const float death_duration;
extern const float death_frame_interval;

extern ALLEGRO_FONT *menu_font;
extern float music_volume;

#define CHUNK_SIZE    512  

#define MAX_OBJ_TYPES 10


#define NUM_BINDS 7
extern int keymap[NUM_BINDS];

extern float cam_x;
extern float cam_y;

#endif
