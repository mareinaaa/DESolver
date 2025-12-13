#pragma once

#include <vector>
#include <memory>

#include <SpriteObject.h>

class Obstacle;

class Drone : public SpriteObject
{
public:
    ~Drone() = default;

    // Identité (utile UI / debug)
    QString name() const;

    // Génère la trajectoire réelle à partir des points de contrôle
    virtual void buildPath(QVector<QPointF> const& controlPoints,
        QVector<QPointF>& pathPoints) const = 0;


protected:
    Drone(QPointF position, double radius, std::string spritePath, QString name);

private:
    QString mName;
};




//class AirDrone : public Drone
//{
//public:
//    AirDrone(QPointF position, double radius, std::string spritePath, QString name);
//    ~AirDrone() override = default;
//
//    void buildPath(QVector<QPointF> const& controlPoints,
//        QVector<QPointF>& pathPoints) const override;
//
//
//};