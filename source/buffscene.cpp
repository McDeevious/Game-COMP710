#include "buffscene.h"
#include <cstdlib>  // for rand()
#include <ctime>
#include "logmanager.h"

#include "texture.h"
#include "sprite.h"
#include "renderer.h"

#include <SDL.h>// 

BuffScene::BuffScene()
    : m_active(false),
    m_selectedIndex(0)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed RNG
}

BuffScene::~BuffScene()
{
    for (BuffSprite* bs : m_buffOptions)
        delete bs;
    m_buffOptions.clear();
}

bool BuffScene::Initialise(Renderer& renderer)
{
    return true; // Nothing to init yet
}

void BuffScene::GenerateRandomBuffs(Renderer& renderer)
{
    m_buffOptions.clear();

    int buff1 = std::rand() % static_cast<int>(BuffType::BUFF_COUNT);
    int buff2;
    do {
        buff2 = std::rand() % static_cast<int>(BuffType::BUFF_COUNT);
    } while (buff2 == buff1);

    float centerX = renderer.GetWidth() / 2.0f;
    float centerY = renderer.GetHeight() / 2.0f;

    const char* spritePaths[] = {
        "../game/assets/Sprites/Items/DEFUP1.png", 
        "../game/assets/Sprites/Items/DMGUP1.png", 
        "../game/assets/Sprites/Items/JumpBoost1.png", 
        "../game/assets/Sprites/Items/MoreHealth1.png", 
        "../game/assets/Sprites/Items/Regen1.png", 
        "../game/assets/Sprites/Items/Speed1.png"
    };

    BuffSprite* left = new BuffSprite();
    left->sprite = renderer.CreateSprite(spritePaths[buff1]);
    if (left->sprite)
    {
        left->sprite->SetScale(1.2f, 1.2f);
        left->SetPosition(centerX - 100, centerY);
    }
    left->data = CreateBuff(static_cast<BuffType>(buff1));
    m_buffOptions.push_back(left);

    BuffSprite* right = new BuffSprite();
    right->sprite = renderer.CreateSprite(spritePaths[buff2]);
    if (right->sprite)
    {
        right->sprite->SetScale(1.2f, 1.2f);
        right->SetPosition(centerX + 100, centerY);
    }
    right->data = CreateBuff(static_cast<BuffType>(buff2));
    m_buffOptions.push_back(right);

    m_active = true;
}

void BuffScene::Draw(Renderer& renderer)
{
    if (!m_active) return;

    for (size_t i = 0; i < m_buffOptions.size(); ++i)
    {
        BuffSprite* bs = m_buffOptions[i];
        // Highlight chosen buff
        if (bs && bs->sprite)
            bs->sprite->Draw(renderer);
    }
}

void BuffScene::ProcessInput(InputSystem& inputSystem)
{
    if (!m_active) return;

    // Use number keys to select buff
    if (inputSystem.GetKeyState(SDL_SCANCODE_1) == BS_PRESSED)
    {
        m_selectedIndex = 0;
    }
    else if (inputSystem.GetKeyState(SDL_SCANCODE_2) == BS_PRESSED)
    {
        m_selectedIndex = 1;
    }

    // Confirm selection
    if (inputSystem.GetKeyState(SDL_SCANCODE_RETURN) == BS_PRESSED ||
        inputSystem.GetKeyState(SDL_SCANCODE_SPACE) == BS_PRESSED)
    {
        if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_buffOptions.size()))
        {
            m_chosenBuff = m_buffOptions[m_selectedIndex]->data;
            Hide();
        }
    }
}

void BuffScene::Show()
{
    m_active = true;
}

void BuffScene::Hide()
{
    m_active = false;
    for (BuffSprite* bs : m_buffOptions)
        delete bs;
    m_buffOptions.clear();
}
