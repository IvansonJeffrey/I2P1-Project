#include "resolutionscene.h"
#include "../global.h"           // for WIDTH, HEIGHT, window
#include "sceneManager.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdlib.h>

static const struct { int w, h; const char *name; } modes[] = {
    { 800, 600,  "800×600" },
    {1024, 768, "1024×768"},
    {1280, 720, "1280×720"},
    {1920,1080, "1920×1080"}
};
#define MODE_COUNT (sizeof(modes)/sizeof(*modes))

typedef struct {
    ALLEGRO_FONT *font;
    int           x, y;
    int           selected;
    bool          up_prev, down_prev, enter_prev;
} ResolutionScene;

Scene *New_ResolutionScene(int label) {
    ResolutionScene *r = malloc(sizeof *r);
    Scene           *scn = New_Scene(label);

    r->font     = al_load_ttf_font("assets/font/pirulen.ttf", 24, 0);
    r->x        = WIDTH  / 2;
    r->y        = HEIGHT / 2 - (MODE_COUNT*30);
    r->selected = 0;
    r->up_prev  = key_state[ALLEGRO_KEY_UP];
    r->down_prev= key_state[ALLEGRO_KEY_DOWN];
    r->enter_prev = key_state[ALLEGRO_KEY_ENTER];

    scn->pDerivedObj = r;
    scn->Update      = resolution_update;
    scn->Draw        = resolution_draw;
    scn->Destroy     = resolution_destroy;
    return scn;
}

void resolution_update(Scene *self) {
    ResolutionScene *r = self->pDerivedObj;
    bool up   = key_state[ALLEGRO_KEY_UP];
    bool down = key_state[ALLEGRO_KEY_DOWN];

    if (up && !r->up_prev)
        r->selected = (r->selected + MODE_COUNT - 1) % MODE_COUNT;
    if (down && !r->down_prev)
        r->selected = (r->selected + 1) % MODE_COUNT;
    r->up_prev   = up;
    r->down_prev = down;

    bool enter = key_state[ALLEGRO_KEY_ENTER];
    if (enter && !r->enter_prev) {
        // apply resolution
        extern ALLEGRO_DISPLAY *display;
        al_resize_display(display,
                          modes[r->selected].w,
                          modes[r->selected].h);
        // update globals if you use them:
        //screen_width = modes[r->selected].w;
        //screen_height= modes[r->selected].h;

        self->scene_end = true;
        window = OptionsScene_L;
    }
    r->enter_prev = enter;
}

void resolution_draw(Scene *self) {
    ResolutionScene *r = self->pDerivedObj;
    al_clear_to_color(al_map_rgb(0,0,0));

    for (int i = 0; i < MODE_COUNT; i++) {
        int yy = r->y + i * 60;
        ALLEGRO_COLOR col = (i == r->selected)
          ? al_map_rgb(255,255,0)
          : al_map_rgb(200,200,200);
        al_draw_text(r->font, col, r->x, yy,
                     ALLEGRO_ALIGN_CENTRE, modes[i].name);
    }

    // highlight box
    int hy = r->y + r->selected * 60;
    al_draw_rectangle(r->x - 150, hy - 20,
                      r->x + 150, hy + 30,
                      al_map_rgb(255,255,255), 2);

    al_flip_display();
}

void resolution_destroy(Scene *self) {
    ResolutionScene *r = self->pDerivedObj;
    al_destroy_font(r->font);
    free(r);
    free(self);
}
