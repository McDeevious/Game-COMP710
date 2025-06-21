#ifndef WEREWOLF_H
#define WEREWOLF_H

#include "enemy.h" 

class Werewolf : public Enemy{ 
public:
    Werewolf();
    virtual ~Werewolf(); 

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
    // Orc sprites
    AnimatedSprite* m_werewolfIdle;
    AnimatedSprite* m_werewolfWalk;
    AnimatedSprite* m_werewolfHurt;
    AnimatedSprite* m_werewolfDeath;
    AnimatedSprite* m_werewolfAttack1;
    AnimatedSprite* m_werewolfAttack2;

    void UpdateSpriteScales();
   
};

#endif // WEREWOLF_H