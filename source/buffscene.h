#pragma once

#pragma once

#ifndef __BUFFSCENE_H__
#define __BUFFSCENE_H__

#include "buff.h"
#include "inputsystem.h"
#include "renderer.h"
#include "sprite.h"
#include <vector>

struct BuffSprite
{
    Sprite* sprite;
    Buff data;

    BuffSprite() : sprite(nullptr) {}
    ~BuffSprite() { delete sprite; }

    void Draw(Renderer& renderer)
    {
        if (sprite) sprite->Draw(renderer);
    }

    void SetPosition(float x, float y)
    {
        if (sprite)
        {
            sprite->SetX(x);
            sprite->SetY(y);
        }
    }
};

class BuffScene
{
public:
    BuffScene();
    ~BuffScene();

    bool Initialise(Renderer& renderer);
    void GenerateRandomBuffs(Renderer& renderer); // Called when an orc is killed
    void Draw(Renderer& renderer);
    void ProcessInput(InputSystem& inputSystem);

    bool IsActive() const { return m_active; }
    Buff GetChosenBuff() const { return m_chosenBuff; }

    void Show();  // Activate scene
    void Hide();  // Deactivate scene

private:
    std::vector<BuffSprite*> m_buffOptions;
    bool m_active;
    Buff m_chosenBuff;
};

#endif // __BUFFSCENE_H__