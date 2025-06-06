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

/// I2P_GameScene: holds the ground_map + object_tiles + dimensions
typedef struct _I2P_GameScene {
    Scene base;                           // so we can cast to Scene*
    ALLEGRO_BITMAP *ground_map;           
    ALLEGRO_BITMAP *object_tiles[MAX_OBJ_TYPES];

    int hit_left_inset[MAX_OBJ_TYPES];
    int hit_top_inset[MAX_OBJ_TYPES];
    int hit_width     [MAX_OBJ_TYPES];
    int hit_height    [MAX_OBJ_TYPES];
    
    int ground_w, ground_h;
} I2P_GameScene;

/// Factory: create a new I2P_GameScene for SceneType == GameScene_L
Scene *I2P_NewGameScene(int label);

/// Each frame: update logic (movement + camera)
void I2P_game_scene_update(Scene *self);

/// Each frame: draw the infinite grass + objects + player
void I2P_game_scene_draw(Scene *self);

/// Free bitmaps + struct when scene ends
void I2P_game_scene_destroy(Scene *self);

#endif // GAME_H_INCLUDED
