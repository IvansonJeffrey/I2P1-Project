#include "keybindscene.h"
#include "sceneManager.h"
#include "../global.h"
#include "scene.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static const char *actions[] = {
    "Move Up",
    "Move Down",
    "Move Left",
    "Move Right",
    "Attack",
    "Apply & Save",
    "Back"
};

Scene *New_KeyBindScene(int label) {
    KeyBindScene *k = malloc(sizeof(*k));
    Scene *scn = New_Scene(label);

    k->font = al_load_ttf_font("assets/font/pirulen.ttf", 20, 0);
    k->x = WIDTH  / 2;
    k->y = HEIGHT / 2 - (KB_MENU_COUNT * 25);
    k->selected = 0;
    k->waiting_for_key = false;

    // initialize defaults:
    k->up_prev = key_state[ALLEGRO_KEY_UP];
    k->down_prev = key_state[ALLEGRO_KEY_DOWN];
    k->enter_prev = key_state[ALLEGRO_KEY_ENTER];

    scn->pDerivedObj = k;
    scn->Update      = keybind_update;
    scn->Draw        = keybind_draw;
    scn->Destroy     = keybind_destroy;
    return scn;
}

void keybind_update(Scene *self) {
    KeyBindScene *k = self->pDerivedObj;

    if (k->waiting_for_key) {
        // look for *any* key-down event:
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            keymap[k->selected] = event.keyboard.keycode;
            k->waiting_for_key = false;
        }
        return;
    }

    // navigate
    static bool up_prev, dn_prev;
    bool up = key_state[ALLEGRO_KEY_UP];
    bool down = key_state[ALLEGRO_KEY_DOWN];
    if (up && !k->up_prev)   k->selected = (k->selected + KB_MENU_COUNT - 1) % KB_MENU_COUNT;
    if (down && !k->down_prev)   k->selected = (k->selected + 1) % KB_MENU_COUNT;
    k->up_prev = up; 
    k->down_prev = down;

    // Enter
    bool enter = key_state[ALLEGRO_KEY_ENTER];
    if (enter && !k->enter_prev) {
        if (k->selected == KB_ACT_COUNT) {
            FILE *f = fopen("keybinds.cfg", "w");
            if (f) {
                for (int i = 0; i < KB_ACT_COUNT; i++) {
                    fprintf(f, "%s=%d\n", menu_items[i], keymap[i]);
                }
                fclose(f);
            }
        } 
        else if(k->selected < KB_ACT_COUNT){
            // begin waiting for key
            k->waiting_for_key = true;
        }
        else {
            self->scene_end = true;
            window = OptionsScene_L;
        }
    }
    k->enter_prev = enter;
}

void keybind_draw(Scene *self) {
    KeyBindScene *k = (KeyBindScene*)self->pDerivedObj;
    al_clear_to_color(al_map_rgb(0,0,0));

    for (int i = 0; i < KB_MENU_COUNT; i++) {
        int yy = k->y + i * 50;
        ALLEGRO_COLOR col = (i == k->selected)
            ? al_map_rgb(255,255,0)
            : al_map_rgb(200,200,200);

        // draw “Action — KeyName”
        char buf[64];
        if (i < KB_ACT_COUNT) {
            const char *name = al_keycode_to_name(keymap[i]);
            snprintf(buf, sizeof(buf), "%-10s   [%s]", actions[i], name);
        } else {
            strcpy(buf, actions[i]);
        }

        al_draw_text(k->font, col, k->x, yy,
                     ALLEGRO_ALIGN_CENTRE, buf);
    }

    if (k->waiting_for_key) {
        al_draw_text(k->font, al_map_rgb(255,0,0),
                     k->x, k->y + KB_MENU_COUNT*50 + 20,
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
