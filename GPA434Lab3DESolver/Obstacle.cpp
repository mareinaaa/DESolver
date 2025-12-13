#include <Obstacle.h>


Obstacle::Obstacle(double x, double y, double radius, std::string spritePath, double weight)
    : SpriteObject(x, y, radius, spritePath)
    , mWeight{ weight }
{

}


double Obstacle::weight() const
{
    return mWeight;
}