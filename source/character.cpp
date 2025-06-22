#include "character.h"
#include "SceneGame.h"
#include <iostream>
// Empty abstract class, just want it for its behaviour

Character::Character()
{

}

Character::~Character()
{

}
float Character::getArenaPos()
{
	return setAreanapos;
}

void Character::getAreaArray(SceneGame& game)
{
    for (int y = 0; y < 40; y++)
    {
        int* temp = game.tilearray(y);
        for (int x = 0; x < 120; x++)
        {
            areaArray[x][y] = temp[x];
        }
        delete[] temp;
    }
}
int* Character::tilearray(int row)
{
    int* array = new int[120];
    for (int i = 0; i < 120; i++)
    {
        if (areaArray[i][row] != NULL)
        {
            array[i] = areaArray[i][row];
        }
        else
        {
            array[i] = -1;
        }
    }
    return array;
}

void Character::setBounds(SceneGame& game)
{
   Hitbox temp = GetHitbox();
   upperBounds.x = ((temp.width + temp.x + 3 * game.getSize()) - game.getSize() / 2 - game.getOffsetX() )/ game.getSize();
   lowerBounds.x = ( (temp.x - 3 * game.getSize()) - game.getSize() / 2 - game.getOffsetX()) / game.getSize();
   upperBounds.y = ((temp.height + temp.y + 3 * game.getSize()) - game.getSize() / 2)/ game.getSize();
   lowerBounds.y = ( (temp.y - 3 * game.getSize()) - game.getSize() / 2) / game.getSize();
   if (upperBounds.x < 10)
   {
       upperBounds.x = 10;
   }
   if (upperBounds.x > 120)
   {
       upperBounds.x = 120;
   }
  
   if (upperBounds.y < 10)
   {
       upperBounds.y = 10;
   }
   if (upperBounds.y > 40)
   {
       upperBounds.y = 40;
   }

   if (lowerBounds.x < 0)
   {
       lowerBounds.x = 0;
   }
   if (lowerBounds.x > 110)
   {
       lowerBounds.x = 110;
   }

   if (lowerBounds.y < 0)
   {
       lowerBounds.y = 0;
   }
   if (lowerBounds.y > 30)
   {
       lowerBounds.y = 30;
   }
   std::cout << lowerBounds.y  << " yy  " << upperBounds.y << std::endl;
   std::cout << lowerBounds.x  << " xx  " << upperBounds.x << std::endl;
}
bool Character::collision(int type, SceneGame& game)
{
    Hitbox temp = GetHitbox();
    std::cout << "x = " << temp.x << " y = " << temp.y << " w = " << temp.width << " h = " << temp.height << std::endl;
    if (type==1)
    {
        //floor
        temp.y += temp.height;
        temp.height = temp.height / 10;
        temp.width -= 0.4;
        temp.x += 0.2;

    }
    else if (type == 2)
    {
      //  temp.y -= temp.height;
    //    temp.y += temp.height;

        temp.height = temp.height/ 10;
        temp.width -= 0.4;
        temp.x += 0.2;
        //roof
    }
    else if (type == 3)
    {
        //around
        temp.height -= 0.4;
        temp.y += 0.2;
     }
    else if (type == 4)
    {
        //right
        temp.width -= 0.4;
        temp.y += 0.2;
        temp.height -= 0.4;
    }
    else if (type == 5)
    {
        //left
        temp.width -= 0.4;
        temp.x += 0.4;
        temp.y += 0.2;
        temp.height -= 0.4;
    }
    for (int y = lowerBounds.y; y < upperBounds.y; y++)
    {
        for (int x = lowerBounds.x; x < upperBounds.x; x++)
        {
            if (areaArray[x][y] != -1)
            {
                Hitbox arena;
                arena.height = (game.getSize() * (y+1) + game.getSize() / 2) - (game.getSize() * y + game.getSize() / 2);
                arena.width = (game.getSize() * (x+1) + game.getSize() / 2 + game.getOffsetX()) -(game.getSize() * x + game.getSize() / 2 + game.getOffsetX());
                arena.x = (game.getSize() * x + game.getSize() / 2 + game.getOffsetX());
                arena.y = (game.getSize() * y + game.getSize() / 2);
                if (Collision::PointHitbox(temp, arena))
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}