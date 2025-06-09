#ifndef BACKGROUNDMANAGER_H
#define BACKGROUNDMANAGER_H

#include "vector2.h"
#include "animatedsprite.h"

// Forward declarations
class Sprite;
class Renderer;

class BackgroundManager
{
public:
    BackgroundManager();
    ~BackgroundManager();

    bool Initialise(Renderer& renderer);
    void Process(float deltaTime);
    void Draw(Renderer& renderer); 
    void DrawLayer(Renderer& renderer, Sprite* layer, float scrollX);
    void DrawGround(Renderer& renderer, float scrollX); 
    
    // Method to update scroll position based on character movement
    void UpdateScrollFromCharacterMovement(const Vector2& movementDirection, float deltaTime);
    
    // Adjust background layers positions
    void AdjustBackLayerPosition(int offsetX, int offsetY);
    void AdjustMiddleLayerPosition(int offsetX, int offsetY);
    void AdjustTilesLayerPosition(int offsetX, int offsetY);
    
    // Adjust background layers scales
    void SetBackLayerScale(float scaleX, float scaleY);
    void SetMiddleLayerScale(float scaleX, float scaleY);
    void SetTilesLayerScale(float scaleX, float scaleY);

private:
    // Background layers
    Sprite* m_pBackgroundBack;
    Sprite* m_pBackgroundMiddle; 
    Sprite* m_pBackgroundTileRock; 
    Sprite* m_pBackgroundTileMoss; 
    float m_backScrollX; 
    float m_middleScrollX; 
    float m_tilesScrollX; 
    
    // Scroll speeds
    float m_backScrollSpeed;
    float m_middleScrollSpeed;
    float m_tilesScrollSpeed;
};

#endif // BACKGROUNDMANAGER_H