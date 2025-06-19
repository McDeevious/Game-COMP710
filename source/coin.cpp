// coin.cpp

#include "coin.h"
#include "texture.h"
#include "sprite.h"
#include "renderer.h"
#include "logmanager.h"
#include <cassert>

Coin::Coin()
    : m_pSprite(nullptr)
    , m_X(0.0f)
    , m_Y(0.0f)
    , m_collected(false)
{
}

Coin::~Coin()
{
    delete m_pSprite;
    m_pSprite = nullptr;
}

bool Coin::Initialise(Renderer& renderer, float x, float y)
{
    // TEMP: Use known good image like the knight sprite
    m_pSprite = renderer.CreateSprite("../game/assets/Sprites/Coin.png");
   
    if (m_pSprite == nullptr)
    {
        LogManager::GetInstance().Log("Failed to create test sprite for coin.");
        return false;
    }

    m_X = x;
    m_Y = y;
    m_pSprite->SetX(m_X);
    m_pSprite->SetY(m_Y);
    m_pSprite->SetScale(1.0f, 1.0f);
    return true;
}

void Coin::Process(float deltaTime)
{
    // You can animate or add logic here if needed
}

void Coin::Draw(Renderer& renderer, float scrollDistance)
{
    if (!m_collected && m_pSprite != nullptr)
    {
        m_pSprite->SetX(m_X - scrollDistance);  // adjust X for camera scroll
        m_pSprite->SetY(m_Y);
        m_pSprite->Draw(renderer);
    }
}

void Coin::SetPosition(float x, float y)
{
    m_X = x;
    m_Y = y;
    if (m_pSprite)
    {
        m_pSprite->SetX(x);
        m_pSprite->SetY(y);
    }
}

float Coin::GetX() const
{
    return m_X;
}

float Coin::GetY() const
{
    return m_Y;
}

void Coin::Collect()
{
    m_collected = true;
}

bool Coin::IsCollected() const
{
    return m_collected;
}
