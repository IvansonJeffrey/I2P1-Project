// File: bats.c
#include "bats.h"
#include "../GAME_ASSERT.h"
#include <stdlib.h>
#include <math.h>
#include "../global.h"

ALLEGRO_BITMAP *bat_frames[BAT_FRAME_COUNT] = { NULL };
float bat_anim_timer = 0.0f;
int bat_current_frame = 0; 

static Bat bats[MAX_BATS];
static int num_bats = 0;
static float spawn_timer = 0.0f;
static float spawn_interval = 3.0f;
static float elapsed_time = 0.0f;


void bats_init(void) {
    for (int i = 0; i < BAT_FRAME_COUNT; i++) {
        char path[64];
        sprintf(path, "assets/tilesets/bat%d.png", i + 1);
        bat_frames[i] = al_load_bitmap(path);
        GAME_ASSERT(bat_frames[i], "Could not load %s\n", path);
    }

    bat_anim_timer = 0.0f;
    bat_current_frame = 0;

    num_bats = 0;
    spawn_interval = 3.0f;
    spawn_timer = spawn_interval;
    elapsed_time = 0.0f;
}

void bats_cheat_spawn(int count) {
    for (int i = 0; i < count && num_bats < MAX_BATS; i++) {
        float angle = ((float)rand() / (float)RAND_MAX) * (2.0f * M_PI);
        float spawn_dist = SAFE_RADIUS; 
        float bx = player_x + cosf(angle) * spawn_dist;
        float by = player_y + sinf(angle) * spawn_dist;

        bats[num_bats].x = bx;
        bats[num_bats].y = by;
        bats[num_bats].speed = 100.0f;  
        num_bats++;
    }
}

void bats_update(float dt) {

    if(player_is_dead)
    {
        return;
    }

    const float BAT_FRAME_INTERVAL = 0.10f;  
    bat_anim_timer += dt;
    if (bat_anim_timer >= BAT_FRAME_INTERVAL) {
        bat_anim_timer -= BAT_FRAME_INTERVAL;
        bat_current_frame = (bat_current_frame + 1) % BAT_FRAME_COUNT;
    }

    elapsed_time += dt;
    spawn_timer  += dt;

    int minute_index = (int)(elapsed_time / 60.0f);
    if (minute_index < 0) minute_index = 0;
    
    int spawn_count = 2 * (minute_index + 1);

    if (spawn_timer >= spawn_interval) {
        spawn_timer -= spawn_interval;

        for (int k = 0; k < spawn_count && num_bats < MAX_BATS; k++) {
            float angle = ((float)rand() / (float)RAND_MAX) * (2.0f * M_PI);
            float spawn_dist = SAFE_RADIUS + ((float)rand() / (float)RAND_MAX) * (SPAWN_RADIUS - SAFE_RADIUS);
            float bx = player_x + cosf(angle) * spawn_dist;
            float by = player_y + sinf(angle) * spawn_dist;

            bats[num_bats].x = bx;
            bats[num_bats].y = by;
            bats[num_bats].speed = 100.0f;
            num_bats++;
       }
    }

    for (int i = 0; i < num_bats; i++) {
        float vx = player_x - bats[i].x;
        float vy = player_y - bats[i].y;
        float dist = sqrtf(vx*vx + vy*vy);
        if (dist > 0.0f) {
            float inv = 1.0f / dist;
            bats[i].x += vx * inv * bats[i].speed * dt;
            bats[i].y += vy * inv * bats[i].speed * dt;
        }
    }

    if (bat_frames[bat_current_frame]) {
        ALLEGRO_BITMAP *frame = bat_frames[bat_current_frame];
        int bw = al_get_bitmap_width(frame);
        int bh = al_get_bitmap_height(frame);
        float bat_radius = (bw < bh ? bw : bh) * 0.5f / 2.0f;

        const float player_radius = 16.0f;  
        const float sum_r  = player_radius + bat_radius;
        const float sum_r2 = sum_r * sum_r;

        for (int i = 0; i < num_bats; ) {
            float dx = bats[i].x - player_x;
            float dy = bats[i].y - player_y;

            if ((dx * dx + dy * dy) <= sum_r2) {
                if (!player_invincible) {
                    if (player_health > 0) {
                        player_health--;
                    }
                    printf("Player hit by a bat!  Health now: %d\n", player_health);

                    if (player_health <= 0 && !player_is_dead) {
                        player_health = 0;
                        player_is_dead = true;
                        printf("\nGame Over!\nScore: %d\n", game_score);
                        fflush(stdout);
                        return; 
                    }

                    bats[i] = bats[num_bats - 1];
                    num_bats--;
                    continue;
                } else {
                    float dist_actual = sqrtf(dx * dx + dy * dy);
                    if (dist_actual > 0.0f) {
                        float push_factor = sum_r / dist_actual;
                        bats[i].x = player_x + dx * push_factor;
                        bats[i].y = player_y + dy * push_factor;
                    }
                   
                    i++;
                    continue;
                }
            }
            i++;
        }
    }
}

void bats_draw(void) {
    ALLEGRO_BITMAP *frame = bat_frames[bat_current_frame];
    if (!frame) return;  

    int bw = al_get_bitmap_width(frame);
    int bh = al_get_bitmap_height(frame);

    for (int i = 0; i < num_bats; i++) {
        float sx = bats[i].x - cam_x - (bw * 0.5f);
        float sy = bats[i].y - cam_y - (bh * 0.5f);
        al_draw_bitmap(frame, sx, sy, 0);
    }
}

void bats_destroy(void) {
    for (int i = 0; i < BAT_FRAME_COUNT; i++) {
        if (bat_frames[i]) {
            al_destroy_bitmap(bat_frames[i]);
            bat_frames[i] = NULL;
        }
    }
    num_bats = 0;
}

int bats_get_count(void) {
    return num_bats;
}

Bat* bats_array(void) {
    return bats;
}

int* bats_count_ptr(void) {
    return &num_bats;
}
