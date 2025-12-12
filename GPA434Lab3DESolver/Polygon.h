#pragma once

#include <QPolygonF>
#include <QColor>
#include <QPointF>

// Classe de base Polygone + 3 derives polymorphiques
class Polygon : public QPolygonF
{
public:
    Polygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    virtual ~Polygon();

    size_t vertexCount() const;
    QColor fillColor() const;
    QColor edgeColor() const;

    virtual QPolygonF basePolygon() const = 0;

    QPolygonF applyTransform(double tx, double ty, double angleDeg, double scaleFactor) const;

protected:
    size_t mVertexCount;
    QColor mFillColor;
    QColor mEdgeColor;
};



class RegularPolygon : public Polygon
{
public:
    RegularPolygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    ~RegularPolygon() override = default;

    QPolygonF basePolygon() const override;
};



class ConvexPolygon : public Polygon
{
public:
    ConvexPolygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    ~ConvexPolygon() override = default;

    QPolygonF basePolygon() const override;
};




class StarPolygon : public Polygon
{
public:
    StarPolygon(size_t numVertices, QColor const& fillColor, QColor const& edgeColor);
    ~StarPolygon() override = default;

    QPolygonF basePolygon() const override;
};