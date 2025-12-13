#pragma once
#include <vector>
#include <string>

class SpriteObject
{
public:
    virtual ~SpriteObject() = default;

    double posX() const;
    double posY() const;
    double radius() const;
    std::string spritePath() const;

    void setPosition(double x, double y);

protected:
    SpriteObject(double x, double y, double radius, std::string spritePath);

private:
    double mRadius;
    double mX;
    double mY;
    std::string mSpritePath;
};