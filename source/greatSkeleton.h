#ifndef GREATSKELETON_H
#define GREATSKELETON_H

#include "skeleton.h"

class GreatSkeleton : public Skeleton
{
public:
    GreatSkeleton();
    virtual ~GreatSkeleton();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;

};

#endif // GREATSKELETON_H



