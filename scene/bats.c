// File: bats.c
#include "bats.h"
#include "../GAME_ASSERT.h"
#include <stdlib.h>    // rand(), srand(), RAND_MAX
#include <math.h>      // floorf(), sqrtf(), cosf(), sinf()
#include "../global.h"

// Internal storage (static)
static ALLEGRO_BITMAP *bat_sprite = NULL;
static Bat bats[MAX_BATS];
static int num_bats = 0;
static float spawn_timer = 0.0f;
static float spawn_interval = 2.0f;
static float elapsed_time = 0.0f;


// Initialize bat subsystem: load sprite and set timers
void bats_init(void) {
    bat_sprite = al_load_bitmap("assets/tilesets/bat.png");
    GAME_ASSERT(bat_sprite, "Could not load assets/tilesets/bat.png\n");
    num_bats       = 0;
    spawn_interval = 2.0f;  
    spawn_timer    = spawn_interval;
    elapsed_time   = 0.0f;
}

void bats_cheat_spawn(int count) {
    for (int i = 0; i < count && num_bats < MAX_BATS; i++) {
       // Spread evenly or randomly? We'll randomize the angle:
        float angle = ((float)rand() / (float)RAND_MAX) * (2.0f * M_PI);
        float spawn_dist = SAFE_RADIUS; 
        // Place exactly on the invisible circle (no extra random radius)
        float bx = player_x + cosf(angle) * spawn_dist;
        float by = player_y + sinf(angle) * spawn_dist;

        bats[num_bats].x = bx;
        bats[num_bats].y = by;
        bats[num_bats].speed = 100.0f;  
        num_bats++;
    }
}


// Update bats each frame: spawn new waves (Fibonacci) and move toward player
void bats_update(float dt) {
    elapsed_time += dt;
    spawn_timer  += dt;

    // Determine current “minute bucket”
    int minute_index = (int)(elapsed_time / 60.0f);
    if (minute_index < 0) minute_index = 0;
    
    int spawn_count = 2 * (minute_index + 1);

    // Spawn a wave if it’s time
    if (spawn_timer >= spawn_interval) {
        spawn_timer -= spawn_interval;

        for (int k = 0; k < spawn_count && num_bats < MAX_BATS; k++) {
            float angle = ((float)rand() / (float)RAND_MAX) * (2.0f * M_PI);
            float spawn_dist = SAFE_RADIUS + ((float)rand() / (float)RAND_MAX) * (SPAWN_RADIUS - SAFE_RADIUS);
            float bx = player_x + cosf(angle) * spawn_dist;
            float by = player_y + sinf(angle) * spawn_dist;

            bats[num_bats].x = bx;
            bats[num_bats].y = by;
            bats[num_bats].speed = 100.0f;     // pixels/sec
            num_bats++;
       }
    }

    // Move all active bats straight toward the player
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

    if (bat_sprite) {
        int bw = al_get_bitmap_width(bat_sprite);
        int bh = al_get_bitmap_height(bat_sprite);
        float bat_radius = (bw < bh ? bw : bh) * 0.5f / 2.0f; 
        // Note: /2.0f because we drew bat with its center, but original code draws
        // bat_sprite with bat’s center at (x - bw/2, y - bh/2). So half of half‐dimension.
        // If you want a slightly smaller collision area, leave it as is; otherwise, you
        // can just use max(bw,bh)*0.5f.

        const float player_radius = 16.0f;  
        const float sum_r = player_radius + bat_radius;
        const float sum_r2 = sum_r * sum_r;

        for (int i = 0; i < num_bats; ) {
            // 1) Compute vector from player to this bat:
            float dx = bats[i].x - player_x;
            float dy = bats[i].y - player_y;

            // 2) Check circle vs circle overlap
            if ((dx * dx + dy * dy) <= sum_r2) {
                if (!player_invincible) {
                    // a) Damage the player and remove the bat
                    player_health--;
                    printf("Player hit by a bat!  Health now: %d\n", player_health);

                    if (player_health <= 0) {
                        // Game Over: print score (0 for now) and exit
                        printf("\nGame Over!\nScore: %d\n", game_score);
                        fflush(stdout);
                        exit(0);
                    }

                    // Remove the bat by swapping with last
                    bats[i] = bats[num_bats - 1];
                    num_bats--;
                    // do NOT i++ because we just swapped another bat into index i
                    continue;
                } else {
                    // b) Player is invincible → push bat just outside collision radius
                    float dist_actual = sqrtf(dx * dx + dy * dy);
                    if (dist_actual > 0.0f) {
                        float push_factor = sum_r / dist_actual;
                        bats[i].x = player_x + dx * push_factor;
                        bats[i].y = player_y + dy * push_factor;
                    }
                    // Then advance to next bat:
                    i++;
                    continue;
                }
            }
            // No collision → move on
            i++;
        }
    }
}

// Draw all active bats on top of the scene
void bats_draw(void) {
    if (!bat_sprite) return;
    int bw = al_get_bitmap_width (bat_sprite);
    int bh = al_get_bitmap_height(bat_sprite);
    for (int i = 0; i < num_bats; i++) {
        float sx = bats[i].x - cam_x - (bw * 0.5f);
        float sy = bats[i].y - cam_y - (bh * 0.5f);
        al_draw_bitmap(bat_sprite, sx, sy, 0);
    }
}

// Clean up bat sprite
void bats_destroy(void) {
    if (bat_sprite) {
        al_destroy_bitmap(bat_sprite);
        bat_sprite = NULL;
    }
    num_bats = 0;
}

int bats_get_count(void) {
    return num_bats;
}
