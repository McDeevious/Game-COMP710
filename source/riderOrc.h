#ifndef RIDERORC_H 
#define RIDERORC_H

#include "Orc.h"

class RiderOrc : public Orc
{
public:
    RiderOrc();  
    virtual ~RiderOrc();  
     
    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;

};

#endif //RIDERORC_H