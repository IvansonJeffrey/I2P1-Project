#ifndef RESOLUTIONSCENE_H
#define RESOLUTIONSCENE_H

#include "scene.h"

Scene *New_ResolutionScene(int label);
void   resolution_update (Scene *self);
void   resolution_draw   (Scene *self);
void   resolution_destroy(Scene *self);

#endif
