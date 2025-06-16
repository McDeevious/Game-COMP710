#ifndef ARCHER_H
#define ARCHER_H

#include "vector2.h"
#include "collision.h"
#include <fmod.hpp>
#include "sharedenums.h"
#include "projectile.h"
#include <vector>
#include "renderer.h"
#include "character.h"

using namespace std;

class AnimatedSprite;
class Hitbox;
class Renderer;
class InputSystem;
class Projectile;

class Archer : public Character
{
public:
	Archer();
	~Archer();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	void ProcessInput(InputSystem& inputSystem);

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
	bool isBlocking() const;
	bool isAttacking() const;

	// State checking
	bool IsDead() const;

	//Collisions 
	Hitbox GetHitbox() const;
	Hitbox GetAttackHitbox() const;

private:
	//Clamp knight to boundaries
	void ClampPositionToBoundaries();

	// Helper method to start an attack animation
	void StartAttack(AttackType attackType);

	// Player character
	AnimatedSprite* m_archerIdle;
	AnimatedSprite* m_archerWalk;
	AnimatedSprite* m_archerHurt;
	AnimatedSprite* m_archerDeath;

	//Movement and state
	Vector2 m_archerPosition;
	float m_archerSpeed;
	bool m_archerLeft;
	bool m_isMoving;
	bool m_isDead;
	bool m_isHurt;
	int m_archerhealth;

	//Player attacks
	AttackType m_attackState;
	bool m_isAttacking;
	AnimatedSprite* m_archerAttack1;
	AnimatedSprite* m_archerSpecial;
	float m_attackDuration;

	vector<Projectile*> m_pArrows;
	int m_iActiveArrows;

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

#endif // ARCHER_H