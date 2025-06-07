#include "sceneManager.h"
#include "gamescene.h"
#include "../global.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

static const char *opt_items[] = {
    "Left",
    "Right",
    "Up",
    "Down",
    "Invicible",
    "Movement speed",
    "Spawn",
    "Mute Music",
    "Back"
};

static const int OPT_COUNT = sizeof(opt_items)/sizeof(*opt_items);

typedef struct {
    Scene base;
    int cursor;
    int num_options;
    float *pMusicVolume;
    bool waiting_for_key; 
    int *keymap; 
} OptionScene;

Scene *New_OptionsScene(int label) {
    OptionScene *o = calloc(1, sizeof *o);
    Scene *s = New_Scene(label);

    // hook up our virtual methods
    s->pDerivedObj = o;
    s->Update = options_update;
    s->Draw = options_draw;
    s->Destroy = options_destroy;

    o->cursor = 0;
    o->num_options = sizeof(opt_items)/sizeof(*opt_items);
    o->waiting_for_key = false;
    o->keymap = keymap;            // assume int keymap[] is declared extern in global.h
    o->pMusicVolume = &music_volume;     // extern float music_volume in global.c

    return s;
}


void options_update(Scene *self) {
    OptionScene *o = (OptionScene*)self->pDerivedObj;
    ALLEGRO_EVENT *ev = &event;

    if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (ev->keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                o->cursor = (o->cursor + o->num_options - 1) % o->num_options;
                break;
            case ALLEGRO_KEY_DOWN:
                o->cursor = (o->cursor + 1) % o->num_options;
                break;
            case ALLEGRO_KEY_ENTER:
            case ALLEGRO_KEY_PAD_ENTER:
                if (o->cursor == 0) {
                    // → jump into key-binding scene
                    window          = KeyBindScene_L;
                    self->scene_end = true;
                }
                else if (o->cursor == 1) {
                    // → toggle music
                    *o->pMusicVolume = (*o->pMusicVolume > 0.0f ? 0.0f : 1.0f);
                    al_set_mixer_gain(al_get_default_mixer(), *o->pMusicVolume);
                }
                else { // cursor == 2
                    // → back to main menu
                    window          = Menu_L;
                    self->scene_end = true;
                }
                break;
            case ALLEGRO_KEY_ESCAPE:
                window          = Menu_L;
                self->scene_end = true;
                break;
        }
    }
}


void options_draw(Scene *self) {
    OptionScene *o = (OptionScene*)self->pDerivedObj;
    al_clear_to_color(al_map_rgb(20,20,50));

    // draw header
    al_draw_text(menu_font, al_map_rgb(255,215,0),
                 WIDTH/2,  80, ALLEGRO_ALIGN_CENTRE,
                 "OPTIONS");

    // draw each entry
    char buf[64];
    for (int i = 0; i < o->num_options; i++) {
        // guard the label array
        const char *label = (i < sizeof(opt_items)/sizeof(*opt_items))
                            ? opt_items[i]
                            : "(??)";

        ALLEGRO_COLOR col = (i == o->cursor)
            ? al_map_rgb(255,200,0)
            : al_map_rgb(200,200,200);

        if (i < 7) {
            // key‐bind row
            const char *kn = al_keycode_to_name(o->keymap[i]);
            if (!kn) kn = "(unknown)";
            snprintf(buf, sizeof buf, "%-15s %s", label, kn);
            al_draw_text(menu_font, col,
                         WIDTH/2, 150 + i*40,
                         ALLEGRO_ALIGN_CENTRE,
                         buf);
        }
        else if (i == 7) {
            // music toggle row
            snprintf(buf, sizeof buf,
                     "Mute Music: %s",
                     *o->pMusicVolume > 0 ? "On" : "Off");
            al_draw_text(menu_font, col,
                         WIDTH/2, 150 + i*40,
                         ALLEGRO_ALIGN_CENTRE,
                         buf);
        }
        else {
            // Back row
            al_draw_text(menu_font, col,
                         WIDTH/2, 150 + i*40,
                         ALLEGRO_ALIGN_CENTRE,
                         label);
        }
    }
    al_flip_display();
}



void options_destroy(Scene *self) {
    free(self->pDerivedObj);
    free(self);
}

