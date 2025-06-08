#include "projectile.h"
#include "../shapes/Circle.h"
#include "../scene/gamescene.h"
#include "../scene/sceneManager.h"
#include <allegro5/allegro_primitives.h>
#include "../scene/bats.h"
#include <stdlib.h>
#include <math.h>

/*
   [Projectile function]
*/
Elements *New_Projectile(int label, float x, float y, float dirx, float diry)
{
    Projectile *pDerivedObj = (Projectile *)malloc(sizeof(Projectile));
    Elements *pObj = New_Elements(label);

    pDerivedObj->x = (float)x;
    pDerivedObj->y = (float)y;
    pDerivedObj->dx = dirx * 500.0f;  // constant speed = 500 px/sec
    pDerivedObj->dy = diry * 500.0f;
    pDerivedObj->hitbox = New_Circle(pDerivedObj->x,
                                     pDerivedObj->y,
                                     4.0f /* radius */);

    pObj->pDerivedObj = pDerivedObj;
    pObj->Update  = Projectile_update;
    pObj->Interact = Projectile_interact;
    pObj->Draw    = Projectile_draw;
    pObj->Destroy = Projectile_destroy;
    return pObj;
}


void Projectile_update(Elements *self)
{
     Projectile *Obj = (Projectile *)(self->pDerivedObj);
    float dt = 1.0f / FPS;
    float step_x = Obj->dx * dt;
    float step_y = Obj->dy * dt;

    Obj->x += step_x;
    Obj->y += step_y;

    Obj->hitbox->update_center_x(Obj->hitbox, step_x);
    Obj->hitbox->update_center_y(Obj->hitbox, step_y);

    float screen_x = Obj->x - cam_x;
    float screen_y = Obj->y - cam_y;
    if (screen_x < -10 || screen_x > WIDTH + 10 ||
        screen_y < -10 || screen_y > HEIGHT + 10) {
        self->dele = true;
        return;
    }
}

void Projectile_interact(Elements *self)
{
    Projectile *Obj = (Projectile *)(self->pDerivedObj);

    extern Bat* bats_array(void);
    extern int* bats_count_ptr(void);

    Bat *bat_arr = bats_array();
    int *bat_cnt = bats_count_ptr();
    int i = 0;
    while (i < *bat_cnt) {
        float bx = bat_arr[i].x;
        float by = bat_arr[i].y;

        float dx = Obj->x - bx;
        float dy = Obj->y - by;

        const float bat_radius = 16.0f;   
        const float proj_radius = 4.0f;
        float combined = bat_radius + proj_radius;

        if ((dx*dx + dy*dy) <= (combined * combined)) {
            bat_arr[i] = bat_arr[*bat_cnt - 1];
            (*bat_cnt)--;

            self->dele = true;

            game_score++;
            return;
        }
        i++;
    }
}

void Projectile_draw(Elements *self)
{
    Projectile *Obj = (Projectile *)(self->pDerivedObj);
    float sx = Obj->x - cam_x;
    float sy = Obj->y - cam_y;
    al_draw_filled_circle(sx, sy, 4.0f, al_map_rgb(255,255,255));
}

void Projectile_destroy(Elements *self)
{
    Projectile *Obj = (Projectile *)(self->pDerivedObj);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}
