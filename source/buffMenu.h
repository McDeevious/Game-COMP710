#ifndef BUFFMENU_H
#define BUFFMENU_H

#include "sprite.h"
#include "bufftype.h"  // Define enum BuffType
#include <fmod.hpp>

class Sprite;
class Renderer;
class InputSystem;


class BuffMenu
{
public:
	BuffMenu();
	~BuffMenu();

	bool Initialise(Renderer& renderer);
	void ProcessInput(InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	void Reset();

	BuffChoiceState GetState() const;
	BuffType GetBuff1() const;
	BuffType GetBuff2() const;

private:
	Sprite* m_buffLeft;
	Sprite* m_buffRight; 
	Sprite* m_buffBack; 

	BuffType m_buffTypes[2];
	BuffType m_selectedBuffs[2];
	int m_selectionCount; 

	bool hoverBuff1;
	bool hoverBuff2;

	BuffChoiceState m_state;
	BuffFocus m_focus;

	float m_navCooldown = 0.0f;
	const float m_navDelay = 0.2f;

	FMOD::Sound* m_hoverSound;
	float m_buttonVolume;
	bool m_hoverBuff1Button;
	bool m_hoverBuff2Button;
};
#endif //BUFFMENU_H
