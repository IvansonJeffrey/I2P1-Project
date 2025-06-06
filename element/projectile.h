#ifndef Projectile_H_INCLUDED
#define Projectile_H_INCLUDED
#include "element.h"
#include "../shapes/Shape.h"
#include "../scene/bats.h"
/*
   [Projectile object]
*/
typedef struct _Projectile
{
    float x, y;          
    float dx, dy;
    Shape *hitbox; 
} Projectile;

Elements *New_Projectile(int label, float x, float y, float dirx, float diry);

void Projectile_update(Elements *self);
void Projectile_interact(Elements *self);
void Projectile_draw(Elements *self);
void Projectile_destroy(Elements *self);


#endif
