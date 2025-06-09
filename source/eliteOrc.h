#ifndef ELITEORC_H 
#define ELITEORC_H

#include "Orc.h"

class EliteOrc : public Orc
{
public:
    EliteOrc();  
    virtual ~EliteOrc(); 

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;

};

#endif //ELITEORC_H