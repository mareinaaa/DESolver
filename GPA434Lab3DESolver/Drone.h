#pragma once

#include <vector>
#include <memory>

#include <SpriteObject.h>

class Obstacle;

class Drone : public SpriteObject
{
public:
    virtual ~Drone() = default;

    // Cost between two slices
    double segmentCost(
        double x0, double y0,
        double x1, double y1,
        const std::vector<std::shared_ptr<Obstacle>>& obstacles
    ) const;

    double weight() const;

protected:
    Drone(double x, double y, double radius, std::string spritePath, double weight);

private:
    double mWeight;
};




class AirDrone : public Drone
{
public:
    AirDrone();
    ~AirDrone() override = default;
};