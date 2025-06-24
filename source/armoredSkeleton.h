#ifndef ARMOREDSKELETON_H
#define ARMOREDSKELETON_H

#include "skeleton.h"
class SceneGame;

class ArmoredSkeleton : public Skeleton
{
public:
    ArmoredSkeleton();
    virtual ~ArmoredSkeleton();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime, SceneGame& game) override;

};

#endif // ARMOREDSKELETON_H


