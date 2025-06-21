
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
	, m_position{0,0}
{


}

area::~area()
{

}
bool area::Initialise(Renderer& renderer)
{
	
	//std::string text;
	//std::string check = "level" + std::to_string(selectlevel);
	//sizey = (float)renderer.GetHeight() / (float)height;
	size = (float)renderer.GetHeight() / (float)height;
	scale = (size / 20);//17 old maybe switch back
	//sizex = sizey;// ((float)renderer.GetWidth() - (float)renderer.GetHeight()) / 2;

	//std::ifstream fileread("assets\\levelselect.txt");
	for (int s = 0; s < 3; s++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < wide; x++)
			{
				tilemap[s][x+y*wide] = -1;
				tile[s][x + y * wide] = new AnimatedSprite();
				tile[s][x + y * wide] = renderer.CreateAnimatedSprite("../game/assets/Sprites/tiles.png");
				tile[s][x + y * wide]->setType();
				tile[s][x + y * wide]->SetupFrames(20, 20);
			}
		}
	}
	
	return 1;
}
void area::changePos(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
	/** for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < wide; x++)
		{
			tile[stage][x + y * wide]->SetX(tile[stage][x + y * wide]->GetX()  +x);
			tile[stage][x + y * wide]->SetY(tile[stage][x + y * wide]->GetY() + y);
		}
	}
	**/
}
float area::getYpos()
{
	return m_position.y;
}
float area::getXpos()
{
	return m_position.x;
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

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < wide; x++)
		{

			switch (tilemap[stage][x + y * wide])
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
			tile[stage][x + y * wide]->SetScale(1, 1);
			tile[stage][x + y * wide]->SetAngle(1);
			tile[stage][x + y * wide]->SetY(y * size + ((((y + 1) * size) - (y * size)) / 2));
			if (x > 0)
			{
				tile[stage][x + y * wide]->SetX(tile[stage][x-1+y*wide]->GetX() + size);
			}
			else
			{
				tile[stage][x + y * wide]->SetX(0+size/2);
			}
			tile[stage][x + y * wide]->SetScale(scale,scale);
		}
	}

}
void area::setLevel(Renderer& renderer, int level)
{
	std::string text;
	//selectlevel = level; // corrrecttttttttttt
	selectlevel = 1;
	std::string check = "level" + std::to_string(selectlevel);
	size = (float)renderer.GetHeight() / (float)height;
	scale = (size / 20);//17 old maybe switch back
	//sizex = ((float)renderer.GetWidth() - (float)renderer.GetHeight()) / 2;
	int tempStage = 1;
	int tempPart = 1;
	int count = 0;
	int offset = 0;
	bool cstage = 0;
	bool cpart = 0;
	std::cout << "test" << std::endl;
	std::ifstream fileread("../game/assets/Sprites/Level-selection.txt");
	std::cout << "passed" << std::endl;

	for (int s = 0; s  < 3; s++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < wide; x++)
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
			tempStage = 1;
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
				tempPart = 1;
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
			for (int i = 0; i < (wide/3); i++)
			{
				tilemap[tempStage-1][i+(tempPart-1)*(wide/3) + offset * wide] = -1;
				switch (array[i])
				{
				case '0':
					tilemap[tempStage-1][i + (tempPart-1) * (wide/3) + offset * wide] = 4;
					break;
				case '1':
					tilemap[tempStage-1][i + (tempPart-1) * (wide/3) + offset * wide] = 1;
					break;
				case '2':
					spawnlevel = (offset - 1) * size + -20;
					tilemap[tempStage-1][i + (tempPart-1) * (wide/3) + offset * wide] = 2;
					break;
				case '3':

					tilemap[tempStage - 1][i + (tempPart - 1) * (wide/3) + offset * wide] = 3;
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
	int* array = new int[wide];
	for (int i = 0; i < wide; i++)
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
	
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < wide; x++)
		{
			
			tile[stage][x + y * wide]->SetY(y * size + ((((y + 1) * size) - (y * size)) / 2) + m_position.y);
			if (x > 0)
			{
				tile[stage][x + y * wide]->SetX(tile[stage][x - 1 + y * wide]->GetX() + size);
			}
			else
			{
				tile[stage][x + y * wide]->SetX(tile[stage][x + y * wide]->GetX() + m_position.x );
			}
			//	tile[stage][x + y * wide]->SetX((tile[stage][x + y * wide]->GetX() + size  + m_position.x));
	
		//	tile[stage][y * wide + x]->SetX(x  + m_position.x);
			//tile[stage][y * wide + x]->SetX(y  + m_position.y);
			tile[stage][y * wide + x]->Process(deltaTime);
		}
	}
}

void area::Draw(Renderer& renderer)
{
	//renderer.SetClearColor(0, 0, 0);

	
	for (int y = 0; y < height; y++)
	{

		for (int x = 0; x < wide; x++)
		{

			tile[stage][y * wide + x]->Draw(renderer);

		}
	}
}
int area::getSpawnlevel()
{
	return spawnlevel;
}