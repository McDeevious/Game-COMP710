#ifndef ELITEORC_H 
#define ELITEORC_H

#include "Orc.h"
class SceneGame;
class EliteOrc : public Orc
{
public:
    EliteOrc();  
    virtual ~EliteOrc(); 

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime, SceneGame& game) override;

};

#endif //ELITEORC_H