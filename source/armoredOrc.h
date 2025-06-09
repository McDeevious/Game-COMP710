#ifndef ARMOREDORC_H
#define ARMOREDORC_H

#include "Orc.h"

class ArmoredOrc : public Orc
{
public:
    ArmoredOrc();
    virtual ~ArmoredOrc();

    bool Initialise(Renderer& renderer) override; 
    void Process(float deltaTime) override;

};

#endif // ARMOREDORC_H

