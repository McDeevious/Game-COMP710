#ifndef BOSSDEATH_H
#define BOSSDEATH_H

#include "enemy.h"

class BossDeath : public Enemy {
public:
    BossDeath();
    virtual ~BossDeath();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;
    void Draw(Renderer& renderer, float scrollX) override;

    void SetPosition(float x, float y) override;
    Vector2 GetPosition() const override;

    void SetBehavior(EnemyBehavior behavior) override;
    void SetPatrolRange(float left, float right) override;
    void UpdateAI(const Vector2& playerPosition, float deltaTime) override; 

    void TakeDamage(int amount) override;
    bool IsAlive() const override;
    bool IsAttacking() const override;
    EnemyAttackType GetAttackState() const override;

    Hitbox GetHitbox() const override;
    Hitbox GetAttackHitbox() const override;

    int GetScore() const override;
    bool WasScored() const override;
    void MarkScored() override;

protected:
    AnimatedSprite* m_deathIdle;
    AnimatedSprite* m_deathWalk;
    AnimatedSprite* m_deathHurt;
    AnimatedSprite* m_deathDeath;
    AnimatedSprite* m_deathAttack;
    AnimatedSprite* m_deathCast;
    AnimatedSprite* m_deathSpell; 

    bool m_spellTriggered = false;
    Vector2 m_targetPosition; 

    void UpdateSpriteScales();
};

#endif // BOSSDEATH_H

