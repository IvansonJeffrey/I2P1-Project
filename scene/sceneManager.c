#include "sceneManager.h"
#include "menu.h"
#include "game.h"
#include "keybindscene.h"
#include "optionscene.h"

Scene *scene = NULL;

void create_scene(SceneType type)
{
    switch (type)
    {
        case Menu_L:
            scene = New_Menu(Menu_L);
            break;

        case GameScene_L:
            scene = I2P_NewGameScene(GameScene_L);
            break;

        case OptionsScene_L:
            scene = New_OptionsScene(OptionsScene_L);
            break;

        case Exit_L:
            break;

        default:
            break;
    }
}

void add_element_to_scene(Scene *scenePtr, Elements *ele)
{
    _Register_elements(scenePtr, ele);
}
