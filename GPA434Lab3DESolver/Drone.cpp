#include <Drone.h>


Drone::Drone(QPointF position, double radius, std::string spritePath, QString name)
    : SpriteObject(position, radius, spritePath)
    , mName{ name }
{

}
    

QString Drone::name() const
{
    return mName;
}



//AirDrone::AirDrone(QPointF position, double radius, std::string spritePath, QString name)
//    : Drone(position, radius, spritePath, name)
//{
//
//}
//
//void AirDrone::buildPath(QVector<QPointF> const& controlPoints, QVector<QPointF>& pathPoints) const
//{
//
//}