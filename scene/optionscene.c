#include "optionscene.h"
#include "sceneManager.h"    // for SceneType enums
#include "scene.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdlib.h>

static const char *main_items[] = {
    "Key Bindings",
    "Resolution",
    "Brightness",
    "Back to Menu"
};
#define MAIN_COUNT (sizeof(main_items)/sizeof(*main_items))


Scene *New_OptionsScene(int label) {
    OptionsScene *o= malloc(sizeof(*o));
    Scene*scn = New_Scene(label);

    o->font = al_load_ttf_font("assets/font/pirulen.ttf", 24, 0);
    o->x = WIDTH  / 2;
    o->y= HEIGHT / 2 - 60;
    o->selected_main  = 0;

    // record the current key_state so we don't fire on an already-held key
    o->up_prev    = key_state[ALLEGRO_KEY_UP];
    o->down_prev  = key_state[ALLEGRO_KEY_DOWN];
    o->enter_prev = key_state[ALLEGRO_KEY_ENTER];


    o->enter_prev = false;

    scn->pDerivedObj = o;
    scn->Update      = options_update;
    scn->Draw        = options_draw;
    scn->Destroy     = options_destroy;
    return scn;
}

void options_update(Scene *self) {
    OptionsScene *o = self->pDerivedObj;

    // Navigate the four main options once per key‐press:
    static bool up_prev = false, down_prev = false;
    bool up   = key_state[ALLEGRO_KEY_UP];
    bool down = key_state[ALLEGRO_KEY_DOWN];

    if (up && !up_prev)
        o->selected_main = (o->selected_main + MAIN_COUNT - 1) % MAIN_COUNT;
    if (down && !down_prev)
        o->selected_main = (o->selected_main + 1) % MAIN_COUNT;

    up_prev   = up;
    down_prev = down;

    bool enter = key_state[ALLEGRO_KEY_ENTER];
    if (enter && !o->enter_prev) {
        self->scene_end = true;
        switch (o->selected_main) {
            case 0: window = KeyBindScene_L; break;
            case 1: window = ResolutionScene_L; break;
            case 2: window = BrightnessScene_L; break;
            default: window = Menu_L;        break;
        }
    }
    o->enter_prev = enter;

    // On Enter, dispatch:
    if (key_state[ALLEGRO_KEY_ENTER]) {
        self->scene_end = true;
        switch (o->selected_main) {
            case 0:  window = KeyBindScene_L; break;
            default: window = Menu_L; break;
        }
    }
}

void options_draw(Scene *self) {
    OptionsScene *o = self->pDerivedObj;
    al_clear_to_color(al_map_rgb(0,0,0));

    // draw each main item
    for (int i = 0; i < MAIN_COUNT; i++) {
        int yy = o->y + i * 60;
        ALLEGRO_COLOR col = (i == o->selected_main)
            ? al_map_rgb(255,255,0)
            : al_map_rgb(200,200,200);
        al_draw_text(o->font, col, o->x, yy,
                     ALLEGRO_ALIGN_CENTRE, main_items[i]);
    }

    // single highlight box
    int hy = o->y + o->selected_main * 60;
    al_draw_rectangle(o->x - 200, hy - 20,
                      o->x + 200, hy + 28,
                      al_map_rgb(255,255,255), 2);

    al_flip_display();
}

void options_destroy(Scene *self) {
    OptionsScene *o = self->pDerivedObj;
    al_destroy_font(o->font);
    free(o);
    free(self);
}
