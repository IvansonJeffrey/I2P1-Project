#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "menu.h"
#include <stdbool.h>
#include <stdlib.h>
#include "sceneManager.h"
#include "../global.h"

static const char *menu_items[] = {
    "Start Game",
    "Options",
    "Exit"
};

#define MENU_COUNT (sizeof(menu_items)/sizeof(menu_items[0]))
#define MENU_SPACING 50

/*
   [Menu function]
*/
Scene *New_Menu(int label)
{
    Menu *m = malloc(sizeof(Menu));
    Scene *pObj = New_Scene(label);
    // setting derived object member
    m->font = al_load_ttf_font("assets/font/pirulen.ttf", 12, 0);
    // Load sound
    m->song = al_load_sample("assets/sound/menu.mp3");
    al_reserve_samples(20);
    m->sample_instance = al_create_sample_instance(m->song);


    m->title_x = WIDTH / 2;
    m->title_y = HEIGHT / 2- 50;
    m->highlight_y = (float)m->title_y;
    m->selected = 0;
    m->up_prev = false;
    m->down_prev = false;

    // Loop the song until the display closes
    al_set_sample_instance_playmode(m->sample_instance, ALLEGRO_PLAYMODE_LOOP);
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer(m->sample_instance, al_get_default_mixer());

    // set the volume of instance
    al_set_sample_instance_gain(m->sample_instance, 0.1);
    pObj->pDerivedObj = m;
    // setting derived object function
    pObj->Update = menu_update;
    pObj->Draw = menu_draw;
    pObj->Destroy = menu_destroy;
    return pObj;
}
void menu_update(Scene *self)
{
    Menu *m = (Menu*)self->pDerivedObj;
    
    float target_y = m->title_y + m->selected * MENU_SPACING;
    m->highlight_y += (target_y - m->highlight_y) * 0.2f;

    bool up = key_state[ALLEGRO_KEY_UP];
    bool down = key_state[ALLEGRO_KEY_DOWN];

    if (up && !m->up_prev) {
        m->selected = (m->selected + MENU_COUNT - 1) % MENU_COUNT;
    }

    if (down && !m->down_prev) {
        m->selected = (m->selected + 1) % MENU_COUNT;
    }

    m->up_prev = up;
    m->down_prev = down;
    
    if (key_state[ALLEGRO_KEY_ENTER]) {
        self->scene_end = true;
        switch (m->selected) {
          case 0: window = GameScene_L;   break;
          case 1: window = OptionsScene_L; break;
          case 2: window = Exit_L; break;
        }
    }
}
void menu_draw(Scene *self)
{
    Menu *m = (Menu*)self->pDerivedObj;
    al_clear_to_color(al_map_rgb(0,0,0));
    al_play_sample_instance(m->sample_instance);

    for (int i = 0; i < MENU_COUNT; i++) {
        ALLEGRO_COLOR col = (i == m->selected)
          ? al_map_rgb(255,255,0)
          : al_map_rgb(200,200,200);

        al_draw_text(m->font, col,
                     m->title_x,
                     m->title_y + i * 50,
                     ALLEGRO_ALIGN_CENTRE,
                     menu_items[i]);
    }
    // optional: draw a box around the selected item
    float y = m->highlight_y;
    al_draw_rectangle(
    m->title_x - 120, 
    y - 10,
    m->title_x + 120,
    y + 30,
    al_map_rgb(255,255,255), 2
    );

    // then draw text (you can center based on each item’s fixed position,
    // or even lerp the text color similarly if you want a fade effect)
    for(int i=0;i<MENU_COUNT;i++)
    {
        float item_y = m->title_y + i*MENU_SPACING;
        ALLEGRO_COLOR col = (fabs(item_y - m->highlight_y)<1.0f)
            ? al_map_rgb(255,255,0) 
            : al_map_rgb(200,200,200);
        al_draw_text(m->font, col,
                    m->title_x,
                    item_y,
                    ALLEGRO_ALIGN_CENTRE,
                    menu_items[i]);
    }
}
void menu_destroy(Scene *self)
{
    Menu *Obj = ((Menu *)(self->pDerivedObj));
    al_destroy_font(Obj->font);
    al_destroy_sample(Obj->song);
    al_destroy_sample_instance(Obj->sample_instance);
    free(Obj);
    free(self);
}
