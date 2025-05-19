#include "brightnessscene.h"
#include "../global.h"           // for window
#include "sceneManager.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdlib.h>

static const int levels[] = { 0, 25, 50, 75, 100 };
#define BR_COUNT (sizeof(levels)/sizeof(*levels))

typedef struct {
    ALLEGRO_FONT *font;
    int           x, y;
    int           selected;
    bool          up_prev, down_prev, enter_prev;
} BrightnessScene;

Scene *New_BrightnessScene(int label) {
    BrightnessScene *b = malloc(sizeof *b);
    Scene           *scn = New_Scene(label);

    b->font     = al_load_ttf_font("assets/font/pirulen.ttf", 24, 0);
    b->x        = WIDTH  / 2;
    b->y        = HEIGHT / 2 - (BR_COUNT*30);
    b->selected = 2;  // default 50%
    b->up_prev     = key_state[ALLEGRO_KEY_UP];
    b->down_prev   = key_state[ALLEGRO_KEY_DOWN];
    b->enter_prev  = key_state[ALLEGRO_KEY_ENTER];

    scn->pDerivedObj = b;
    scn->Update      = brightness_update;
    scn->Draw        = brightness_draw;
    scn->Destroy     = brightness_destroy;
    return scn;
}

void brightness_update(Scene *self) {
    BrightnessScene *b = self->pDerivedObj;
    bool up   = key_state[ALLEGRO_KEY_UP];
    bool down = key_state[ALLEGRO_KEY_DOWN];

    if (up && !b->up_prev)
        b->selected = (b->selected + BR_COUNT - 1) % BR_COUNT;
    if (down && !b->down_prev)
        b->selected = (b->selected + 1) % BR_COUNT;
    b->up_prev   = up;
    b->down_prev = down;

    bool enter = key_state[ALLEGRO_KEY_ENTER];
    if (enter && !b->enter_prev) {
        // apply brightness globally (you can use al_set_blender or a shader)
        extern int brightness;
        brightness = levels[b->selected];

        self->scene_end = true;
        window = OptionsScene_L;
    }
    b->enter_prev = enter;
}

void brightness_draw(Scene *self) {
    BrightnessScene *b = self->pDerivedObj;
    al_clear_to_color(al_map_rgb(0,0,0));

    for (int i = 0; i < BR_COUNT; i++) {
        char buf[16];
        snprintf(buf, sizeof buf, "%3d%%", levels[i]);
        int yy = b->y + i * 60;
        ALLEGRO_COLOR col = (i == b->selected)
          ? al_map_rgb(255,255,0)
          : al_map_rgb(200,200,200);
        al_draw_text(b->font, col, b->x, yy,
                     ALLEGRO_ALIGN_CENTRE, buf);
    }

    // highlight box
    int hy = b->y + b->selected * 60;
    al_draw_rectangle(b->x - 80, hy - 20,
                      b->x + 80, hy + 30,
                      al_map_rgb(255,255,255), 2);

    al_flip_display();
}

void brightness_destroy(Scene *self) {
    BrightnessScene *b = self->pDerivedObj;
    al_destroy_font(b->font);
    free(b);
    free(self);
}
