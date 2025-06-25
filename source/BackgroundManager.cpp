#include "BackgroundManager.h"
#include "renderer.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "logmanager.h"
#include "area.h"
#include "sharedenums.h"

#include <cmath>

BackgroundManager::BackgroundManager()
    : m_pBackgroundBack(nullptr)
    , m_pBackgroundMiddle(nullptr)
    , m_pForestBack(nullptr)
    , m_pForestMiddle(nullptr)
    , m_pMushroomBack(nullptr)
    , m_pMushroomMiddle(nullptr)
    , m_backScrollX(0.0f)
    , m_middleScrollX(0.0f)
    , m_tilesScrollX(0.0f)
    , m_backScrollSpeed(60.0f)    // Slowest (furthest background)
    , m_middleScrollSpeed(90.0f)  // Medium speed (middle layer)
    , m_tilesScrollSpeed(120.0f)   // Fastest (foreground tiles)
    , gameLevel(0)
{
}

BackgroundManager::~BackgroundManager()
{
    delete m_pBackgroundBack;
    m_pBackgroundBack = nullptr;

    delete m_pBackgroundMiddle;
    m_pBackgroundMiddle = nullptr;

    delete m_pForestBack;
    m_pForestBack = nullptr;

    delete m_pForestMiddle;
    m_pForestMiddle = nullptr;

    delete m_pMushroomBack;
    m_pMushroomBack = nullptr;

    delete m_pMushroomMiddle;
    m_pMushroomMiddle = nullptr;
}

bool BackgroundManager::Initialise(Renderer& renderer)
{
    // Load background layers
    int screenW = renderer.GetWidth();
    int screenH = renderer.GetHeight();

  //  int screenH = renderer.GetHeight();
    //  gameLevel = 0;
    gameLevel = new area();
    gameLevel->Initialise(renderer);

    //just for now
    gameLevel->setLevel(renderer, 1);
    gameLevel->setScene(0);
    //getAreaArray();
    m_pForestBack = renderer.CreateSprite("../game/assets/Sprites/Forest/Layers/back.png");
    //m_pBackgroundBack = renderer.CreateSprite("../game/assets/Sprites/Backgrounds/Mushroom Forest/Layers/back.png");
    // Position and scale background layers to ensure full coverage
    if (m_pForestBack)
    {
        float backW = static_cast<float>(m_pForestBack->GetWidth());

        // Calculate scale needed to fully cover screen height with a bit extra
        float scaleY = (screenH / backW) * 1.1f;
        float scaleX = scaleY * 1.5f;

        m_pForestBack->SetX(0);
        m_pForestBack->SetY(screenH / 2);
        m_pForestBack->SetScale(scaleX, -scaleY);
    }

    m_pForestMiddle = renderer.CreateSprite("../game/assets/Sprites/Forest/Layers/middle.png");
    //m_pBackgroundMiddle = renderer.CreateSprite("../game/assets/Sprites/Backgrounds/Mushroom Forest/Layers/middle.png");
    if (m_pForestMiddle)
    {
        float midH = static_cast<float>(m_pForestMiddle->GetHeight());

        // Calculate scale needed to cover most of screen height
        float scaleY = (screenH / midH) * 1.0f;
        float scaleX = scaleY * 1.2f;

        m_pForestMiddle->SetX(0);
        m_pForestMiddle->SetY(screenH / 2);
        m_pForestMiddle->SetScale(scaleX, -scaleY);
    }

    // Set up mushroom sprites
    m_pMushroomBack = renderer.CreateSprite("../game/assets/Sprites/Backgrounds/Mushroom Forest/Layers/back.png");
    // Position and scale background layers to ensure full coverage
    if (m_pMushroomBack)
    {
        float backW = static_cast<float>(m_pMushroomBack->GetWidth());

        // Calculate scale needed to fully cover screen height with a bit extra
        float scaleY = (screenH / backW) * 1.1f;
        float scaleX = scaleY * 1.5f;

        m_pMushroomBack->SetX(0);
        m_pMushroomBack->SetY(screenH / 2);
        m_pMushroomBack->SetScale(scaleX, -scaleY);
    }

    m_pMushroomMiddle = renderer.CreateSprite("../game/assets/Sprites/Backgrounds/Mushroom Forest/Layers/middle.png");
    if (m_pMushroomMiddle)
    {
        float midH = static_cast<float>(m_pMushroomMiddle->GetHeight());

        // Calculate scale needed to cover most of screen height
        float scaleY = (screenH / midH) * 1.0f;
        float scaleX = scaleY * 1.2f;

        m_pMushroomMiddle->SetX(0);
        m_pMushroomMiddle->SetY(screenH / 2);
        m_pMushroomMiddle->SetScale(scaleX, -scaleY);
    }

    // First level bg
    m_pBackgroundBack = m_pForestBack;
    m_pBackgroundMiddle = m_pForestMiddle;

    getAreaArray();
    return true;
}


