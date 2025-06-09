// COMP710 GP Framework 2022
#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H
// Local includes:
#include "sprite.h"
// Forward declarations:
class Renderer;
class VertexArray;
// Class declaration:
class AnimatedSprite : public Sprite
{
	// Member methods:
public:
	AnimatedSprite();
	~AnimatedSprite();
	bool Initialise(Texture& texture);
	void SetupFrames(int fixedFrameWidth, int fixedFrameHeight);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void SetLooping(bool loop);
	void Animate();
	bool IsAnimating() const;
	void StopAnimating();
	void Restart();
	void SetFrameDuration(float seconds);
	int GetWidth() const;
	int GetHeight() const;
	void DebugDraw();
	
	// New methods for frame selection and tracking
	void SetFrameX(int x);
	void SetFrameY(int y);
	int GetCurrentFrame() const; 
	void SetCurrentFrame(int frame);  
protected:
private:
	AnimatedSprite(const AnimatedSprite& animatedsprite);
	AnimatedSprite& operator=(const AnimatedSprite& animatedsprite);
	// Member data:
public:
protected:
	VertexArray* m_pVertexData;
	int m_iFrameWidth;
	int m_iFrameHeight;
	int m_iCurrentFrame;
	int m_iTotalFrames;
	float m_fTimeElapsed;
	float m_frameDuration;
	float totalTime;
	bool m_bAnimating;
	bool m_bLooping;
	
	// Keep track of row and column for sprite sheets
	int m_iFrameX;
	int m_iFrameY;
	int m_iFramesPerRow;
private:
};
#endif // ANIMATEDSPRITE_H