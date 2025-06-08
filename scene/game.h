// File: scene/game.h
#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "scene.h"
#include "../global.h"
#include <allegro5/allegro.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define PLAYER_FRAME_COUNT 7

typedef struct {
    int cx, cy;
} EatenBurger;

typedef struct _I2P_GameScene {
    Scene base;
    ALLEGRO_BITMAP *ground_map;
    ALLEGRO_BITMAP *object_tiles[MAX_OBJ_TYPES];

    int hit_left_inset[MAX_OBJ_TYPES];
    int hit_top_inset[MAX_OBJ_TYPES];
    int hit_width [MAX_OBJ_TYPES];
    int hit_height [MAX_OBJ_TYPES];
    
    int ground_w, ground_h;

    EatenBurger *eaten_burgers;
    int eaten_count;
    int eaten_cap;
} I2P_GameScene;

Scene *I2P_NewGameScene(int label);
void I2P_game_scene_update(Scene *self);
void I2P_game_scene_draw(Scene *self);
void I2P_game_scene_destroy(Scene *self);

#endif // GAME_H_INCLUDED
