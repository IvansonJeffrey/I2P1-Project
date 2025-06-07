#include "GameWindow.h"
#include "GAME_ASSERT.h"
#include "global.h"
#include "shapes/Shape.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "scene/sceneManager.h"
#include "scene/keybindscene.h"
#include <stdbool.h>
#include "scene/bats.h"

static bool cheat_speed_active = false;
static float cheat_original_speed = 0.0f;

typedef enum {
    STATE_MAIN_MENU,
    STATE_PLAYING,
    STATE_OPTIONS,
    STATE_GAMEOVER
} GameState;

static GameState game_state = STATE_MAIN_MENU;
static bool music_muted = false;
static int options_cursor = 0;
static const int num_options = 3;
static const char* options_text[3] = {
    "Resume",
    "Music",
    "Exit"
};

static float music_volume = 1.0f;

static bool  slider_dragging = false;


static ALLEGRO_FONT *menu_font = NULL;

Game *New_Game()
{
    Game *game = (Game *)malloc(sizeof(Game));
    game->execute = execute;
    game->game_init = game_init;
    game->game_update = game_update;
    game->game_draw = game_draw;
    game->game_destroy = game_destroy;
    game->title = "Final Project 10xxxxxxx";
    game->display = NULL;
    game->game_init(game);
    return game;
}

