#ifndef ARMOREDSKELETON_H
#define ARMOREDSKELETON_H

#include "skeleton.h"

class ArmoredSkeleton : public Skeleton
{
public:
    ArmoredSkeleton();
    virtual ~ArmoredSkeleton();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;

};

#endif // ARMOREDSKELETON_H


