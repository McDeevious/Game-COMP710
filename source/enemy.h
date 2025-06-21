#ifndef ENEMY_H
#define ENEMY_H

#include "vector2.h"
#include "collision.h"
#include "enemytype.h"
#include <fmod.hpp>
#include <vector>

class AnimatedSprite;
class Renderer;
struct Hitbox;

class Enemy {
public:
    Enemy();
    virtual ~Enemy();

    virtual bool Initialise(Renderer& renderer) = 0;
    virtual void Process(float deltaTime) = 0;
    virtual void Draw(Renderer& renderer, float scrollX) = 0;

    virtual void SetPosition(float x, float y);
    virtual Vector2 GetPosition() const;

    virtual void SetBehavior(EnemyBehavior behavior);
    virtual void SetPatrolRange(float left, float right);
    virtual void UpdateAI(const Vector2& playerPosition, float deltaTime);

    virtual void TakeDamage(int amount);
    virtual bool IsAlive() const;
    virtual bool IsAttacking() const;
    virtual EnemyAttackType GetAttackState() const;

    virtual Hitbox GetHitbox() const = 0;
    virtual Hitbox GetAttackHitbox() const = 0;

    virtual int GetScore() const;
    virtual bool WasScored() const;
    virtual void MarkScored();

protected:
    Vector2 m_position;
    float m_speed;
    int m_direction;
    bool m_isMoving;
    bool m_isHurt;

    bool m_isAlive;
    int m_health;

    bool m_wasScored;

    EnemyAttackType m_attackState;
    bool m_isAttacking;
    float m_attackDuration;

    EnemyType m_type;
    EnemyBehavior m_behavior;
    float m_patrolRangeLeft;
    float m_patrolRangeRight;
    float m_detectionRange;
    float m_attackRange;
    float m_attackCooldown;
    float m_currentAttackCooldown;

    FMOD::Sound* m_attackSound;
    FMOD::Sound* m_hurtSound;
    FMOD::Sound* m_deathSound;
    float m_sfxVolume;

    std::vector<AnimatedSprite*> m_flipSprites;

    void UpdateSpriteScales();
};

#endif // ENEMY_H
