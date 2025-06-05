// File: scene/game.h
#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "scene.h"
#include "../global.h"
#include <allegro5/allegro.h>

#define MAX_BATS 100       // maximum number of bats at once
#define SPAWN_RADIUS 300.0f // spawn at least this far away from player
#define SAFE_RADIUS 200.0f  // do not spawn closer than this to player

typedef struct {
    float x, y;      // world coords of this bat
    float speed;     // movement speed (pixels/sec)
} Bat;


/// I2P_GameScene: holds the ground_map + object_tiles + dimensions
typedef struct _I2P_GameScene {
    Scene base;                           // so we can cast to Scene*
    ALLEGRO_BITMAP *ground_map;           
    ALLEGRO_BITMAP *object_tiles[MAX_OBJ_TYPES];

    int hit_left_inset[MAX_OBJ_TYPES];
    int hit_top_inset[MAX_OBJ_TYPES];
    int hit_width     [MAX_OBJ_TYPES];
    int hit_height    [MAX_OBJ_TYPES];

    ALLEGRO_BITMAP *bat_sprite;    // the bat image (shared by all bats)
    Bat bats[MAX_BATS];
    int num_bats;

    // Timing/spawn control:
    float spawn_timer;     // counts up (seconds) until next spawn
    float spawn_interval;  // current seconds between spawns
    float elapsed_time;    // total time since level start (seconds)
    
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
