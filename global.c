#include "global.h"
#include "shapes/Shape.h"
#include <stdbool.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// variables for global usage
const double FPS = 60.0;
const int WIDTH = 1920;
const int HEIGHT = 1080;
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

ALLEGRO_FONT *health_font = NULL; 

float player_x = 0.0f; 
float player_y = 0.0f;
float player_speed = 200.0f;  

int player_health = 150; 
bool player_invincible = false;
int game_score = 0;

float cam_x = 0.0f;
float cam_y = 0.0f;

int keymap[NUM_BINDS] = {
    ALLEGRO_KEY_A,
    ALLEGRO_KEY_D,
    ALLEGRO_KEY_W, 
    ALLEGRO_KEY_S, 
    ALLEGRO_KEY_C, 
    ALLEGRO_KEY_F,  
    ALLEGRO_KEY_QUOTE
};

float music_volume = 1.0f;

void init_audio(void) {
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(4);
    al_set_mixer_gain(al_get_default_mixer(), music_volume);
}

