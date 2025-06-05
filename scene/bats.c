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

static int fib(int n) {
    if (n <= 1) return 1;   // treat fib(0)=1, fib(1)=1 for our “shifted” sequence
    return fib(n - 1) + fib(n - 2);
}


// Initialize bat subsystem: load sprite and set timers
void bats_init(void) {
    bat_sprite = al_load_bitmap("assets/tilesets/bat.png");
    GAME_ASSERT(bat_sprite, "Could not load assets/tilesets/bat.png\n");
    num_bats       = 0;
    spawn_interval = 2.0f;       // first wave at 2s
    spawn_timer    = spawn_interval;
    elapsed_time   = 0.0f;
}

// Update bats each frame: spawn new waves (Fibonacci) and move toward player
void bats_update(float dt) {
    elapsed_time += dt;
    spawn_timer  += dt;

    // Determine current “minute bucket”
    int minute_index = (int)(elapsed_time / 60.0f);
    if (minute_index < 0) minute_index = 0;
    
    int n = minute_index + 2;
    if (n > 11) n = 11;
    int spawn_count = fib(n);

    // Spawn a wave if it’s time
    if (spawn_timer >= spawn_interval) {
        spawn_timer -= spawn_interval;

        spawn_interval = 2.0f + ((float)rand() / (float)RAND_MAX) * 1.0f;

        for (int k = 0; k < spawn_count && num_bats < MAX_BATS; k++) {
            // Random angle [0,2π)
            float angle = ((float)rand() / (float)RAND_MAX) * (2.0f * M_PI);
            // Random distance ∈ [SAFE_RADIUS, SPAWN_RADIUS]
            float spawn_dist = SAFE_RADIUS +
                ((float)rand() / (float)RAND_MAX) * (SPAWN_RADIUS - SAFE_RADIUS);
            float bx = player_x + cosf(angle) * spawn_dist;
            float by = player_y + sinf(angle) * spawn_dist;

            bats[num_bats].x = bx;
            bats[num_bats].y = by;
            bats[num_bats].speed = 100.0f;  // pixels/sec
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
            float dx = bats[i].x - player_x;
            float dy = bats[i].y - player_y;
            if ((dx * dx + dy * dy) <= sum_r2) {
                // Collision detected!
                player_health--;
                printf("Player hit by a bat!  Health now: %d\n", player_health);

                bats[i] = bats[num_bats - 1];
                num_bats--;
                
                // If health is zero or below, immediately terminate the program:
                if (player_health <= 0) {
                    printf("Game Over!\n");
                    fflush(stdout);
                    exit(0);
                }
                // Note: do NOT increment i, because we swapped in a new bat at index i.
                continue;
            }
            // no collision: move on to next bat
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
