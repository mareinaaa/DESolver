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






AirDrone::AirDrone(QPointF position, double radius, std::string spritePath, QString name)
    : Drone(position, radius, spritePath, name)
{

}

void AirDrone::buildSegment(QPointF const& from, QPointF const& to, std::vector<QPointF>& outSegment) const
{
    // Straight-line motion: one segment = two points
    outSegment.clear();
    outSegment.reserve(2);

    outSegment.push_back(from);
    outSegment.push_back(to);
}





WaterDrone::WaterDrone(QPointF position, double radius, std::string spritePath, double amplitude, int samples)
    : Drone(position, radius, spritePath, "Water drone")
    , mAmplitude(amplitude)
    , mSamples(samples)
{
}

void WaterDrone::buildSegment(QPointF const& from, QPointF const& to, std::vector<QPointF>& outSegment) const
{
    double pi = 3.14159265358979323846;

    outSegment.clear();
    outSegment.reserve(mSamples + 1);

    for (int i = 0; i <= mSamples; ++i)
    {
        double t = static_cast<double>(i) / mSamples;

        // Linear interpolation in X and Y
        double x = from.x() + t * (to.x() - from.x());
        double y = from.y() + t * (to.y() - from.y());

        // Sinusoidal offset perpendicular to the segment direction
        double offset = mAmplitude * std::sin(2.0 * pi * t);

        // Apply offset on Y (simple & sufficient for this project)
        y += offset;

        outSegment.emplace_back(x, y);
    }
}





LandDrone::LandDrone(QPointF position, double radius, std::string spritePath, double amplitude, int steps)
    : Drone(position, radius, spritePath, "Land drone")
    , mAmplitude(amplitude)
    , mSteps(steps)
{
}

void LandDrone::buildSegment(QPointF const& from, QPointF const& to, std::vector<QPointF>& outSegment) const
{
    outSegment.clear();
    outSegment.reserve(mSteps + 1);

    // Direction vector
    double dx = to.x() - from.x();
    double dy = to.y() - from.y();

    // Perpendicular direction (normalized)
    double length = std::sqrt(dx * dx + dy * dy);
    double px = -dy / length;
    double py = dx / length;

    for (int i = 0; i <= mSteps; ++i)
    {
        double t = static_cast<double>(i) / mSteps;

        // Base linear interpolation
        double x = from.x() + t * dx;
        double y = from.y() + t * dy;

        // Zig-zag offset: alternate left/right
        double sign = (i % 2 == 0) ? 1.0 : -1.0;
        x += sign * mAmplitude * px;
        y += sign * mAmplitude * py;

        outSegment.emplace_back(x, y);
    }
}