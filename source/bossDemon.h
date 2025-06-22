// Orc.h
#ifndef SKELETON_H
#define SKELETON_H

#include "enemy.h"   

class BossDemon : public Enemy {
public:
    BossDemon();
    virtual ~BossDemon();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;
    void Draw(Renderer& renderer, float scrollX) override;

    void SetPosition(float x, float y);
    Vector2 GetPosition() const;

    void SetBehavior(EnemyBehavior behavior);
    void SetPatrolRange(float left, float right);
    //virtual void UpdateAI(const Vector2& playerPosition, float deltaTime);

    void TakeDamage(int amount);
    bool IsAlive() const;
    bool IsAttacking() const;
    EnemyAttackType GetAttackState() const;

    bool IsAnimatingDeath() const;

    Hitbox GetHitbox() const override;
    Hitbox GetAttackHitbox() const override;

    int GetScore() const;
    bool WasScored() const;
    void MarkScored();

protected:
    // Skeleton sprites
    AnimatedSprite* m_demonIdle;
    AnimatedSprite* m_demonWalk;
    AnimatedSprite* m_demonHurt;
    AnimatedSprite* m_demonDeath;
    AnimatedSprite* m_demonCleave;
    AnimatedSprite* m_demonSmash;

    void UpdateSpriteScales();

};

#endif // SKELETON_H