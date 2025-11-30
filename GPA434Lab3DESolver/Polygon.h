#pragma once

#include <QPolygonF>
#include <QColor>
#include <QPointF>

// -----------------------------------------------------------------------------
// Classe de base Polygone + 3 derives polymorphiques
// -----------------------------------------------------------------------------

class Polygon : public QPolygonF
{
public:
    Polygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    virtual ~Polygon();

    size_t setNumVertices() const;
    QColor setFillColor() const;
    QColor setEdgeColor() const;

    virtual QPolygonF basePolygon() const = 0;

    QPolygonF translate(QPointF const& delta) const;
    QPolygonF rotate(double angleDeg) const;
    QPolygonF scale(double factor) const;

protected:
    size_t mNumVertices{};
    QColor mFillColor;
    QColor mEdgeColor;
};

// -----------------------------------------------------------------------------
// Polygone regulier
// -----------------------------------------------------------------------------

class PolygoneRegulier : public Polygon
{
public:
    PolygoneRegulier(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    ~PolygoneRegulier() override = default;

    QPolygonF basePolygon() const override;
};

// -----------------------------------------------------------------------------
// Forme en L (polygone concave dans le carre [-1,1] x [-1,1])
// -----------------------------------------------------------------------------

class PolygoneConvexe : public Polygon
{
public:
    PolygoneConvexe(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    ~PolygoneConvexe() override = default;

    QPolygonF basePolygon() const override;
};

// -----------------------------------------------------------------------------
// Polygone etoile
// -----------------------------------------------------------------------------

class PolygoneEtoile : public Polygon
{
public:
    PolygoneEtoile(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    ~PolygoneEtoile() override = default;

    QPolygonF basePolygon() const override;
};
