#ifndef CHARATER_H_INCLUDED
#define CHARATER_H_INCLUDED
#include <allegro5/allegro_audio.h>
#include "element.h"
#include "../shapes/Shape.h"
#include "../algif5/algif.h"
#include <stdbool.h>

/*
   [character object]
*/
typedef enum CharacterType
{
    STOP = 0,
    MOVE,
    ATK
} CharacterType;

typedef struct _Character
{
    int x, y;
    int width, height;
    bool dir;
    int state;
    ALGIF_ANIMATION *gif_status[3]; 
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime;
    int anime_time;
    bool new_proj;
    Shape *hitbox;
} Character;

Elements *New_Character(int label);
void Character_update(Elements *self);
void Character_interact(Elements *self);
void Character_draw(Elements *self);
void Character_destory(Elements *self);
void _Character_update_position(Elements *self, int dx, int dy);

#endif
