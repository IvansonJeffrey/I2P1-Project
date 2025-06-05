#ifndef BATS_H_INCLUDED
#define BATS_H_INCLUDED

#include <allegro5/allegro.h>
#include "../global.h"   // for player_x, player_y, cam_x, cam_y, FPS

#define MAX_BATS 100
#define SAFE_RADIUS 200.0f
#define SPAWN_RADIUS 300.0f

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// A single bat moves straight toward the player (ignoring obstacles):
typedef struct {
    float x, y;    // world coordinates
    float speed;   // pixels per second
} Bat;

// Public API for the “bat subsystem”:
void bats_init(void);
void bats_update(float dt);
void bats_draw(void);
void bats_destroy(void);

#endif // BATS_H_INCLUDED