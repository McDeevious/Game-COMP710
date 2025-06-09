#ifndef SCENEGUIDE_H
#define SCENEGUIDE_H

#include "scene.h"

// Forward declarations
class Sprite;
class Renderer;

class SceneGuide: public Scene
{
public:
    SceneGuide();
    ~SceneGuide();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;
    void Draw(Renderer& renderer) override;
    bool IsFinished() const;
    float GetAlpha() const;

private:
    Sprite* m_controlGuide; 
    float m_alpha;
    float m_timer;
    bool m_fadingIn;
    bool m_finished;
};

#endif // SCENEGUIDE_H

