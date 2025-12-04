#pragma once
//#ifndef QDEGEOMETRICOPTIMISATIONPANEL_H
//#define QDEGEOMETRICOPTIMISATIONPANEL_H
#include "QDESolutionPanel.h"
#include <SolutionStrategy.h>

#include <QRectF>
#include <QColor>
#include <QVector>
#include <QList>
#include <QPolygonF>

class QImageViewer;
class QScrollBar;
class QHBoxLayout;
class QComboBox;
class Polygon;

//! \brief Panneau représentant le problème d'optimisation géométrique affine.
class QDEGeometricOptimisationPanel : public QDESolutionPanel
{
    Q_OBJECT

public:
    // Constructors, destructor and assignment operators
    QDEGeometricOptimisationPanel(QWidget* parent = nullptr);
    QDEGeometricOptimisationPanel(QDEGeometricOptimisationPanel const&) = default;
    QDEGeometricOptimisationPanel(QDEGeometricOptimisationPanel&&) = default;
    QDEGeometricOptimisationPanel& operator=(QDEGeometricOptimisationPanel const&) = default;
    QDEGeometricOptimisationPanel& operator=(QDEGeometricOptimisationPanel&&) = default;
    ~QDEGeometricOptimisationPanel() override = default;

    // Box dimensions getter ??? GETTERS

    // QDESolutionPanel interface : retourne la stratégie pour ce problème
    de::SolutionStrategy* buildSolution() const override;

public slots:
    // QDESolutionPanel interface : met à jour la visualisation
    void updateVisualization(QDEAdapter const& de) override;

private:
    enum class polygoneMode { Regular, Convex, Star };

    //Widgets 
    QImageViewer* mVisualizationLabel;
    QScrollBar* mObstacleScrollBar;
    QScrollBar* mVertexScrollBar;
    QComboBox* mShapeComboBox;
    //QPushButton* regenerateButton;

    // Données de visualisation
    QRectF         mCanvasRect;
    QList<QPointF> mObstaclePoints;
    QPolygonF      mBasePolygon;

    // Couleurs
    QColor const mCanvasColor;
    QColor const mObstacleColor;
    QColor const mShapeFillColor;
    QColor const mShapeEdgeColor;

    // Formes de base
    QVector<Polygon*> mPolygones;

    // Construction d'une ligne "scrollbar + étiquette"
    QWidget* buildScrollBarWidget(QScrollBar*& sb, int min, int max,
        int defValue, QString const& suffix = QString());

    void updateCanvasRect();
    polygoneMode currentShapeKind() const;
    Polygon* currentPolygone() const;

    void regenerateObstacles();  // Recréation aléatoire des obstacles
    void rebuildPolygons();      // Reconstruit les 3 formes (régulier, convexe, étoile)
    void drawPreview();          // Affiche uniquement la forme de base centrée
    void establishConnections();

    // Stratégie de solution interne
    class geometricOptimisationStrategy;

    // Description texte du problème (HTML)
    static const QString gop_description;
};


//======================================================================
//  geometricOptimisationStrategy
//======================================================================

class QDEGeometricOptimisationPanel::geometricOptimisationStrategy
    : public de::SolutionStrategy
{
public:
    geometricOptimisationStrategy(QRectF canvas, QList<QPointF> obstacles, QPolygonF basePolygon);
    geometricOptimisationStrategy(geometricOptimisationStrategy const&) = default;
    geometricOptimisationStrategy(geometricOptimisationStrategy&&) = default;
    geometricOptimisationStrategy& operator=(geometricOptimisationStrategy const&) = default;
    geometricOptimisationStrategy& operator=(geometricOptimisationStrategy&&) = default;
    ~geometricOptimisationStrategy() override = default;

    std::string toString(de::Solution const& solution) const override;

    DEFINE_OVERRIDE_CLONE_METHOD(geometricOptimisationStrategy)

protected:
    double process(de::Solution const& solution) override;

private:
    QRectF         mCanvas;
    QList<QPointF> mObstacles;
    QPolygonF      mBasePolygon;
};

//#endif // QDEGEGEOMETRICOPTIMISATIONPANEL_H
