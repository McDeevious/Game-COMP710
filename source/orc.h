// Orc.h
#ifndef ORC_H
#define ORC_H

#include "vector2.h"
#include "collision.h"
#include <fmod.hpp>

class AnimatedSprite;
class Hitbox;
class Renderer;

enum OrcType {
    ORC,
    ORC_ARMORED,
    ORC_ELITE,
    ORC_RIDER 
};

// Type of attack the orc is performing
enum OrcAttackType {
    ORC_ATTACK_NONE,
    ORC_ATTACK_1,
    ORC_ATTACK_2,
};

// Orc behavior states for AI
enum OrcBehavior {
    ORC_IDLE, 
    ORC_PATROL,   
    ORC_AGGRESSIVE  
};

class Orc {
public:
    Orc();
    ~Orc();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime);  
    virtual void Draw(Renderer& renderer, float scrollX); 

    // Position and movement
    void SetPosition(float x, float y);
    Vector2 GetPosition() const;

    // AI behavior
    void SetBehavior(OrcBehavior behavior);
    void SetPatrolRange(float left, float right);
    void UpdateAI(const Vector2& playerPosition, float deltaTime);

    // Combat
    void TakeDamage(int amount);
    bool IsAlive() const;
    OrcAttackType GetAttackState() const;
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
    AnimatedSprite* m_orcIdle;
    AnimatedSprite* m_orcWalk;
    AnimatedSprite* m_orcHurt;
    AnimatedSprite* m_orcDeath; 
    AnimatedSprite* m_orcAttack1;
    AnimatedSprite* m_orcAttack2;

    // Movement and state
    Vector2 m_orcPosition;
    float m_orcSpeed;
    int m_orcDirection;
    bool m_orcIsMoving;
    bool m_orcIsHurt;

    // Attack state
    OrcAttackType m_attackState;
    bool m_isAttacking;
    float m_attackDuration;

    bool m_wasScored;

    // AI behavior
    OrcType m_orcType; 
    OrcBehavior m_currentBehavior;
    float m_patrolRangeLeft;
    float m_patrolRangeRight;
    float m_detectionRange;  
    float m_attackRange;     

    // Combat attributes
    int m_orcHealth;
    bool m_isAlive;
    float m_attackCooldown;
    float m_currentAttackCooldown;

    //Orc audio
    FMOD::Sound* m_attackSound;
    FMOD::Sound* m_hurtSound;
    FMOD::Sound* m_deathSound;
    float m_sfxVolume;

    void UpdateSpriteScales(); 
    
};

#endif // ORC_H