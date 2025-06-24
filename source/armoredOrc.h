#ifndef ARMOREDORC_H
#define ARMOREDORC_H

#include "Orc.h"
class SceneGame;
class Render;
class ArmoredOrc : public Orc
{
public:
    ArmoredOrc();
    virtual ~ArmoredOrc();

    bool Initialise(Renderer& renderer) override; 
    void Process(float deltaTime, SceneGame& game) override;

};

#endif // ARMOREDORC_H

