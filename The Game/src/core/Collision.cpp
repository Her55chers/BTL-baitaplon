#include "core/Collision.h"
#include "gameObject/CollisionComponent.h"

bool Collision::AABB(const SDL_Rect& recA,const SDL_Rect& recB)
{
    if (
        recA.x + recA.w >= recB.x + 20 &&
        recB.x + recB.w >= recA.x + 20 &&
        recA.y + recA.h >= recB.y + 20 &&
        recB.y + recB.h >= recA.y + 20
        )
        return true;
    {
        return false;
    }
}
