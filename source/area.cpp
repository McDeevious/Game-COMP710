
#include "area.h"
#include "renderer.h"
#include "sprite.h"
#include "animatedsprite.h"
#include <iostream>
#include <fstream>
#include <string>

area::area()
	: wide(120),
	height(40),
	tilemap{0},
	Materials{ 0 }
	, selectlevel(2)
	, tile{ 0}
	, spawnlevel(0)
	, scale(0)
	, sscene(0)
	, stage(0)
	
{


}

area::~area()
{

}
bool area::Initialise(Renderer& renderer)
{
	
	//std::string text;
	//std::string check = "level" + std::to_string(selectlevel);
	sizey = (float)renderer.GetHeight() / (float)height;
	scale = (sizey / 20);//17 old maybe switch back
	sizex = ((float)renderer.GetWidth() - (float)renderer.GetHeight()) / 2;

	//std::ifstream fileread("assets\\levelselect.txt");
	for (int s = 0; s < 3; s++)
	{
		for (int offset = 0; offset < 40; offset++)
		{
			for (int i = 0; i < 120; i++)
			{
				tilemap[s][i+offset*wide] = -1;
				tile[s][i + offset * wide] = new AnimatedSprite();
				tile[s][i + offset * wide] = renderer.CreateAnimatedSprite("../game/assets/Sprites/tiles.png");
				tile[s][i + offset * wide]->SetupFrames(20, 20);
			}
		}
	}
	
	return 1;
}

