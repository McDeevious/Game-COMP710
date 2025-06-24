#ifndef KNIGHTCLASS_H
#define KNIGHTCLASS_H

#include "vector2.h"
#include "character.h"
#include "collision.h"
#include <fmod.hpp>
#include "sharedenums.h"	

class AnimatedSprite;
class Hitbox; 
class Renderer;
class InputSystem;
class Enemy;

//enum AttackType {
//	ATTACK_NONE,
//	ATTACK_1,
//	ATTACK_2,
//	SP_ATTACK
//};

class KnightClass : public Character
{
public:
	KnightClass();
	~KnightClass();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime, SceneGame& game);
	void Draw(Renderer& renderer);

	void ProcessInput(InputSystem& inputSystem, SceneGame& game);
	
	// Sets boundaries for knight movement
	void SetBoundaries(float left, float right, float top, float bottom);
	
	// Get the current movement direction (for background scrolling)
	const Vector2& GetLastMovementDirection() const;
    
    // Get the knight's current position
    const Vector2& GetPosition() const;

    // Get the knight's health
	int GetHealth() const;

	//Damage taking
	void TakeDamage(int amount); 
	int AttackDamage() const;
	bool isAttacking() const;
	bool isProjectilesActive() const;

	// Buff Character
	void buffCharacter(BuffType buff);

	void SetPosition(Vector2 pos);

	// State checking
	bool IsDead() const;

	//Collisions 
	Hitbox GetHitbox() const;  
	Hitbox GetAttackHitbox(const Enemy& enemy) const;

private:
	//Clamp knight to boundaries
	void ClampPositionToBoundaries();
	
	// Helper method to start an attack animation
	void StartAttack(AttackType attackType);

	// Player character
	AnimatedSprite* m_knightIdle; 
	AnimatedSprite* m_knightWalk;
	AnimatedSprite* m_knightHurt;
	AnimatedSprite* m_knightDeath;

	//Movement and state
	Vector2 m_knightPosition;
	float m_knightSpeed;
	bool m_knightLeft;
	bool m_isMoving;
	bool m_isDead;
	bool m_isHurt; 
	int m_damageReduction;
	int m_knighthealth;
	int m_maxHealth;

	// Regen
	int m_regen;
	float m_regenTimeAcculmated;
	bool m_isRegenApplied;

	//Player attacks
	AttackType m_attackState;
	bool m_isAttacking;
	bool m_alternateAttacks;
	AnimatedSprite* m_knightAttack1;
	AnimatedSprite* m_knightAttack2;
	AnimatedSprite* m_knightSpecial;
	AnimatedSprite* m_knightBlock;
	float m_attackDuration;
	int m_attackModifier;

	
	// Movement tracking for background scrolling
	Vector2 m_lastMovementDirection;
	
	// Screen boundaries
	float m_leftBoundary;
	float m_rightBoundary;
	float m_topBoundary;
	float m_bottomBoundary;
	
	// Jumping mechanics
	bool m_isJumping;
	float m_jumpVelocity;
	float m_gravity;         
	float m_jumpStrength;    
	float m_groundY;

	FMOD::Sound* m_attackSound;
	FMOD::Sound* m_hurtSound;
	FMOD::Sound* m_deathSound;
	FMOD::Sound* m_jumpSound;
	float m_sfxVolume; 
};

#endif // KNIGHTCLASS_H