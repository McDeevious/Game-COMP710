// COMP710 GP Framework 2025
#ifndef TEXTURE_H
#include <SDL_image.h>
#include "glew.h"
#define TEXTURE_H
class Texture
{
	// Member methods:
public:
	Texture();
	~Texture();
	bool Initialise(const char* pcFilename);
	void SetActive();
	int GetWidth() const;
	int GetHeight() const;
	void LoadTextTexture(const char* text, const char* fontname, int pointsize); 
protected:
private:
	Texture(const Texture& texture);
	Texture& operator=(const Texture& texture);
	void LoadSurfaceIntoTexture(SDL_Surface* pSurface); 
	// Member data:
public:
protected:
	unsigned int m_uiTextureId;
	int m_iWidth;
	int m_iHeight;
private:
};
#endif // TEXTURE_H
