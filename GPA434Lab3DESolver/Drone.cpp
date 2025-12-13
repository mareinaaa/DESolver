#include <Drone.h>


Drone::Drone(double x, double y, double radius, std::string spritePath, double weight)
    : SpriteObject(x, y, radius, spritePath)
    , mWeight{ weight }
{

}
    

double Drone::weight() const
{
    return mWeight;
}




class AirDrone : public Drone
{
public:
    AirDrone();
    ~AirDrone() override = default;
};