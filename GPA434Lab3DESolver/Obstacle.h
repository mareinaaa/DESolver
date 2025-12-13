#pragma once

#include <SpriteObject.h>

class Obstacle : public SpriteObject
{
public:
    Obstacle(QPointF position, double radius, std::string spritePath, double weight);
    ~Obstacle() = default;

    double weight() const;

private:
    double mWeight;
};