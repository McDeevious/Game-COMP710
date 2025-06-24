#include "collision.h"

Collision::Collision() 
{
}

Collision::~Collision()
{
}

bool Collision::CheckCollision(const Hitbox& a, const Hitbox& b) {
    if (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y)
    {
        return 1;
    }
    return 0;
    
    /**
    return !(a.x + a.width < b.x ||
        a.x > b.x + b.width ||
        a.y + a.height < b.y ||
        a.y > b.y + b.height);
        **/
}

bool Collision::PointHitbox(const Hitbox& a, const Hitbox& b) {
    if (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y)
    {
        return 1;
    }
    return 0;
}