void execute(Game *self)
{
    // main game loop
    bool run = true;
    while (run)
    {
        al_wait_for_event(event_queue, &event);
        switch (event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if (game_state == STATE_PLAYING) {
                    run &= self->game_update(self);
                }
                // Always redraw (so the options overlay still shows)
                self->game_draw(self);
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // stop game
                run = false;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key_state[event.keyboard.keycode] = true;

                // → Enter pause/options
                if (game_state == STATE_PLAYING &&
                    event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    game_state     = STATE_OPTIONS;
                    options_cursor = 0;
                    break;
                }
                // → Navigation & actions in options
                else if (game_state == STATE_OPTIONS)
                {
                    switch (event.keyboard.keycode)
                    {
                        // Move cursor up
                        case ALLEGRO_KEY_UP:
                        case ALLEGRO_KEY_W:
                            options_cursor =
                                (options_cursor + num_options - 1)
                                % num_options;
                            break;

                        // Move cursor down
                        case ALLEGRO_KEY_DOWN:
                        case ALLEGRO_KEY_S:
                            options_cursor =
                                (options_cursor + 1)
                                % num_options;
                            break;

                        // Select / toggle
                        case ALLEGRO_KEY_ENTER:
                        case ALLEGRO_KEY_PAD_ENTER:
                            if (options_cursor == 0) {
                                // Resume
                                game_state = STATE_PLAYING;
                            }
                            else if (options_cursor == 1) {
                                // Mute/unmute toggle
                                if (music_volume > 0.0f) {
                                    music_volume = 0.0f;
                                } else {
                                    music_volume = 1.0f;
                                }
                                al_set_mixer_gain(
                                    al_get_default_mixer(),
                                    music_volume
                                );
                            }
                            else {
                                // Exit to main menu
                                game_state = STATE_MAIN_MENU;
                            }
                            break;

                        // Nudge slider left
                        case ALLEGRO_KEY_LEFT:
                        case ALLEGRO_KEY_A:
                            if (options_cursor == 1) {
                                music_volume -= 0.05f;
                                if (music_volume < 0.0f)
                                    music_volume = 0.0f;
                                al_set_mixer_gain(
                                    al_get_default_mixer(),
                                    music_volume
                                );
                            }
                            break;

                        // Nudge slider right
                        case ALLEGRO_KEY_RIGHT:
                        case ALLEGRO_KEY_D:
                            if (options_cursor == 1) {
                                music_volume += 0.05f;
                                if (music_volume > 1.0f)
                                    music_volume = 1.0f;
                                al_set_mixer_gain(
                                    al_get_default_mixer(),
                                    music_volume
                                );
                            }
                            break;

                        // Back out = resume
                        case ALLEGRO_KEY_ESCAPE:
                            game_state = STATE_PLAYING;
                            break;

                        default:
                            break;
                    }
                    break;
                }

                // ── Your existing cheat‐key handlers ──
                if (event.keyboard.keycode == ALLEGRO_KEY_C) {
                    player_invincible = !player_invincible;
                    printf("*** Cheat: Invincibility %s ***\n",
                           player_invincible ? "ON" : "OFF");
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_QUOTE) {
                    printf("*** Cheat: Spawning 150 bats around you ***\n");
                    bats_cheat_spawn(150);
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_F) {
                    cheat_speed_active = !cheat_speed_active;
                    if (cheat_speed_active) {
                        cheat_original_speed = player_speed;
                        player_speed = cheat_original_speed * 2.0f;
                    } else {
                        player_speed = cheat_original_speed;
                    }
                    printf("*** Cheat: Speed %s (%.0f → %.0f) ***\n",
                           cheat_speed_active ? "ON" : "OFF",
                           cheat_original_speed,
                           player_speed);
                }

                // ── KeyBindScene handling ──
                if (window == KeyBindScene_L) {
                    KeyBindScene *k = (KeyBindScene*)scene->pDerivedObj;
                    if (k->waiting_for_key) {
                        k->keymap[k->selected] =
                            event.keyboard.keycode;
                        k->waiting_for_key = false;
                        break;
                    }
                }
                break;

            case ALLEGRO_EVENT_KEY_UP:
                key_state[event.keyboard.keycode] = false;
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                if (slider_dragging) {
                    int mx = event.mouse.x;
                    // same slider bounds calc as above
                    const float panel_w = 400, panel_h = 240;
                    float panel_x = (WIDTH - panel_w)/2;
                    float panel_y = (HEIGHT - panel_h)/2;
                    const float line_h = 50;
                    float y1 = panel_y + 60 + line_h;

                    const char *label = "Music";
                    float lx = panel_x + 80;
                    float label_w = al_get_text_width(menu_font, label);
                    const float spacing = 20;
                    const float sw = 120;
                    float sx = lx + label_w + spacing;

                    float t = (mx - sx) / sw;
                    music_volume = t < 0 ? 0 : t > 1 ? 1 : t;
                    al_set_mixer_gain(
                        al_get_default_mixer(),
                        music_volume
                    );
                }
                mouse.x = event.mouse.x;
                mouse.y = event.mouse.y;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (game_state == STATE_OPTIONS &&
                    event.mouse.button == 1)
                {
                    int mx = event.mouse.x;
                    int my = event.mouse.y;

                    // Panel & rows metrics
                    const float panel_w = 400, panel_h = 240;
                    float panel_x = (WIDTH - panel_w)/2;
                    float panel_y = (HEIGHT - panel_h)/2;
                    const float line_h = 50;
                    float y0 = panel_y + 60;
                    float y1 = y0 + line_h;
                    float y2 = y1 + line_h;

                    // 1) Resume row
                    if (mx >= panel_x && mx <= panel_x + panel_w &&
                        my >= y0 - line_h/2 &&
                        my <= y0 + line_h/2)
                    {
                        options_cursor = 0;
                        game_state = STATE_PLAYING;
                        break;
                    }

                    // 2) Music row (toggle or drag)
                    if (mx >= panel_x && mx <= panel_x + panel_w &&
                        my >= y1 - line_h/2 &&
                        my <= y1 + line_h/2)
                    {
                        options_cursor = 1;

                        // Slider bounds
                        const char *label = "Music";
                        float lx = panel_x + 80;
                        float label_w = al_get_text_width(menu_font, label);
                        const float spacing = 20;
                        const float sw = 120, sh = 16;
                        float sx = lx + label_w + spacing;
                        float sy =
                          y1 + (al_get_font_line_height(menu_font)/2.0f)
                          - (sh/2.0f);

                        // Click inside track → begin drag
                        if (mx >= sx && mx <= sx + sw &&
                            my >= sy && my <= sy + sh)
                        {
                            slider_dragging = true;
                            float t = (mx - sx) / sw;
                            music_volume = t < 0 ? 0 : t > 1 ? 1 : t;
                            al_set_mixer_gain(
                              al_get_default_mixer(),
                              music_volume
                            );
                        }
                        else {
                            // Click in row → toggle mute
                            if (music_volume > 0.0f)
                                music_volume = 0.0f;
                            else
                                music_volume = 1.0f;
                            al_set_mixer_gain(
                              al_get_default_mixer(),
                              music_volume
                            );
                        }
                        break;
                    }

                    // 3) Exit row
                    if (mx >= panel_x && mx <= panel_x + panel_w &&
                        my >= y2 - line_h/2 &&
                        my <= y2 + line_h/2)
                    {
                        options_cursor = 2;
                        game_state = STATE_MAIN_MENU;
                        break;
                    }
                }
                // … any other mouse‐down logic …
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                if (slider_dragging &&
                    event.mouse.button == 1)
                {
                    slider_dragging = false;
                }
                mouse_state[event.mouse.button] = false;
                break;

            default:
                break;
        }
    }
}

