#include <SpriteObject.h>


SpriteObject::SpriteObject(QPointF position, double radius, std::string spritePath)
    : mPosition{ position }
    , mRadius{ radius }
    , mSpritePath{ spritePath }
{

}


double SpriteObject::radius() const
{
    return mRadius;
}

QPointF const& SpriteObject::position() const
{
    return mPosition;
}

std::string const& SpriteObject::spritePath() const
{
    return mSpritePath;
}



void SpriteObject::setPosition(QPointF position)
{
    mPosition = position;
}

void SpriteObject::setPosition(double x, double y)
{
    mPosition.setX(x);
    mPosition.setY(y);
}