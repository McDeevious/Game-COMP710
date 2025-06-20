#ifndef WEREWOLF_H
#define WEREWOLF_H

#include "vector2.h"
#include "collision.h"
#include "enemytype.h" 
#include <fmod.hpp>

class AnimatedSprite;
class Hitbox;
class Renderer;

class Werewolf {
public:
    Werewolf();
    ~Werewolf();

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
    AnimatedSprite* m_werewolfIdle;
    AnimatedSprite* m_werewolfWalk;
    AnimatedSprite* m_werewolfHurt;
    AnimatedSprite* m_werewolfDeath;
    AnimatedSprite* m_werewolfAttack1;
    AnimatedSprite* m_werewolfAttack2;

    // Movement and state
    Vector2 m_werewolfPosition;
    float m_werewolfSpeed;
    int m_werewolfDirection;
    bool m_werewolfIsMoving;
    bool m_werewolfIsHurt;

    // Attack state
    EnemyAttackType m_attackState;
    bool m_isAttacking;
    float m_attackDuration;

    bool m_wasScored;

    // AI behavior
    EnemyType m_werewolfType;
    EnemyBehavior m_currentBehavior;
    float m_patrolRangeLeft;
    float m_patrolRangeRight;
    float m_detectionRange;
    float m_attackRange;

    // Combat attributes
    int m_werewolfHealth;
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

#endif // WEREWOLF_H