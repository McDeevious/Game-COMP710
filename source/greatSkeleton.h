#ifndef GREATSKELETON_H
#define GREATSKELETON_H

#include "skeleton.h"
class SceneGame;
class GreatSkeleton : public Skeleton
{
public:
    GreatSkeleton();
    virtual ~GreatSkeleton();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime, SceneGame& game) override;

};

#endif // GREATSKELETON_H



