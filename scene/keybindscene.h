#ifndef KEYBINDSCENE_H
#define KEYBINDSCENE_H

#include "scene.h"
#include <allegro5/allegro_font.h>

#define KB_ACT_COUNT 6

typedef struct KeyBindScene {
    ALLEGRO_FONT *font;
    int x, y;
    int selected;
    bool waiting_for_key;
    int keymap[KB_ACT_COUNT];
} KeyBindScene;

Scene *New_KeyBindScene(int label);
void   keybind_update (Scene *self);
void   keybind_draw   (Scene *self);
void   keybind_destroy(Scene *self);

#endif
