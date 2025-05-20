#ifndef KEYBINDSCENE_H
#define KEYBINDSCENE_H

#include "scene.h"
#include <allegro5/allegro_font.h>

#define KB_ACT_COUNT 5
#define KB_MENU_COUNT (KB_ACT_COUNT + 2)

extern int keymap[KB_MENU_COUNT]; 

typedef struct KeyBindScene {
    ALLEGRO_FONT *font;
    int x, y;
    int selected;
    bool waiting_for_key;
    bool up_prev, down_prev, enter_prev;
} KeyBindScene;

Scene *New_KeyBindScene(int label);
void   keybind_update (Scene *self);
void   keybind_draw   (Scene *self);
void   keybind_destroy(Scene *self);

#endif
