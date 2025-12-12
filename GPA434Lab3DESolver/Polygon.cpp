#include "Polygon.h"

#include <QTransform>
#include <QtMath>


Polygon::Polygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : mVertexCount(numVertices), mFillColor(fillColor), mEdgeColor(edgeColor)
{
}

Polygon::~Polygon() = default;

size_t Polygon::vertexCount() const { return mVertexCount; }
QColor Polygon::fillColor() const { return mFillColor; }
QColor Polygon::edgeColor() const { return mEdgeColor; }

QPolygonF Polygon::applyTransform(double tx, double ty, double angleDeg, double scaleFactor) const
{
    QTransform tr;
    tr.translate(tx, ty);
    tr.rotate(angleDeg);
    tr.scale(scaleFactor, scaleFactor);

    return tr.map(basePolygon());
}

// -----------------------------------------------------------------------------
// PolygoneRegulier
// -----------------------------------------------------------------------------

RegularPolygon::RegularPolygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : Polygon(numVertices, fillColor, edgeColor)
{
}




QPolygonF RegularPolygon::basePolygon() const
{
    QPolygonF polygon;

    if (mVertexCount < 3) {
        return polygon;
    }

    const double radius = 1.0;
    double step = 2.0 * M_PI / static_cast<double>(mVertexCount);

    polygon.reserve(mVertexCount);
    for (int i = 0; i < mVertexCount; ++i) {
        double angle = static_cast<double>(i) * step - M_PI / 2.0;
        double x = radius * qCos(angle);
        double y = radius * qSin(angle);
        polygon << QPointF(x, y);
    }

    return polygon;
}




ConvexPolygon::ConvexPolygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : Polygon(numVertices, fillColor, edgeColor)
{
}

QPolygonF ConvexPolygon::basePolygon() const
{
    QPolygonF polygon;

    if (mVertexCount < 3)
        return polygon;

    const double baseR = 1.0;
    const double amplitude = 0.25;

    double step = 2.0 * M_PI / static_cast<double>(mVertexCount);

    polygon.reserve(mVertexCount);
    for (int i = 0; i < mVertexCount; ++i) {
        double angle = step * static_cast<double>(i) - M_PI / 2.0;

        double radius = baseR * (1.0 + amplitude * qSin(3.0 * angle));

        double x = radius * qCos(angle);
        double y = radius * qSin(angle);
        polygon << QPointF(x, y);
    }
    return polygon;
}




StarPolygon::StarPolygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : Polygon(numVertices, fillColor, edgeColor)
{
}

QPolygonF StarPolygon::basePolygon() const
{
    QPolygonF polygon;

    if (mVertexCount < 3) {
        return polygon;
    }

    const int branches = static_cast<int>(mVertexCount);
    const int totalSides = branches * 2;
    const double outerR = 1.0;
    const double innerR = 0.30;
    polygon.reserve(totalSides);

    for (int i = 0; i < totalSides; ++i) {

        double angle = -M_PI / 2.0 + (2.0 * M_PI * i) / static_cast<double>(totalSides);
        double radius = (i % 2 == 0) ? outerR : innerR;

        polygon << QPointF(radius * qCos(angle),
            radius * qSin(angle));
    }

    return polygon;
}

