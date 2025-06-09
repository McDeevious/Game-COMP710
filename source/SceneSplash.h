#ifndef SCENESPLASH_H
#define SCENESPLASH_H

#include "scene.h"

// Forward declarations
class Sprite;
class Renderer;

class SceneSplash : public Scene
{
public:
    SceneSplash();
    ~SceneSplash();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;
    void Draw(Renderer& renderer) override;
    bool IsFinished() const;
    int GetStage() const; 
    float GetAlpha() const;

private:
    Sprite* m_pAUTSprite;
    Sprite* m_pFMODSprite;
    float m_alpha;
    float m_timer;
    bool m_fadingIn;
    bool m_finished;
    int m_stage;  
};

#endif // SCENESPLASH_H


