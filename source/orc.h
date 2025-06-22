// Orc.h
#ifndef ORC_H
#define ORC_H

#include "enemy.h"  // Inherit from shared base class

class Orc : public Enemy {
public:
    Orc();
    virtual ~Orc();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;
    void Draw(Renderer& renderer, float scrollX) override;

    virtual void SetPosition(float x, float y);
    virtual Vector2 GetPosition() const;

    virtual void SetBehavior(EnemyBehavior behavior);
    virtual void SetPatrolRange(float left, float right);
    //virtual void UpdateAI(const Vector2& playerPosition, float deltaTime);

    virtual void TakeDamage(int amount);
    virtual bool IsAlive() const;
    virtual bool IsAttacking() const;
    virtual EnemyAttackType GetAttackState() const;

    bool IsAnimatingDeath() const; 

    Hitbox GetHitbox() const override;
    Hitbox GetAttackHitbox() const override;

    virtual int GetScore() const;
    virtual bool WasScored() const;
    virtual void MarkScored();

protected:
    // Orc-specific sprites
    AnimatedSprite* m_orcIdle;
    AnimatedSprite* m_orcWalk;
    AnimatedSprite* m_orcHurt;
    AnimatedSprite* m_orcDeath;
    AnimatedSprite* m_orcAttack1;
    AnimatedSprite* m_orcAttack2;

    void UpdateSpriteScales(); 
};

#endif // ORC_H