void game_init(Game *self)
{
    printf("Game Initializing...\n");
    GAME_ASSERT(al_init(), "failed to initialize allegro.");
    // initialize allegro addons
    bool addon_init = true;
    addon_init &= al_init_primitives_addon();
    addon_init &= al_init_font_addon();   // initialize the font addon
    addon_init &= al_init_ttf_addon();    // initialize the ttf (True Type Font) addon

    health_font = al_load_ttf_font("assets/font/PixelPurl.ttf", 40, 0);
    GAME_ASSERT(health_font, "Failed to load health font at assets/font/PixelPurl.ttf\n");

    menu_font = al_load_ttf_font("assets/font/PixelPurl.ttf", 28, 0);
    GAME_ASSERT(menu_font, "Failed to load menu font at assets/font/PixelPurl.ttf");    

    addon_init &= al_init_image_addon();  // initialize the image addon
    addon_init &= al_init_acodec_addon(); // initialize acodec addon
    addon_init &= al_install_keyboard();  // install keyboard event
    addon_init &= al_install_mouse();     // install mouse event
    addon_init &= al_install_audio();     // install audio event
    GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

    al_reserve_samples(1);

    // Load your BGM file (adjust path as needed)
    ALLEGRO_SAMPLE *bgm_sample = al_load_sample("assets/sound/stagebgm.mp3");
    GAME_ASSERT(bgm_sample,
        "Failed to load BGM: assets/audio/stagebgm.mp3");

    // Start playing in a loop (volume=1.0, pan=0.0, speed=1.0)
    al_play_sample(bgm_sample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_LOOP, NULL);  

    // Create display
    self->display = al_create_display(WIDTH, HEIGHT);
    GAME_ASSERT(self->display, "failed to create display.");
    display = self->display;

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    // Create first scene
    create_scene(GameScene_L);

    game_state = STATE_PLAYING;

    // create event queue
    event_queue = al_create_event_queue();
    GAME_ASSERT(event_queue, "failed to create event queue.");
    // Initialize Allegro settings

    ALLEGRO_MONITOR_INFO mi;
    al_get_monitor_info(0, &mi);  

    int screen_w = mi.x2 - mi.x1;
    int screen_h = mi.y2 - mi.y1;

    int win_x = mi.x1 + (screen_w  - WIDTH)  / 2;
    int win_y = mi.y1 + (screen_h - HEIGHT) / 2;
    al_set_window_position(self->display, win_x, win_y);

    //al_set_window_position(self->display, 0, 0);
    al_set_window_title(self->display, self->title);
    // Register event
    al_register_event_source(event_queue, al_get_display_event_source(self->display)); // register display event
    al_register_event_source(event_queue, al_get_keyboard_event_source());             // register keyboard event
    al_register_event_source(event_queue, al_get_mouse_event_source());                // register mouse event
    // register timer event
    fps = al_create_timer(1.0 / FPS);
    al_register_event_source(event_queue, al_get_timer_event_source(fps));
    al_start_timer(fps);
    // initialize the icon on the display
    ALLEGRO_BITMAP *icon = al_load_bitmap("assets/image/icon.jpg");
    al_set_display_icon(self->display, icon);
}

