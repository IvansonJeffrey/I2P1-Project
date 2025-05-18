#include "sceneManager.h"
#include "menu.h"
#include "gamescene.h"
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
        scene = New_GameScene(GameScene_L);
        break;
    case OptionsScene_L:
        scene = New_OptionsScene(OptionsScene_L);
        break;
    case KeyBindScene_L:
        scene = New_KeyBindScene(type);
        break;
    case Exit_L:
        break;
    default:
        break;
    }
}
