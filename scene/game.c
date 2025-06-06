// File: scene/game.c

#include "game.h"
#include "../GAME_ASSERT.h"     // for runtime assertions
#include <allegro5/allegro.h>           // core Allegro definitions
#include <allegro5/allegro_image.h>     // al_load_bitmap, al_convert_mask_to_alpha, etc.
#include <allegro5/allegro_primitives.h> // al_draw_bitmap, al_draw_filled_circle, etc.
#include <allegro5/allegro_color.h>     // al_unmap_rgb (and al_map_rgb)
#include <stdlib.h>
#include <math.h>
#include "bats.h"
#include "../element/projectile.h"
#include "sceneManager.h"
#include "../global.h"
#include "gamescene.h"

#define M_PI 3.14159265358979323846f

static ALLEGRO_BITMAP *player_frames[PLAYER_FRAME_COUNT] = { NULL };
static int player_current_frame = 0;
static float player_anim_timer = 0.0f;
static const float player_frame_interval = 0.10f;  // seconds per frame

static bool player_is_moving(float dx, float dy) {
    return (dx != 0.0f || dy != 0.0f);
}

float player_facing_dx = 0.0f;
float player_facing_dy = -1.0f; 

static float attack_timer = 0.0f;

typedef struct {
    int         type;
    float       world_x, world_y;
    int         spr_w, spr_h;
} ObjPlacement;

// Paths to the 10 object sprites in assets/tilesets/:
static const char *object_filenames[MAX_OBJ_TYPES] = {
    "assets/tilesets/fence-1.png",
    "assets/tilesets/fence-2.png",
    "assets/tilesets/tree-1.png",
    "assets/tilesets/tree-2.png"
};
static const int NUM_OBJ_TYPES = 4;

static int I2P_get_objects_in_chunk(I2P_GameScene *gs, int cx, int cy, ObjPlacement *out, int max_slots) {
    // Now you can safely use `gs->object_tiles[type]` below.
    unsigned int seed = (unsigned int)((cx * 73856093u) ^ (cy * 19349663u));
    srand(seed);

    int num = rand() % 10;
    float placed_cx[10], placed_cy[10], placed_r[10];
    int placed_count = 0;
    const float BUFFER = 4.0f;
    const int N_ATTEMPTS = 5;

    for (int i = 0; i < num && placed_count < max_slots; i++) {
        int type = rand() % NUM_OBJ_TYPES;
        ALLEGRO_BITMAP *bmp = gs->object_tiles[type];
        int spr_w = al_get_bitmap_width(bmp);
        int spr_h = al_get_bitmap_height(bmp);
        float obj_radius = (spr_w < spr_h ? spr_w : spr_h) * 0.25f;

        bool placed_ok = false;
        float final_wx = 0.0f, final_wy = 0.0f;

        for (int attempt = 0; attempt < N_ATTEMPTS; attempt++) {
            float local_x = (rand() / (float)RAND_MAX) * CHUNK_SIZE;
            float local_y = (rand() / (float)RAND_MAX) * CHUNK_SIZE;
            float world_x = cx * CHUNK_SIZE + local_x;
            float world_y = cy * CHUNK_SIZE + local_y;

            float c_x = world_x + (spr_w  / 2.0f);
            float c_y = world_y + (spr_h  / 2.0f);

            bool collide_any = false;
            for (int k = 0; k < placed_count; k++) {
                float dx = c_x - placed_cx[k];
                float dy = c_y - placed_cy[k];
                float minsep = obj_radius + placed_r[k] + BUFFER;
                if (dx*dx + dy*dy < (minsep * minsep)) {
                    collide_any = true;
                    break;
                }
            }
            if (!collide_any) {
                placed_ok = true;
                final_wx  = world_x;
                final_wy  = world_y;
                placed_cx[placed_count] = final_wx + (spr_w  / 2.0f);
                placed_cy[placed_count] = final_wy + (spr_h  / 2.0f);
                placed_r [placed_count] = obj_radius;
                placed_count++;
                break;
            }
        }
        if (!placed_ok) {
            continue;
        }

        ObjPlacement *op = &out[placed_count - 1];
        op->type    = type;
        op->world_x = final_wx;
        op->world_y = final_wy;
        op->spr_w   = spr_w;
        op->spr_h   = spr_h;
    }

    return placed_count;
}



