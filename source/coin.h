#pragma once

// coin.h

#ifndef __COIN_H__
#define __COIN_H__

class Sprite;
class Renderer;

class Coin
{
public:
    Coin();
    ~Coin();

    bool Initialise(Renderer& renderer, float x, float y);
    void Process(float deltaTime);
    void Draw(Renderer& renderer, float scrollDistance);

    void SetPosition(float x, float y);
    float GetX() const;
    float GetY() const;

    void Collect();
    bool IsCollected() const;

private:
    Sprite* m_pSprite;
    float m_X;
    float m_Y;
    bool m_collected;
};

#endif // __COIN_H__
