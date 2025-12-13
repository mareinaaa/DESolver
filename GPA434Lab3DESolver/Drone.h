#pragma once

#include <vector>
#include <SpriteObject.h>



class Drone : public SpriteObject
{
public:
    ~Drone() = default;

    // Identité (utile UI / debug)
    QString name() const;

    // Build the local curve between two consecutive slices
    virtual void buildSegment(QPointF const& from, QPointF const& to, std::vector<QPointF>& outSegment) const = 0;

protected:
    Drone(QPointF position, double radius, std::string spritePath, QString name);

private:
    QString mName;
};




class AirDrone : public Drone
{
public:
    AirDrone(QPointF position, double radius, std::string spritePath, QString name);
    ~AirDrone() override = default;

    void buildSegment(QPointF const& from, QPointF const& to, std::vector<QPointF>& outSegment) const override;
};



class WaterDrone : public Drone
{
public:
    WaterDrone(QPointF position, double radius, std::string spritePath, double amplitude, int samples = 20);

    void buildSegment(QPointF const& from, QPointF const& to, std::vector<QPointF>& outSegment) const override;

private:
    double mAmplitude;   // height of the sinus oscillation
    int    mSamples;     // number of points used to discretize the curve
};



class LandDrone : public Drone
{
public:
    LandDrone(QPointF position, double radius, std::string spritePath, double amplitude, int steps = 10);

    void buildSegment(QPointF const& from, QPointF const& to, std::vector<QPointF>& outSegment) const override;

private:
    double mAmplitude; // lateral zig-zag amplitude
    int    mSteps;     // number of zig-zag segments
};