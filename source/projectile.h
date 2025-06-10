#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "vector2.h"
#include "collision.h"
#include <fmod.hpp>
#include "sharedenums.h"

class Sprite;
class Hitbox;
class Renderer;
class InputSystem;

class Projectile {
public:
	Projectile();
	~Projectile();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	// Get the arrow's current position
	const Vector2& GetPosition() const;
	void SetPosition(Vector2 newPos);

	//Collisions 
	Hitbox GetHitbox() const;

private:
	Sprite* m_pProjectile;

	Vector2 m_position;
	float m_fHitBoxWidth;
	float m_fHitBoxHeight;
	float m_fSpeed;
	float m_fLifeSpan;
	float m_fTimeElapsed;

public:
	bool m_bActive;
	bool m_bJustDied;
};

#endif // PROJECTILE_H