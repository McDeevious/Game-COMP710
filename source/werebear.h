#ifndef WEREBEAR_H
#define WEREBEAR_H

#include "werewolf.h" 

class Werebear : public Werewolf 
{
public:
    Werebear();
    virtual ~Werebear();

    bool Initialise(Renderer& renderer) override;
    void Process(float deltaTime) override;

};

#endif // WEREBEAR_H