bool game_update(Game *self)
{
    scene->Update(scene);
    if (scene->scene_end)
    {
        scene->Destroy(scene);
        switch (window)
        {
        case Menu_L:
            create_scene(Menu_L);
            break;
        case GameScene_L:
            create_scene(GameScene_L);
            break;
        case OptionsScene_L:   
            create_scene(OptionsScene_L);
            break;
        case KeyBindScene_L:   
            create_scene(KeyBindScene_L);
            break;
        case Exit_L:
            return false;
        default:
            break;
        }
    }
    return true;
}

// --- game_draw() with aligned, color-changing slider ---
void game_draw(Game *self)
{
    al_clear_to_color(al_map_rgb(100,100,100));
    scene->Draw(scene);

    if (game_state == STATE_OPTIONS) {
        // 1) backdrop
        al_draw_filled_rectangle(0, 0, WIDTH, HEIGHT,
                                 al_map_rgba_f(0,0,0,0.6f));

        // 2) panel
        const float panel_w = 400, panel_h = 240;
        const float panel_x = (WIDTH - panel_w) / 2;
        const float panel_y = (HEIGHT - panel_h) / 2;
        al_draw_filled_rounded_rectangle(
            panel_x, panel_y,
            panel_x + panel_w, panel_y + panel_h,
            12, 12,
            al_map_rgb(20,20,60)
        );

        // 3) header
        al_draw_text(
            menu_font,
            al_map_rgb(255,255,255),
            panel_x + panel_w/2, panel_y + 20,
            ALLEGRO_ALIGN_CENTRE,
            "PAUSED"
        );

        // row Y positions
        const float line_h = 50;
        const float y0 = panel_y + 60;
        const float y1 = y0 + line_h;
        const float y2 = y1 + line_h;

        // 4) Resume
        {
            ALLEGRO_COLOR col = (options_cursor==0)
                ? al_map_rgb(255,200,0)
                : al_map_rgb(200,200,200);
            al_draw_text(menu_font, col,
                         panel_x + panel_w/2, y0,
                         ALLEGRO_ALIGN_CENTRE,
                         "Resume");
        }

        // 5) Music + slider
        {
            const char *label = "Music";
            ALLEGRO_COLOR labcol = (options_cursor==1)
                ? al_map_rgb(255,200,0)
                : al_map_rgb(200,200,200);
            // fixed label X
            float lx = panel_x + 100;
            float ly = y1;
            al_draw_text(menu_font, labcol, lx, ly, 0, label);

            const float sw = 60, sh = 16;
            const float right_margin = 90; 
            // slider flush-right inside the panel
            float sx = panel_x + panel_w - sw - right_margin;
            // vertical center on the text baseline
            float fh = al_get_font_line_height(menu_font);
            float sy = ly + (fh/2.0f) - (sh/2.0f);

            al_draw_filled_rounded_rectangle(
                sx, sy, sx+sw, sy+sh,
                sh/2, sh/2,
                al_map_rgb(100,100,100)
            );

            float fill_w = sw * music_volume;
            al_draw_filled_rounded_rectangle(
                sx, sy, sx+fill_w, sy+sh,
                sh/2, sh/2,
                al_map_rgb(0,150,255)
            );

            float kr = (sh/2) - 2;
            float kx = sx + music_volume * sw;
            float ky = sy + sh/2;
            al_draw_filled_circle(kx, ky, kr,
                                al_map_rgb(255,255,255));
        }
        // 6) Exit
        {
            ALLEGRO_COLOR col = (options_cursor==2)
                ? al_map_rgb(255,200,0)
                : al_map_rgb(200,200,200);
            al_draw_text(menu_font, col,
                         panel_x + panel_w/2, y2,
                         ALLEGRO_ALIGN_CENTRE,
                         "Exit");
        }
    }

    al_flip_display();
}

void game_destroy(Game *self)
{
    // Make sure you destroy all things
    al_destroy_event_queue(event_queue);
    al_destroy_display(self->display);
    scene->Destroy(scene);
    free(self);
}
