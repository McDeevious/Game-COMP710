#include "BackgroundManager.h"
#include "renderer.h"
#include "sprite.h"
#include "logmanager.h"

#include <cmath>

BackgroundManager::BackgroundManager()
    : m_pBackgroundBack(nullptr)
    , m_pBackgroundMiddle(nullptr)
    , m_pBackgroundTileRock(nullptr)
    , m_pBackgroundTileMoss(nullptr)
    , m_backScrollX(0.0f)
    , m_middleScrollX(0.0f)
    , m_tilesScrollX(0.0f)
    , m_backScrollSpeed(60.0f)    // Slowest (furthest background)
    , m_middleScrollSpeed(90.0f)  // Medium speed (middle layer)
    , m_tilesScrollSpeed(120.0f)   // Fastest (foreground tiles)
{
}

BackgroundManager::~BackgroundManager()
{
    delete m_pBackgroundBack;
    m_pBackgroundBack = nullptr;

    delete m_pBackgroundMiddle;
    m_pBackgroundMiddle = nullptr;

    delete m_pBackgroundTileRock;
    m_pBackgroundTileRock = nullptr;

    delete m_pBackgroundTileMoss;
    m_pBackgroundTileMoss = nullptr;
}

bool BackgroundManager::Initialise(Renderer& renderer)
{
    // Load background layers
    int screenW = renderer.GetWidth();
    int screenH = renderer.GetHeight();

    m_pBackgroundBack = renderer.CreateSprite("../game/assets/Sprites/Forest/Layers/back.png");
    // Position and scale background layers to ensure full coverage
    if (m_pBackgroundBack)
    {
        float backW = static_cast<float>(m_pBackgroundBack->GetWidth());

        // Calculate scale needed to fully cover screen height with a bit extra
        float scaleY = (screenH / backW) * 1.1f; 
        float scaleX = scaleY * 1.5f; 

        m_pBackgroundBack->SetX(0);  
        m_pBackgroundBack->SetY(screenH / 2); 
        m_pBackgroundBack->SetScale(scaleX, scaleY);
    }

    m_pBackgroundMiddle = renderer.CreateSprite("../game/assets/Sprites/Forest/Layers/middle.png"); 
    if (m_pBackgroundMiddle)
    {
        float midH = static_cast<float>(m_pBackgroundMiddle->GetHeight());

        // Calculate scale needed to cover most of screen height
        float scaleY = (screenH / midH) * 1.0f; 
        float scaleX = scaleY * 1.2f; 

        m_pBackgroundMiddle->SetX(0); 
        m_pBackgroundMiddle->SetY(screenH / 2);
        m_pBackgroundMiddle->SetScale(scaleX, -scaleY); 
    }

    m_pBackgroundTileRock = renderer.CreateAnimatedSprite("../game/assets/Sprites/Forest/Layers/tile_rock.png");
    if (m_pBackgroundTileRock)
    {
        float tilesH = static_cast<float>(m_pBackgroundTileRock->GetHeight());

        // Calculate scale for ground to cover bottom portion
        float desiredHeight = screenH * 0.3f;
        float scale = desiredHeight / tilesH;

        m_pBackgroundTileRock->SetScale(scale, -scale);
        m_pBackgroundTileRock->SetX(0);  
        m_pBackgroundTileRock->SetY(screenH - (tilesH * scale) / 2 + 50.0f);  
    }

    m_pBackgroundTileMoss = renderer.CreateAnimatedSprite("../game/assets/Sprites/Forest/Layers/tile_moss.png");
    if (m_pBackgroundTileMoss)
    {
        float tilesH = static_cast<float>(m_pBackgroundTileMoss->GetHeight());

        // Calculate scale for ground to cover bottom portion
        float desiredHeight = screenH * 0.3f;
        float scale = desiredHeight / tilesH;

        m_pBackgroundTileMoss->SetScale(scale, -scale);
        m_pBackgroundTileMoss->SetX(0); 
        m_pBackgroundTileMoss->SetY(screenH - (tilesH * scale) / 2 + 50.0f);  
    }

    return true;
}

void BackgroundManager::Process(float deltaTime)
{
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

    if (m_pBackgroundTileRock && m_pBackgroundTileMoss)
    {
        DrawGround(renderer, m_tilesScrollX);
    }
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

void BackgroundManager::DrawGround(Renderer& renderer, float scrollX)
{
    if (!m_pBackgroundTileRock || !m_pBackgroundTileMoss)
        return;

    const float screenW = static_cast<float>(renderer.GetWidth());

    const float tileWidth = static_cast<float>(m_pBackgroundTileRock->GetWidth());
    const float tileScale = m_pBackgroundTileRock->GetScaleX();
    const float scaledWidth = tileWidth * tileScale;
    
    //overlap tiles to remove gaps
    const float overlapTileWidth = scaledWidth * 0.195f; 

    // calculate horizontal offset so tiles scroll smoothly in both ways
    float offsetX = scrollX;
    while (offsetX < 0.0f)  
    {
        offsetX += overlapTileWidth; 
    }

    offsetX = fmodf(scrollX, overlapTileWidth);

    offsetX -= overlapTileWidth; 

    const float startX = offsetX - overlapTileWidth * 2; // Start off-screen
    const int tileCount = static_cast<int>(ceil(screenW / overlapTileWidth)) + 10;

    const float yRock = m_pBackgroundTileRock->GetY();
    const float yMoss = m_pBackgroundTileMoss->GetY();

    // Base index used to maintain a consistent global tile pattern
    const int startIndex = static_cast<int>(floor(fabs(scrollX / overlapTileWidth)));

    for (int i = 0; i < tileCount; ++i)
    {
        const float posX = startX + i * overlapTileWidth;
        const int globalTileIndex = (startIndex + i) % 2;

        // Select which tile to draw: rock or moss
        Sprite* tile = (globalTileIndex == 0) ? m_pBackgroundTileRock : m_pBackgroundTileMoss;
        const float y = (globalTileIndex == 0) ? yRock : yMoss;

        tile->SetX(posX);
        tile->SetY(y);
        tile->Draw(renderer);
    }
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

void BackgroundManager::AdjustTilesLayerPosition(int offsetX, int offsetY)
{
    if (m_pBackgroundTileRock)
    {
        int currentX = m_pBackgroundTileRock->GetX();
        int currentY = m_pBackgroundTileRock->GetY();
        m_pBackgroundTileRock->SetX(currentX + offsetX);
        m_pBackgroundTileRock->SetY(currentY + offsetY);
    }

    if (m_pBackgroundTileMoss)
    {
        int currentX = m_pBackgroundTileMoss->GetX();
        int currentY = m_pBackgroundTileMoss->GetY();
        m_pBackgroundTileMoss->SetX(currentX + offsetX);
        m_pBackgroundTileMoss->SetY(currentY + offsetY);
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
 
void BackgroundManager::SetTilesLayerScale(float scaleX, float scaleY) 
{
    if (m_pBackgroundTileRock)
    {
        m_pBackgroundTileRock->SetScale(scaleX, scaleY);
    }

    if (m_pBackgroundTileMoss)
    {
        m_pBackgroundTileMoss->SetScale(scaleX, scaleY);
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
