// Orc.h
#ifndef SKELETON_H
#define SKELETON_H

#include "enemy.h"   

class Skeleton : public Enemy{
public:
    Skeleton();
    virtual ~Skeleton();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;
    void Draw(Renderer& renderer, float scrollX) override;

    virtual void SetPosition(float x, float y);
    virtual Vector2 GetPosition() const;

    virtual void SetBehavior(EnemyBehavior behavior);
    virtual void SetPatrolRange(float left, float right);
    virtual void UpdateAI(const Vector2& playerPosition, float deltaTime);

    virtual void TakeDamage(int amount);
    virtual bool IsAlive() const;
    virtual bool IsAttacking() const;
    virtual EnemyAttackType GetAttackState() const;

    Hitbox GetHitbox() const override;
    Hitbox GetAttackHitbox() const override;

    virtual int GetScore() const;
    virtual bool WasScored() const;
    virtual void MarkScored();

protected:
    // Skeleton sprites
    AnimatedSprite* m_skeletonIdle;
    AnimatedSprite* m_skeletonWalk;
    AnimatedSprite* m_skeletonHurt;
    AnimatedSprite* m_skeletonDeath;
    AnimatedSprite* m_skeletonAttack1;
    AnimatedSprite* m_skeletonAttack2;

    void UpdateSpriteScales();
    
};

#endif // SKELETON_H