#ifndef SCENEMANAGER_H_INCLUDED
#define SCENEMANAGER_H_INCLUDED
#include "scene.h"
extern Scene *scene;
typedef enum SceneType
{
    Menu_L = 0,
    GameScene_L,
    OptionsScene_L,
    KeyBindScene_L, 
    ResolutionScene_L,
    BrightnessScene_L,
    Exit_L
} SceneType;
void create_scene(SceneType);

#endif