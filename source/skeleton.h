// Orc.h
#ifndef SKELETON_H
#define SKELETON_H

#include "vector2.h"
#include "collision.h"
#include "enemytype.h" 
#include <fmod.hpp>

class AnimatedSprite;
class Hitbox;
class Renderer;

class Skeleton {
public:
    Skeleton();
    ~Skeleton();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime);
    virtual void Draw(Renderer& renderer, float scrollX);

    // Position and movement
    void SetPosition(float x, float y);
    Vector2 GetPosition() const;

    // AI behavior
    void SetBehavior(EnemyBehavior behavior);
    void SetPatrolRange(float left, float right);
    void UpdateAI(const Vector2& playerPosition, float deltaTime);

    // Combat
    void TakeDamage(int amount);
    bool IsAlive() const;
    EnemyAttackType GetAttackState() const;
    bool IsAttacking() const;

    //Score value
    int GetScore() const;
    bool WasScored() const;
    void MarkScored();

    Hitbox GetHitbox() const;
    Hitbox GetAttackHitbox() const;

private:


protected:
    // Orc sprites
    AnimatedSprite* m_skeletonIdle;
    AnimatedSprite* m_skeletonWalk;
    AnimatedSprite* m_skeletonHurt;
    AnimatedSprite* m_skeletonDeath;
    AnimatedSprite* m_skeletonAttack1;
    AnimatedSprite* m_skeletonAttack2;

    // Movement and state
    Vector2 m_skeletonPosition;
    float m_skeletonSpeed;
    int m_skeletonDirection;
    bool m_skeletonIsMoving;
    bool m_skeletonIsHurt;

    // Attack state
    EnemyAttackType m_attackState;
    bool m_isAttacking;
    float m_attackDuration;

    bool m_wasScored;

    // AI behavior
    EnemyType m_skeletonType;
    EnemyBehavior m_currentBehavior;
    float m_patrolRangeLeft;
    float m_patrolRangeRight;
    float m_detectionRange;
    float m_attackRange;

    // Combat attributes
    int m_skeletonHealth;
    bool m_isAlive;
    float m_attackCooldown;
    float m_currentAttackCooldown;

    // audio 
    FMOD::Sound* m_attackSound;
    FMOD::Sound* m_hurtSound;
    FMOD::Sound* m_deathSound;
    float m_sfxVolume;

    void UpdateSpriteScales(); 

};

#endif // SKELETON_H