void BackgroundManager::getAreaArray()
{
    for (int y = 0; y < 40; y++)
    {
        int* temp = gameLevel->tilearray(y);
        for (int x = 0; x < 120; x++)
        {
            areaArray[x][y] = temp[x];
        }
        delete[] temp;
    }
}
int* BackgroundManager::tilearray(int row)
{
    int* array = new int[120];
    for (int i = 0; i < 120; i++)
    {
        if (areaArray[i][row] != NULL)
        {
            array[i] = areaArray[i][row];
        }
        else
        {
            array[i] = -1;
        }
    }
    return array;
}
float BackgroundManager::getSize()
{
    return gameLevel->getSize();
}
float BackgroundManager::getWide()
{
    return gameLevel->getWide();
}
float BackgroundManager::getHeight()
{
    return gameLevel->getHeight();
}
float BackgroundManager::getWH()
{
    return gameLevel->getWH();
}
void BackgroundManager::Process(float deltaTime)
{
      gameLevel->Process(deltaTime);
}

void BackgroundManager::Draw(Renderer& renderer)
{
    // Draw background layers in order (back to front) with parallax scrolling
    if (m_pBackgroundBack)
    {
        DrawLayer(renderer, m_pBackgroundBack, m_backScrollX);
    }

    if (m_pBackgroundMiddle)
    {
        DrawLayer(renderer, m_pBackgroundMiddle, m_middleScrollX);
    }

    gameLevel->Draw(renderer);
}
void BackgroundManager::changePos(float x, float y)
{
    gameLevel->changePos(x, y);
    m_position.x += x;
    m_position.y += y;
}
int BackgroundManager::getStage()
{
    return gameLevel->getStage(); 
}
void BackgroundManager::setStage(int s)
{
    gameLevel->setStage(s);
}

float BackgroundManager::getOffsetX()
{
    return gameLevel->getOffsetX();
}
void BackgroundManager::DrawLayer(Renderer& renderer, Sprite* layer, float scrollX)
{
    if (!layer) return;

    // Save original position to restore later
    float originalX = layer->GetX();
    float originalY = layer->GetY();

    // Screen dimensions
    float screenW = static_cast<float>(renderer.GetWidth());

    // Get scaled width of the layer
    float scaledW = static_cast<float>(layer->GetWidth()); // Already includes scaling

    // Calculate scroll offset
    float offsetX = fmodf(scrollX, scaledW);
    if (offsetX > 0.0f)
    {
        offsetX -= scaledW; // wrap backwards
    }

    float startX = offsetX - scaledW;

    // Calculate how many copies are needed to fully cover the screen
    int numCopies = static_cast<int>(ceil(screenW / scaledW)) + 3;

    for (int i = 0; i < numCopies; ++i)
    {
        float posX = startX + (i * scaledW);
        layer->SetX(posX);
        layer->SetY(originalY);
        layer->Draw(renderer);
    }

    // Restore original position
    layer->SetX(originalX);
    layer->SetY(originalY);
}

void BackgroundManager::AdjustBackLayerPosition(int offsetX, int offsetY)
{
    if (m_pBackgroundBack)
    {
        int currentX = m_pBackgroundBack->GetX();
        int currentY = m_pBackgroundBack->GetY();
        m_pBackgroundBack->SetX(currentX + offsetX);
        m_pBackgroundBack->SetY(currentY + offsetY);
    }
}

void BackgroundManager::AdjustMiddleLayerPosition(int offsetX, int offsetY)
{
    if (m_pBackgroundMiddle)
    {
        int currentX = m_pBackgroundMiddle->GetX();
        int currentY = m_pBackgroundMiddle->GetY();
        m_pBackgroundMiddle->SetX(currentX + offsetX);
        m_pBackgroundMiddle->SetY(currentY + offsetY);
    }
}

void BackgroundManager::SetBackLayerScale(float scaleX, float scaleY)
{
    if (m_pBackgroundBack)
    {
        m_pBackgroundBack->SetScale(scaleX, scaleY);
    }
}

void BackgroundManager::SetMiddleLayerScale(float scaleX, float scaleY)
{
    if (m_pBackgroundMiddle)
    {
        m_pBackgroundMiddle->SetScale(scaleX, scaleY);
    }
}

void BackgroundManager::UpdateScrollFromCharacterMovement(const Vector2& movementDirection, float deltaTime)
{
    // Invert the direction for proper parallax effect
    float directionX = -movementDirection.x;

    // This ensures consistent scrolling regardless of framerate
    m_backScrollX += directionX * m_backScrollSpeed * deltaTime;
    m_middleScrollX += directionX * m_middleScrollSpeed * deltaTime;
    m_tilesScrollX += directionX * m_tilesScrollSpeed * deltaTime;
}

void BackgroundManager::ChangeBackgrounds(BackgroundType backgroundType)
{
    if (backgroundType == FOREST) 
    {
        m_pBackgroundBack = m_pForestBack;
        m_pBackgroundMiddle = m_pForestMiddle;
    }
    else if (backgroundType == MUSHROOM_FOREST)
    {
        m_pBackgroundBack = m_pMushroomBack;
        m_pBackgroundMiddle = m_pMushroomMiddle;
    }
}