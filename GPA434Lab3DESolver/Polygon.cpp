#include "Polygon.h"

#include <QTransform>
#include <QtMath>

// -----------------------------------------------------------------------------
// Polygone (base)
// -----------------------------------------------------------------------------

Polygon::Polygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : mNumVertices(numVertices), mFillColor(fillColor), mEdgeColor(edgeColor)
{
}

Polygon::~Polygon() = default;

size_t Polygon::setNumVertices() const { return mNumVertices; }
QColor Polygon::setFillColor() const { return mFillColor; }
QColor Polygon::setEdgeColor() const { return mEdgeColor; }

QPolygonF Polygon::translate(QPointF const& delta) const
{
    QTransform transform;
    transform.translate(delta.x(), delta.y());
    return transform.map(basePolygon());
}

QPolygonF Polygon::rotate(double angleDeg) const
{
    QTransform transform;
    transform.rotate(angleDeg);
    return transform.map(basePolygon());
}

QPolygonF Polygon::scale(double factor) const
{
    QTransform transform;
    transform.scale(factor, factor);
    return transform.map(basePolygon());
}

// -----------------------------------------------------------------------------
// PolygoneRegulier
// -----------------------------------------------------------------------------

PolygoneRegulier::PolygoneRegulier(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : Polygon(numVertices, fillColor, edgeColor)
{
}

QPolygonF PolygoneRegulier::basePolygon() const
{
    QPolygonF polygon;

    if (mNumVertices < 3) {
        return polygon;
    }

    const double radius = 1.0;
    const int n = static_cast<int>(mNumVertices);

    polygon.reserve(n);
    for (int i = 0; i < n; ++i) {
        double angle = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(n)
            - M_PI / 2.0; // pointe vers le haut
        double x = radius * qCos(angle);
        double y = radius * qSin(angle);
        polygon << QPointF(x, y);
    }

    return polygon;
}

// ---------------- PolygoneConvexe -----------------

PolygoneConvexe::PolygoneConvexe(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : Polygon(numVertices, fillColor, edgeColor)
{
}

QPolygonF PolygoneConvexe::basePolygon() const
{
    QPolygonF polygon;

    if (mNumVertices < 3)
        return polygon;

    const int    n = static_cast<int>(mNumVertices);
    const double baseR = 1.0;
    const double amplitude = 0.25;

    polygon.reserve(n);
    for (int i = 0; i < n; ++i) {
        double angle = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(n)
            - M_PI / 2.0;

        double radius = baseR * (1.0 + amplitude * qSin(3.0 * angle));

        double x = radius * qCos(angle);
        double y = radius * qSin(angle);
        polygon << QPointF(x, y);
    }
    return polygon;
}


// -----------------------------------------------------------------------------
// PolygoneEtoile
// -----------------------------------------------------------------------------

PolygoneEtoile::PolygoneEtoile(size_t numVertices, QColor const& fillColor, QColor const& edgeColor)
    : Polygon(numVertices, fillColor, edgeColor)
{
}

QPolygonF PolygoneEtoile::basePolygon() const
{
    QPolygonF polygon;

    if (mNumVertices < 3) {
        return polygon;
    }

    const int branches = static_cast<int>(mNumVertices);
    const int totalPoints = branches * 2;
    const double outerR = 1.0;
    const double innerR = 0.30;
    polygon.reserve(totalPoints);

    for (int i = 0; i < totalPoints; ++i) {

        double angle = -M_PI / 2.0 + (2.0 * M_PI * i) / static_cast<double>(totalPoints);
        double radius = (i % 2 == 0) ? outerR : innerR;

        polygon << QPointF(radius * qCos(angle),
            radius * qSin(angle));
    }

    return polygon;
}

