#ifndef CHARACTER_H
#define CHARACTER_H

#include "vector2.h"
#include "collision.h"
#include <fmod.hpp>
#include "sharedenums.h"
#include "enemy.h"
#include "bufftype.h"

class Renderer;
class InputSystem;
class Hitbox;
class Enemy;
class SceneGame;
class Character {
public:
    Character();
    virtual ~Character();

    // Core interface
    virtual bool Initialise(Renderer& renderer) = 0;
    virtual void Process(float deltaTime, SceneGame& game) = 0;
    virtual void Draw(Renderer& renderer) = 0;
    virtual void ProcessInput(InputSystem& inputSystem, SceneGame& game) = 0;

    // State and logic
    virtual void SetBoundaries(float left, float right, float top, float bottom) = 0;
    virtual const Vector2& GetLastMovementDirection() const = 0;
    virtual const Vector2& GetPosition() const = 0;
    virtual int GetHealth() const = 0;
    virtual void TakeDamage(int amount) = 0;
    virtual int AttackDamage() const = 0;
    virtual bool isAttacking() const = 0;
    virtual bool isProjectilesActive() const = 0;
    virtual void buffCharacter(BuffType buff) = 0;
    virtual bool IsDead() const = 0;

    // Optional
    virtual bool isBlocking() const { return false; }

    // Collision
    virtual Hitbox GetHitbox() const = 0;
    virtual Hitbox GetAttackHitbox(const Enemy& enemy) const = 0;

    float setAreanapos;
    float getArenaPos();
    void getAreaArray(SceneGame& game);
    int areaArray[120][40];//wide, height
    int* tilearray(int row);
    bool collision(int type, SceneGame& game);
    void setBounds(SceneGame& game);
    Vector2 upperBounds;
    Vector2 lowerBounds;
    float character_size;
    CharacterType characterType;
};

#endif // CHARACTER_H