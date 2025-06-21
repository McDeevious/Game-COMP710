#include "buffscene.h"
#include <cstdlib>  // for rand()
#include <ctime>
#include "logmanager.h"

BuffScene::BuffScene()
    : m_active(false)
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
        "../game/assets/Sprites/Item_Buff/buff_health.png", 
        "../game/assets/Sprites/Item_Buff/buff_damage.png", 
        "../game/assets/Sprites/Item_Buff/buff_defense.png", 
        "../game/assets/Sprites/Item_Buff/buff_speed.png", 
        "../game/assets/Sprites/Item_Buff/buff_jump.png", 
        "../game/assets/Sprites/Item_Buff/buff_regen.png"
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

    for (BuffSprite* bs : m_buffOptions)
    {
        if (bs)
            bs->Draw(renderer);
    }
}

void BuffScene::ProcessInput(InputSystem& inputSystem)
{
    if (!m_active) return;

    if (inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_PRESSED)
    {
        m_chosenBuff = m_buffOptions[0]->data;
        Hide();
    }
    else if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_PRESSED)
    {
        m_chosenBuff = m_buffOptions[1]->data;
        Hide();
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
