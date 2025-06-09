#ifndef KNIGHTHUD_H
#define KNIGHTHUD_H

#include "sprite.h"
#include "animatedsprite.h"
#include "renderer.h"

class Sprite;
class AnimatedSprite;
class Renderer;

class KnightHUD
{
public:
	KnightHUD();
	~KnightHUD();

	bool InitialiseHealth(Renderer& renderer);
	void InitialiseScore(Renderer& renderer);
	void HealthUpdate(int currentHealth);
	void ScoreUpdate(int newScore, Renderer& renderer); 
	void Draw(Renderer& renderer);

private:
	AnimatedSprite* m_healthHUD;  
	Sprite* m_scoreHUD;
	float m_frameW; 
	float m_frameH; 
	int m_maxHealth; 
	int m_score;
};

#endif // KNIGHTHUD_H