static void I2P_generate_and_draw_objects_in_chunk(int cx, int cy, I2P_GameScene *gs) {
    ObjPlacement placements[10];
    // Now pass `gs` as the first argument:
    int count = I2P_get_objects_in_chunk(gs, cx, cy, placements, 10);

    for (int i = 0; i < count; i++) {
        ObjPlacement *op = &placements[i];
        float sx = op->world_x - cam_x;
        float sy = op->world_y - cam_y;
        if (sx >= -gs->ground_w && sx <= WIDTH + gs->ground_w &&
            sy >= -gs->ground_h && sy <= HEIGHT + gs->ground_h) {
            al_draw_bitmap(gs->object_tiles[op->type], sx, sy, 0);
        }
    }
}


Scene *I2P_NewGameScene(int label) {
    I2P_GameScene *gs = (I2P_GameScene *)calloc(1, sizeof(I2P_GameScene));
    GAME_ASSERT(gs, "Failed to allocate I2P_GameScene\n");

    // 1) Create the base Scene and attach our derived object:
    Scene *scene = New_Scene(label);
    scene->pDerivedObj = gs;

    // 2) Load ground_map.png (grass background) and store its dimensions:
    gs->ground_map = al_load_bitmap("assets/tilesets/ground_map.png");
    GAME_ASSERT(gs->ground_map, "Could not load assets/tilesets/ground_map.png\n");
    gs->ground_w = al_get_bitmap_width(gs->ground_map);
    gs->ground_h = al_get_bitmap_height(gs->ground_map);

    for (int i = 0; i < NUM_OBJ_TYPES; i++) {
        ALLEGRO_BITMAP *bmp = al_load_bitmap(object_filenames[i]);
        GAME_ASSERT(bmp, "Could not load %s\n", object_filenames[i]);
        // Store in our array:
        gs->object_tiles[i] = bmp;
    }

    bats_init();

    for (int i = 0; i < PLAYER_FRAME_COUNT; i++) {
        char path[64];
        sprintf(path, "assets/image/walk%d.png", i+1);
        player_frames[i] = al_load_bitmap(path);
        GAME_ASSERT(player_frames[i], "Failed to load player walk frame: %s\n", path);
    }
    
    // 4) Initialize global player position & camera:
    player_x = 0.0f;
    player_y = 0.0f;
    player_speed = 200.0f; 
    cam_x = player_x - (WIDTH  / 2.0f);
    cam_y = player_y - (HEIGHT / 2.0f);

    // 5) Hook up the update/draw/destroy callbacks:
    scene->Update  = I2P_game_scene_update;
    scene->Draw    = I2P_game_scene_draw;
    scene->Destroy = I2P_game_scene_destroy;

    return scene;
}

static bool I2P_check_collision(float cx, float cy, I2P_GameScene *gs) {
    const float player_radius = 16.0f;

    float pmin_x = cx - player_radius;
    float pmax_x = cx + player_radius;
    float pmin_y = cy - player_radius;
    float pmax_y = cy + player_radius;

    int chunk_min_x = floor(pmin_x / (float)CHUNK_SIZE) - 1;
    int chunk_max_x = floor(pmax_x / (float)CHUNK_SIZE) + 1;
    int chunk_min_y = floor(pmin_y / (float)CHUNK_SIZE) - 1;
    int chunk_max_y = floor(pmax_y / (float)CHUNK_SIZE) + 1;

    ObjPlacement placements[10];

    for (int cyunk = chunk_min_y; cyunk <= chunk_max_y; cyunk++) {
        for (int cxunk = chunk_min_x; cxunk <= chunk_max_x; cxunk++) {
            // Pass `gs` first:
            int count = I2P_get_objects_in_chunk(gs, cxunk, cyunk, placements, 10);
            for (int i = 0; i < count; i++) {
                ObjPlacement *op = &placements[i];
                float obj_cx = op->world_x + (op->spr_w / 2.0f);
                float obj_cy = op->world_y + (op->spr_h / 2.0f);

                float obj_radius = (op->spr_w < op->spr_h ? op->spr_w : op->spr_h) * 0.25f;
                float sum_r = player_radius + obj_radius;
                float sum_r2 = sum_r * sum_r;

                // Broad‐phase AABB check
                if (obj_cx - sum_r > pmax_x) continue;
                if (obj_cx + sum_r < pmin_x) continue;
                if (obj_cy - sum_r > pmax_y) continue;
                if (obj_cy + sum_r < pmin_y) continue;

                float dx = cx - obj_cx;
                float dy = cy - obj_cy;
                if ((dx * dx + dy * dy) <= sum_r2) {
                    return true;
                }
            }
        }
    }
    return false;
}


