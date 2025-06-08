#ifndef BATS_H_INCLUDED
#define BATS_H_INCLUDED

#include <allegro5/allegro.h>
#include "../global.h"

#define MAX_BATS 150
#define SAFE_RADIUS 200.0f
#define SPAWN_RADIUS 300.0f

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define BAT_FRAME_COUNT 3
extern float bat_anim_timer;   
extern int bat_current_frame;  

typedef struct {
    float x, y;
    float speed;
} Bat;

void bats_init(void);
void bats_update(float dt);
void bats_draw(void);
void bats_destroy(void);

int bats_get_count(void);
void bats_cheat_spawn(int count);

#endif // BATS_H_INCLUDED