#pragma once

#include <SpriteObject.h>

class Obstacle : public SpriteObject
{
public:
    Obstacle(double x, double y, double radius, std::string spritePath, double weight);
    ~Obstacle() = default;

    double weight() const;

private:
    double mWeight;
};