void I2P_game_scene_update(Scene *self) {
    (void)self;
    float dt = 1.0f / FPS;

    // 1) Movement delta from input
    float dx = 0.0f, dy = 0.0f;
    if (key_state[ALLEGRO_KEY_W] || key_state[ALLEGRO_KEY_UP])    dy -= player_speed * dt;
    if (key_state[ALLEGRO_KEY_S] || key_state[ALLEGRO_KEY_DOWN])  dy += player_speed * dt;
    if (key_state[ALLEGRO_KEY_A] || key_state[ALLEGRO_KEY_LEFT])  dx -= player_speed * dt;
    if (key_state[ALLEGRO_KEY_D] || key_state[ALLEGRO_KEY_RIGHT]) dx += player_speed * dt;

    // 2) Normalize diagonal movement
    if (dx != 0.0f || dy != 0.0f) {
        float len = sqrtf(dx*dx + dy*dy);
        if (len > 0.0f) {
            player_facing_dx = dx / len;
            player_facing_dy = dy / len;
        }
    }

    if (dx != 0.0f && dy != 0.0f) {
        dx *= 0.7071067f;
        dy *= 0.7071067f;
    }

    
    I2P_GameScene *gs = (I2P_GameScene *)self->pDerivedObj;
   
   float try_x = player_x + dx;
   float try_y = player_y + dy;
   if (!I2P_check_collision(try_x, player_y, gs)) {
        player_x = try_x;
   }
   if (!I2P_check_collision(player_x, try_y, gs)) {
        player_y = try_y;
    }

    cam_x = player_x - (WIDTH  / 2.0f);
    cam_y = player_y - (HEIGHT / 2.0f);

    bool moving = player_is_moving(dx, dy);
    if (moving) {
        player_anim_timer += dt;
        if (player_anim_timer >= player_frame_interval) {
            player_anim_timer -= player_frame_interval;
            player_current_frame = (player_current_frame + 1) % PLAYER_FRAME_COUNT;
        }
    } else {
        // If not moving, go back to frame 0 (idle) and reset timer
        player_current_frame = 0;
        player_anim_timer = 0.0f;
    }

    bats_update(dt);

    attack_timer += dt;
    if (attack_timer >= 1.0f) {
        Elements *p = New_Projectile(Projectile_L, player_x, player_y, player_facing_dx, player_facing_dy);
        add_element_to_scene(self, p);
        attack_timer -= 1.0f;
    }

    ElementVec allEle = _Get_all_elements(self);
    //     6b) Run each element’s Update()
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        ele->Update(ele);        // e.g. Projectile_update(ele)
    }
    //     6c) Run each element’s Interact() (handles collisions, deletions)
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        ele->Interact(ele);
    }
    //     6d) Remove any elements flagged for deletion
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        if (ele->dele)
            _Remove_elements(self, ele);
    }
}

