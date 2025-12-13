#include <Obstacle.h>


Obstacle::Obstacle(QPointF position, double radius, std::string spritePath, double weight)
    : SpriteObject(position, radius, spritePath)
    , mWeight{ weight }
{

}


double Obstacle::weight() const
{
    return mWeight;
}