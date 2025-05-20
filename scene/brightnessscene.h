#ifndef BRIGHTNESSSCENE_H
#define BRIGHTNESSSCENE_H

#include "scene.h"

Scene *New_BrightnessScene(int label);
void   brightness_update (Scene *self);
void   brightness_draw   (Scene *self);
void   brightness_destroy(Scene *self);

#endif