void I2P_game_scene_draw(Scene *self) {
    I2P_GameScene *gs = (I2P_GameScene *)self->pDerivedObj;

    al_clear_to_color(al_map_rgb(0, 0, 0));

    // Tile the ground_map:
    int first_tile_x = floor(cam_x / (float)gs->ground_w);
    int first_tile_y = floor(cam_y / (float)gs->ground_h);
    int tiles_across = (WIDTH  / gs->ground_w) + 2;
    int tiles_down   = (HEIGHT / gs->ground_h) + 2;

    for (int ty = 0; ty < tiles_down; ty++) {
        for (int tx = 0; tx < tiles_across; tx++) {
            int tile_x = first_tile_x + tx;
            int tile_y = first_tile_y + ty;
            float draw_x = tile_x * gs->ground_w - cam_x;
            float draw_y = tile_y * gs->ground_h - cam_y;
            al_draw_bitmap(gs->ground_map, draw_x, draw_y, 0);
        }
    }

    // Draw objects in each visible 512×512 chunk:
    int chunk_min_x = floor((cam_x) / (float)CHUNK_SIZE) - 1;
    int chunk_max_x = floor((cam_x + WIDTH) / (float)CHUNK_SIZE) + 1;
    int chunk_min_y = floor((cam_y) / (float)CHUNK_SIZE) - 1;
    int chunk_max_y = floor((cam_y + HEIGHT) / (float)CHUNK_SIZE) + 1;

    for (int cy = chunk_min_y; cy <= chunk_max_y; cy++) {
        for (int cx = chunk_min_x; cx <= chunk_max_x; cx++) {
            I2P_generate_and_draw_objects_in_chunk(cx, cy, gs);
        }
    }

    ALLEGRO_BITMAP *frame = player_frames[player_current_frame];    
    int fw = al_get_bitmap_width(frame);
    int fh = al_get_bitmap_height(frame);
    float draw_x = (player_x - cam_x) - (fw / 2.0f);
    float draw_y = (player_y - cam_y) - (fh / 2.0f);
    al_draw_bitmap(frame, draw_x, draw_y, 0);

    {
        // Player’s screen‐space center:
        float px = player_x - cam_x;
        float py = player_y - cam_y;

        // Facing unit vector:
        float dx = player_facing_dx;
        float dy = player_facing_dy;

        // A perpendicular vector to (dx,dy):
        float perp_x = -dy;
        float perp_y = dx;

        // How far out for the “tip”:
        float tip_dist  = 12.0f;
        // How wide the base:
        float base_dist = 4.0f;

        // Tip of the triangle:
        float tip_x = px + dx * tip_dist;
        float tip_y = py + dy * tip_dist;

        // Two base corners (slightly behind the player):
        float back_offset = 2.0f;
        float base1_x = px + perp_x * base_dist - dx * back_offset;
        float base1_y = py + perp_y * base_dist - dy * back_offset;
        float base2_x = px - perp_x * base_dist - dx * back_offset;
        float base2_y = py - perp_y * base_dist - dy * back_offset;

        ALLEGRO_COLOR col = al_map_rgb(255, 255, 255);
        al_draw_filled_triangle(
            tip_x,   tip_y,
            base1_x, base1_y,
            base2_x, base2_y,
            col
        );
    }

    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        ele->Draw(ele);  // e.g. Projectile_draw(ele) draws a white circle 
    }

    bats_draw();

    {
        char buf[32];
        sprintf(buf, "%d / 150 HP", player_health);
        // Y‐position: a 2-pixel margin above the bottom edge
        int text_x = 5;
        int text_y = HEIGHT - (40 + 2);
        al_draw_text(health_font, al_map_rgb(255, 0, 0), text_x, text_y, 0, buf);
    }

    al_flip_display();
}

void I2P_game_scene_destroy(Scene *self) {

    I2P_GameScene *gs = (I2P_GameScene *)self->pDerivedObj;
    if (!gs) return;

    if (gs->ground_map)
        al_destroy_bitmap(gs->ground_map);

    for (int i = 0; i < NUM_OBJ_TYPES; i++) {
        if (gs->object_tiles[i])
            al_destroy_bitmap(gs->object_tiles[i]);
    }


    bats_destroy();

    for (int i = 0; i < PLAYER_FRAME_COUNT; i++) {
        if (player_frames[i]) {
            al_destroy_bitmap(player_frames[i]);
            player_frames[i] = NULL;
        }
    }

    free(gs);
    free(self);
}
