#include "collision.h"

Collision::Collision() 
{
}

Collision::~Collision()
{
}

bool Collision::CheckCollision(const Hitbox& a, const Hitbox& b) {
    return !(a.x + a.width < b.x ||
        a.x > b.x + b.width ||
        a.y + a.height < b.y ||
        a.y > b.y + b.height);
}

bool Collision::PointHitbox(const Vector2& point, const Hitbox& b) {
    return (point.x >= b.x &&
        point.x <= b.x + b.width && 
        point.y >= b.y &&
        point.y <= b.y + b.height); 
}