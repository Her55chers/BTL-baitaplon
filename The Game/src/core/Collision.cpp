#include "core/Collision.h"
#include "gameObject/CollisionComponent.h"

bool Collision::AABB(const SDL_Rect& recA,const SDL_Rect& recB)
{
    if (
        recA.x + recA.w >= recB.x + 25 &&
        recB.x + recB.w >= recA.x + 25 &&
        recA.y + recA.h >= recB.y + 25 &&
        recB.y + recB.h >= recA.y + 25
        )
        return true;
    {
        return false;
    }
}
