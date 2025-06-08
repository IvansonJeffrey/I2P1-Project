#ifndef OPTIONSCENE_H
#define OPTIONSCENE_H

#include "scene.h"
#include <allegro5/allegro_font.h>

#include <stdbool.h>

typedef struct _OptionsScene {
    ALLEGRO_FONT *font;
    int x, y;
    int selected_main;
    bool up_prev, down_prev, enter_prev;
} OptionsScene;

Scene *New_OptionsScene(int label);
void options_update(Scene *self);
void options_draw  (Scene *self);
void options_destroy(Scene *self);

#endif
