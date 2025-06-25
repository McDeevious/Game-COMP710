#pragma once

// COMP710 GP Framework 2022
#ifndef AREA_H
#define AREA_H
#define _CRT_SECURE_NO_WARNINGS

// Local includes:
//#include ""
#include "vector2.h"
// Forward declarations:
class Renderer;
class Sprite;
class AnimatedSprite;
// Class declaration:
class area
{
	// Member methods:
public:
	area();
	~area();
	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void setScene(int scene);
	void setLevel(Renderer& renderer, int level);
	void changePos(float x, float y);
	//	int tilearray(int row);
	int getSpawnlevel();
	float getSizeX();
	float getSizeY();
	float getScale();
	float getOffsetX();
	float getSize();
	float getWide();
	float getHeight();
	float getWH();
	int getStage();
	void setStage(int s);
	//int getLevelStage();
	//void setStage();
	int* tilearray(int row);
	int wh;
	int sscene;
	int stage;
	float scale;
	int wide;
	int height;
	int tilemap[3][40*120];
	AnimatedSprite* tile[3][40 * 120];
	AnimatedSprite* Materials[4];
	int spawnlevel;
	int selectlevel;
	float size;
	float sizex;
	float sizey;
	Vector2 m_position;
private:
};
#endif // AREA_H_