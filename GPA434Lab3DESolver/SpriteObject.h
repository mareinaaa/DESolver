#pragma once
#include <vector>
#include <QPointF>
#include <string>

class SpriteObject
{
public:
    virtual ~SpriteObject() = default;

    double radius() const;

    QPointF const& position() const;
    std::string const& spritePath() const;

    void setPosition(QPointF position);
    void setPosition(double x, double y);

protected:
    SpriteObject(QPointF position, double radius, std::string spritePath);

private:
    double mRadius;
    QPointF mPosition;
    std::string mSpritePath;
};