void area::setScene(int scene)
{
	int array[4];
	sscene = scene;
	if (scene == 0)
	{
		array[0] = 12;//void
		array[1] = 8;
		array[2] = 0;
		array[3] = 4;
		//array[4] = 5;
	}
	else if (scene == 1)
	{
		array[0] = 13;
		array[1] = 8;
		array[2] = 0;
		array[3] = 4;
		//array[4] = 5;
	}
	else
	{
		array[0] = 15;
		array[1] = 10;
		array[2] = 2;
		array[3] = 6;
		//	array[4] = 7;
	}

	for (int y = 0; y < 40; y++)
	{
		for (int x = 0; x < 120; x++)
		{

			switch (tilemap[stage][x + y*wide])
			{
			case -1://void
				if (x % 2 == 1 && scene == 1) {
					tile[stage][x + y * wide]->SetCurrentFrame(array[0] + 1);
					tile[stage][x + y * wide]->setLastFrame(array[0] + 1);
				}
				else
				{
					tile[stage][x + y * wide]->SetCurrentFrame(array[0]);
					tile[stage][x + y * wide]->setLastFrame(array[0]);

				}
				break;

			case 1://dirt
				if (x % 2 == 1) {
					tile[stage][x + y * wide]->SetCurrentFrame(array[1]);
					tile[stage][x + y * wide]->setLastFrame(array[1]);
				}
				else
				{
					tile[stage][x + y * wide]->SetCurrentFrame(array[1] + 1);
					tile[stage][x + y * wide]->setLastFrame(array[1] + 1);
				}
				break;
			case 2://grass
				if (x % 2 == 1) {
					tile[stage][x + y * wide]->SetCurrentFrame(array[2]);
					tile[stage][x + y * wide]->setLastFrame(array[2]);
				}
				else
				{
					tile[stage][x + y * wide]->SetCurrentFrame(array[2] + 1);
					tile[stage][x + y * wide]->setLastFrame(array[2] + 1);
				}
				break;
			case 3://stone

			case 4://barrier
				if (x % 2 == 1) {
					tile[stage][x + y * wide]->SetCurrentFrame(array[3]);
					tile[stage][x + y * wide]->setLastFrame(array[3]);
				}
				else
				{
					tile[stage][x + y * wide]->SetCurrentFrame(array[3] + 1);
					tile[stage][x + y * wide]->setLastFrame(array[3] + 1);

				}
				break;
			}
			tile[stage][x + y * wide]->SetFrameDuration(0);
			tile[stage][x + y * wide]->SetLooping(1);
			tile[stage][x + y * wide]->Animate();
			tile[stage][x + y * wide]->SetScale(1,1);
			tile[stage][x + y * wide]->SetAngle(1);
			tile[stage][x + y * wide]->SetX(((((x + 1) * sizey) - (x * sizey)) / 2) + sizex + (x * sizey));
			tile[stage][x + y * wide]->SetY(y * sizey + ((((y + 1) * sizey) - (y * sizey)) / 2));
			tile[stage][x + y * wide]->SetScale(scale,scale);
		}
	}

}
void area::setLevel(Renderer& renderer, int level)
{
	std::string text;
	//selectlevel = level;
	selectlevel = 1;
	std::string check = "level" + std::to_string(selectlevel);
	sizey = (float)renderer.GetHeight() / (float)height;
	scale = (sizey / 20);//17 old maybe switch back
	sizex = ((float)renderer.GetWidth() - (float)renderer.GetHeight()) / 2;
	int tempStage = 1;
	int tempPart = 1;
	int count = 0;
	int offset = 0;
	bool cstage = 0;
	bool cpart = 0;
	std::ifstream fileread("../game/assets/Sprites/Level-selection.txt");
	for (int s = 0; s  < 3; s++)
	{
		for (int y = 0; y < 40; y++)
		{
			for (int x = 0; x < 120; x++)
			{
				tilemap[s][x + y * wide] = -1;
			}
		}
	}
	while (std::getline(fileread, text))
	{
		std::string checkPart = "part" + std::to_string(tempPart);
		std::string checkStage = "stage" + std::to_string(tempStage);
		if (text.compare(check) == 0)
		{
			count ^= 1;
			tempStage = 0;
		}
		else if (text.compare(checkPart) == 0)
		{
			if (cpart)
			{
				tempPart++;
				cpart = 0;
				offset = 0;
			}
			else
			{
				cpart = 1;
			}
		}
		else if (text.compare(checkStage) == 0)
		{
			if (cstage)
			{
				tempStage++;
				cstage = 0;
				tempPart = 0;
			}
			else
			{
				cstage = 1;
			}
		}
		else if (count == 1 && offset < 40)
		{
			char array[100];
		std:strcpy(array, text.c_str());
			for (int i = (40*tempPart-1); i < 40*tempPart; i++)
			{
				tilemap[tempStage][i + offset*40] = -1;
				switch (array[i])
				{
				case '0':
					tilemap[tempStage][i + offset * 40] = 4;
					break;
				case '1':
					tilemap[tempStage][i + offset * 40] = 1;
					break;
				case '2':
					spawnlevel = (offset - 1) * sizey + -20;
					tilemap[tempStage][i + offset * 40] = 2;
					break;
				case '3':

					tilemap[tempStage][i + offset * 40] = 3;
					break;

				}

			}
			offset++;
		}
	}
}











float area::getSizeX()
{
	return sizex;
}
float area::getSizeY()
{
	return sizey;
}
int* area::tilearray(int row)
{
	int* array = new int[120];
	for (int i = 0; i < 120; i++)
	{
		if (tilemap[stage][i+row*wide] != NULL)
		{
			array[i] = tilemap[stage][i+row*wide];
		}
		else
		{
			array[i] = -1;
		}
	}
	return array;
}

float area::getScale()
{
	return scale;
}

void area::Process(float deltaTime)
{

	for (int y = 0; y < 40; y++)
	{
		for (int x = 0; x < 120; x++)
		{
			tile[stage][y * 120 + x]->Process(deltaTime);
		}
	}
}

void area::Draw(Renderer& renderer)
{
	//renderer.SetClearColor(0, 0, 0);

	
	for (int y = 0; y < 40; y++)
	{

		for (int x = 0; x < 120; x++)
		{

			tile[stage][y * 120 + x]->Draw(renderer);

		}
	}
}
int area::getSpawnlevel()
{
	return spawnlevel;
}