#include <SpriteObject.h>


SpriteObject::SpriteObject(double x, double y, double radius, std::string spritePath)
    : mX{ x }
    , mY{ y }
    , mRadius{ radius }
    , mSpritePath{ spritePath }
{

}


double SpriteObject::posX() const
{
    return mX;
}

double SpriteObject::posY() const
{
    return mY;
}

double SpriteObject::radius() const
{
    return mRadius;
}

std::string SpriteObject::spritePath() const
{
    return mSpritePath;
}



void SpriteObject::setPosition(double x, double y)
{
    mX = x;
    mY = y;
}