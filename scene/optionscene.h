#ifndef OPTIONSCENE_H
#define OPTIONSCENE_H

#include "scene.h"
#include <allegro5/allegro_font.h>

Scene *New_OptionsScene(int label);
void   options_update(Scene *self);
void   options_draw  (Scene *self);
void   options_destroy(Scene *self);

#endif
