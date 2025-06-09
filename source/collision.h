#ifndef COLLISION_H
#define COLLISION_H

#include "vector2.h"

struct Hitbox {
	float x;
	float y;
	float width;
	float height;
};

class Collision {

public:
	Collision();
	~Collision(); 

	static bool CheckCollision(const Hitbox& a, const Hitbox& b); 

	static bool PointHitbox(const Vector2& point, const Hitbox& b);
};

#endif COLLISION_H
