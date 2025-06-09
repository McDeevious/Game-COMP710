#ifndef __SCENEMUSHROOMFOREST_H_
#define __SCENEMUSHROOMFOREST_H_

#include "scene.h"
#include "inputsystem.h"
#include "vector2.h"
#include "orc.h"
#include <vector>
#include <string>
#include <fmod.hpp>

class KnightClass;
class Orc;
class Renderer;
class BackgroundManager;
class PauseMenu;
class GameOverMenu;
class AnimatedSprite;
class ArmoredOrc;
class EliteOrc;
class RiderOrc;
class KnightHUD;
class SceneGuide;

class SceneGame : public Scene
{
public:
    SceneGame();
    ~SceneGame();

    bool Initialise(Renderer& renderer);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);

protected:
    

};

#endif // __SCENEMUSHROOMFOREST_H_