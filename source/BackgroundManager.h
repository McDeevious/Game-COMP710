#ifndef BACKGROUNDMANAGER_H
#define BACKGROUNDMANAGER_H

#include "vector2.h"
#include "animatedsprite.h"
#include "sharedenums.h"

// Forward declarations
class Sprite;
class Renderer;

class area;
class AnimatedSprite;
class BackgroundManager
{
public:
    BackgroundManager();
    ~BackgroundManager();
    int* tilearray(int row);
    int areaArray[120][40];//wide, height
   // float getOffsetX();
    float getSize();
    float getWide();
    float getHeight();
    float getWH();
    bool Initialise(Renderer& renderer);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);
    void DrawLayer(Renderer& renderer, Sprite* layer, float scrollX);

    // Method to update scroll position based on character movement
    void UpdateScrollFromCharacterMovement(const Vector2& movementDirection, float deltaTime);

    // Adjust background layers positions
    void AdjustBackLayerPosition(int offsetX, int offsetY);
    void AdjustMiddleLayerPosition(int offsetX, int offsetY);

    // Adjust background layers scales
    void SetBackLayerScale(float scaleX, float scaleY);
    void SetMiddleLayerScale(float scaleX, float scaleY);
    void changePos(float x, float y);
    float getOffsetX();
    void getAreaArray();
   // float getOffsetX();

    // Method to change background to mushroom
    void ChangeBackgrounds(BackgroundType backgroundType);

private:
    // Background layers
    Sprite* m_pBackgroundBack;
    Sprite* m_pBackgroundMiddle;

    // Forest back (lvl 1)
    Sprite* m_pForestBack;
    Sprite* m_pForestMiddle;

    // Mushroom forest back (lvl 2)
    Sprite* m_pMushroomBack;
    Sprite* m_pMushroomMiddle;

    float m_backScrollX;
    float m_middleScrollX;
    float m_tilesScrollX;

    // Scroll speeds
    float m_backScrollSpeed;
    float m_middleScrollSpeed;
    float m_tilesScrollSpeed;
   
    area* gameLevel;

    Vector2 m_position;
};

#endif // BACKGROUNDMANAGER_H