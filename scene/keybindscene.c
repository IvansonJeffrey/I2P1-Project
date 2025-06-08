#include "keybindscene.h"
#include "sceneManager.h"
#include "scene.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdlib.h>

static const char *actions[] = {
    "Move Up",
    "Move Down",
    "Move Left",
    "Move Right",
    "Attack",
    "Back"
};
#define ACT_COUNT (sizeof(actions)/sizeof(*actions))


Scene *New_KeyBindScene(int label) {
    KeyBindScene *k = malloc(sizeof(*k));
    Scene *scn = New_Scene(label);

    k->font = al_load_ttf_font("assets/font/pirulen.ttf", 20, 0);
    k->x = WIDTH  / 2;
    k->y = HEIGHT / 2 - 100;
    k->selected = 0;
    k->waiting_for_key = false;

    k->keymap[0] = ALLEGRO_KEY_W;
    k->keymap[1] = ALLEGRO_KEY_S;
    k->keymap[2] = ALLEGRO_KEY_A;
    k->keymap[3] = ALLEGRO_KEY_D;
    k->keymap[4] = ALLEGRO_KEY_SPACE;
    k->keymap[5] = 0; // Back

    scn->pDerivedObj = k;
    scn->Update = keybind_update;
    scn->Draw = keybind_draw;
    scn->Destroy = keybind_destroy;
    return scn;
}

void keybind_update(Scene *self) {
    KeyBindScene *k = self->pDerivedObj;

    if (k->waiting_for_key) {
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            k->keymap[k->selected] = event.keyboard.keycode;
            k->waiting_for_key = false;
        }
        return;
    }

    static bool up_prev, dn_prev;
    bool up = key_state[ALLEGRO_KEY_UP],
         dn = key_state[ALLEGRO_KEY_DOWN];
    if (up && !up_prev)   k->selected = (k->selected + ACT_COUNT - 1) % ACT_COUNT;
    if (dn && !dn_prev)   k->selected = (k->selected + 1) % ACT_COUNT;
    up_prev = up; dn_prev = dn;

    if (key_state[ALLEGRO_KEY_ENTER]) {
        if (k->selected == ACT_COUNT-1) {
            self->scene_end = true;
            window = OptionsScene_L;
        } else {
            k->waiting_for_key = true;
        }
    }
}

void keybind_draw(Scene *self) {
    KeyBindScene *k = self->pDerivedObj;
    al_clear_to_color(al_map_rgb(0,0,0));

    for (int i = 0; i < ACT_COUNT; i++) {
        int yy = k->y + i * 50;
        ALLEGRO_COLOR col = (i == k->selected)
            ? al_map_rgb(255,255,0)
            : al_map_rgb(200,200,200);

        char buf[64];
        if (i < ACT_COUNT-1) {
            const char *name = al_keycode_to_name(k->keymap[i]);
            snprintf(buf, sizeof(buf), "%-10s   [%s]", actions[i], name);
        } else {
            strcpy(buf, actions[i]);
        }

        al_draw_text(k->font, col, k->x, yy,
                     ALLEGRO_ALIGN_CENTRE, buf);
    }

    if (k->waiting_for_key) {
        al_draw_text(k->font, al_map_rgb(255,0,0),
                     k->x, k->y + ACT_COUNT*50 + 20,
                     ALLEGRO_ALIGN_CENTRE,
                     "Press a new key...");
    }

    al_flip_display();
}

void keybind_destroy(Scene *self) {
    KeyBindScene *k = self->pDerivedObj;
    al_destroy_font(k->font);
    free(k);
    free(self);
}
