#include "QDEGeometricOptimisationPanel.h"

#include "QImageViewer.h"
#include "Polygon.h"

#include <QFrame>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QComboBox>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>
#include <QtMath>
#include <QPushButton>
#include <QTimer>

#include <OptimizationMaximization.h>
#include <FitnessIdentity.h>

#include <format>

// Pour de::SolutionStrategy
static const std::string sTitle{
    "Optimisation geometrique"
};

static const std::string sSummary{
    "Ce probleme consiste a determiner les parametres de transformation affine afin "
    "qu'une forme maximise sa taille sans depasser le canevas ou englober un obstacle."
};

const QString QDEGeometricOptimisationPanel::sDescription =
QStringLiteral(
    "<p>La solution est repr‚sent‚e dans un espace … quatre dimensions, o— "
    "chaque composante correspond … un paramŠtre de transformation affine :</p>"
    "<ul>"
    "<li>x : d‚placement horizontal;</li>"
    "<li>y : d‚placement vertical;</li>"
    "<li>angle : rotation en degr‚s;</li>"
    "<li>s : facteur d?‚chelle uniforme.</li>"
    "</ul>"
    "<p>La fonction objective utilise directement la valeur de s. "
    "Une p‚nalit‚ n‚gative est appliqu‚e lorsqu?une transformation entraŒne "
    "un d‚passement du canevas ou recouvre un obstacle.</p>"
);



//======================================================================
//  QDEGeometricOptimisationPanel : constructeur
//======================================================================

QDEGeometricOptimisationPanel::QDEGeometricOptimisationPanel(QWidget* parent)
    : QDESolutionPanel(parent)
    , mCanvasView{ new QImageViewer }
    , mObstacleScroll{ nullptr }
    , mVertexScroll{ nullptr }
    , mShapeCombo{ new QComboBox }
    , mCanvasRect{}
    , mCanvasColor(21, 23, 43)
    , mObstacleColor(90, 96, 120)
    , mShapeFillColor(205, 164, 52, 200)
    , mShapeEdgeColor(244, 197, 66)
{
    // Groupe "Parametres"
    QGroupBox* parameterGroupBox{ new QGroupBox("Definition des Parametres") };
    QFormLayout* parameterLayout{ new QFormLayout };

    // -----------------------------------------------------------------
    // Nombre d'obstacles  :  [scrollbar]  [valeur]  [bouton Regenerer]
    // -----------------------------------------------------------------
    mObstacleScroll = new QScrollBar;
    mObstacleScroll->setOrientation(Qt::Horizontal);
    mObstacleScroll->setRange(0, 360);
    mObstacleScroll->setValue(120);
    mObstacleScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QLabel* obstacleLabel{ new QLabel(QString::number(120)) };
    obstacleLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QPushButton* regenButton{ new QPushButton("Regenerer") };
    // QPushButton* regenButton{ new QPushButton(QStringLiteral("R?g?n?rer")) };

    QHBoxLayout* obstacleRow{ new QHBoxLayout };
    obstacleRow->addWidget(mObstacleScroll);
    obstacleRow->addWidget(obstacleLabel);
    obstacleRow->addWidget(regenButton);

    parameterLayout->addRow("Nombre d'obstacles", obstacleRow);

    // -----------------------------------------------------------------
    // Forme geometrique + nombre de sommets
    // -----------------------------------------------------------------
    QHBoxLayout* shapeRow{ new QHBoxLayout };

    mShapeCombo->addItem("Polygone regulier");
    mShapeCombo->addItem("Polygone convexe");
    mShapeCombo->addItem("Polygone etoile");
    shapeRow->addWidget(mShapeCombo);

    QHBoxLayout* vertexRow{
        buildScrollBarLayout(mVertexScroll, 3, 20, 14, " sommets")
    };
    shapeRow->addLayout(vertexRow);

    parameterLayout->addRow("Forme geometrique", shapeRow);
    parameterGroupBox->setLayout(parameterLayout);

    // Groupe "Visualisation"
    QGroupBox* visualizationGroupBox{ new QGroupBox("Visualisation") };
    QVBoxLayout* visualizationLayout{ new QVBoxLayout };
    visualizationLayout->addWidget(mCanvasView);
    visualizationGroupBox->setLayout(visualizationLayout);

    // Layout principal
    QVBoxLayout* mainLayout{ new QVBoxLayout(this) };
    mainLayout->addWidget(parameterGroupBox);
    mainLayout->addWidget(visualizationGroupBox);

    // --------------------------------------------------------------
    // Connexions
    // --------------------------------------------------------------

    // Scrollbar d'obstacles : met a jour la valeur et signale un changement
    connect(mObstacleScroll, &QScrollBar::valueChanged,
        obstacleLabel, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));
    connect(mObstacleScroll, &QScrollBar::valueChanged,
        this, &QDESolutionPanel::parameterChanged);

    // Scrollbar de sommets + combo
    connect(mVertexScroll, &QScrollBar::valueChanged,
        this, &QDESolutionPanel::parameterChanged);
    connect(mShapeCombo, &QComboBox::currentIndexChanged,
        this, &QDESolutionPanel::parameterChanged);

    // Obstacles : ne se regenerent que sur clic du bouton
    connect(regenButton, &QPushButton::clicked,
        this, &QDEGeometricOptimisationPanel::regenerateObstacles);

    // Polygones : se reconstruisent lorsque le nombre de sommets ou le type change
    connect(mVertexScroll, &QScrollBar::valueChanged,
        this, &QDEGeometricOptimisationPanel::rebuildPolygons);
    connect(mShapeCombo, &QComboBox::currentIndexChanged,
        this, &QDEGeometricOptimisationPanel::rebuildPolygons);

    // Redessiner automatiquement lorsque le canevas est redimensionn?
    connect(mCanvasView, &QImageViewer::resized,
        this, [this]()
        {
            updateCanvasRect();
            drawPreview();
        });

    // Initialisation differ?e pour avoir la taille finale du canevas
    QTimer::singleShot(0, this, [this]()
        {
            updateCanvasRect();
            regenerateObstacles();
            rebuildPolygons();
        });
}


//======================================================================
//  Outils internes
//======================================================================

QHBoxLayout* QDEGeometricOptimisationPanel::buildScrollBarLayout(
    QScrollBar*& sb, int min, int max, int defValue, QString const& suffix)
{
    sb = new QScrollBar;
    sb->setOrientation(Qt::Horizontal);
    sb->setRange(min, max);
    sb->setValue(defValue);
    sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QLabel* label{ new QLabel };
    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout* layout{ new QHBoxLayout };
    layout->addWidget(sb);
    layout->addWidget(label);

    connect(sb, &QScrollBar::valueChanged, label,
        [label, suffix](int value)
        {
            if (suffix.isEmpty())
                label->setText(QString::number(value));
            else
                label->setText(QString::number(value) + suffix);
        });

    return layout;
}

void QDEGeometricOptimisationPanel::updateCanvasRect()
{
    QSize size(mCanvasView->size().width() - 1,
        mCanvasView->size().height() - 1);
    if (size.width() < 1)  size.setWidth(1);
    if (size.height() < 1) size.setHeight(1);

    mCanvasRect = QRectF(QPointF(0.0, 0.0), size);
}

QDEGeometricOptimisationPanel::polygoneMode
QDEGeometricOptimisationPanel::currentShapeKind() const
{
    switch (mShapeCombo->currentIndex()) {
    case 0:  return polygoneMode::Regular;
    case 1:  return polygoneMode::Convex;
    case 2:  return polygoneMode::Star;
    default: return polygoneMode::Regular;
    }
}

Polygon* QDEGeometricOptimisationPanel::currentPolygone() const
{
    int index = mShapeCombo->currentIndex();
    if (index < 0 || index >= mPolygones.size())
        return nullptr;
    return mPolygones[index];
}

void QDEGeometricOptimisationPanel::regenerateObstacles()
{
    mObstaclePoints.clear();

    int const count = mObstacleScroll->value();
    if (count <= 0 || !mCanvasRect.isValid()) {
        drawPreview();
        return;
    }

    for (int i = 0; i < count; ++i) {
        double x = QRandomGenerator::global()->bounded(mCanvasRect.width());
        double y = QRandomGenerator::global()->bounded(mCanvasRect.height());
        mObstaclePoints << QPointF(x, y);
    }

    drawPreview();
}

void QDEGeometricOptimisationPanel::rebuildPolygons()
{
    for (Polygon* p : mPolygones)
        delete p;
    mPolygones.clear();

    size_t n = static_cast<size_t>(mVertexScroll->value());

    mPolygones.push_back(new PolygoneRegulier(n, mShapeFillColor, mShapeEdgeColor));
    mPolygones.push_back(new PolygoneConvexe(n, mShapeFillColor, mShapeEdgeColor));
    mPolygones.push_back(new PolygoneEtoile(n, mShapeFillColor, mShapeEdgeColor));

    if (Polygon* p = currentPolygone())
        mBasePolygon = p->basePolygon();
    else
        mBasePolygon.clear();

    drawPreview();
}

void QDEGeometricOptimisationPanel::drawPreview()
{
    updateCanvasRect();

    QSize const size = mCanvasRect.size().toSize();
    if (!size.isValid())
        return;

    QPixmap pixmap(size);
    pixmap.fill(mCanvasColor);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(mObstacleColor);
    double const obstacleRadius = 3.0;
    for (QPointF const& pt : mObstaclePoints)
        painter.drawEllipse(pt, obstacleRadius, obstacleRadius);

    if (!mBasePolygon.isEmpty()) {
        painter.save();
        painter.translate(mCanvasRect.center());

        painter.setBrush(mShapeFillColor);
        QPen pen(mShapeEdgeColor, 0.0);
        pen.setCosmetic(true);
        painter.setPen(pen);

        double const maxSide = std::min(mCanvasRect.width(), mCanvasRect.height());
        double const initialScale = 0.4 * maxSide;
        painter.scale(initialScale, initialScale);

        painter.drawPolygon(mBasePolygon);
        painter.restore();
    }

    mCanvasView->setPixmap(pixmap);
}


//======================================================================
//  buildSolution
//======================================================================

de::SolutionStrategy* QDEGeometricOptimisationPanel::buildSolution() const
{
    auto* self = const_cast<QDEGeometricOptimisationPanel*>(this);
    self->updateCanvasRect();

    return new geometricOptimisationStrategy(self->mCanvasRect,
        self->mObstaclePoints,
        self->mBasePolygon);
}


//======================================================================
//  updateVisualization : pendant la simulation
//======================================================================

void QDEGeometricOptimisationPanel::updateVisualization(QDEAdapter const& de)
{
    updateCanvasRect();

    QSize const size = mCanvasRect.size().toSize();
    if (!size.isValid())
        return;

    QPixmap pixmap(size);
    pixmap.fill(mCanvasColor);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(mObstacleColor);
    double const obstacleRadius = 3.0;
    for (QPointF const& pt : mObstaclePoints)
        painter.drawEllipse(pt, obstacleRadius, obstacleRadius);

    auto const& pop = de.actualPopulation();

    if (!pop.size() || mBasePolygon.isEmpty()) {
        painter.save();
        painter.translate(mCanvasRect.center());

        painter.setBrush(mShapeFillColor);
        QPen pen(mShapeEdgeColor, 0.0);
        pen.setCosmetic(true);
        painter.setPen(pen);

        double const maxSide = std::min(mCanvasRect.width(), mCanvasRect.height());
        double const initialScale = 0.4 * maxSide;
        painter.scale(initialScale, initialScale);

        painter.drawPolygon(mBasePolygon);
        painter.restore();
    }

    for (size_t i = 0; i < pop.size(); ++i) {
        double tx = pop[i][0];
        double ty = pop[i][1];
        double angle = pop[i][2];
        double s = pop[i][3];

        QTransform tr;
        tr.translate(tx, ty);
        tr.rotate(angle);
        tr.scale(s, s);

        QPolygonF poly = tr.map(mBasePolygon);

        painter.save();
        if (i == 0) {
            painter.setBrush(mShapeFillColor);
            QPen bestPen(mShapeEdgeColor, 1.5);
            bestPen.setCosmetic(true);
            painter.setPen(bestPen);
        }
        else {
            QPen otherPen(mShapeEdgeColor, 1.0, Qt::DashLine);
            otherPen.setCosmetic(true);
            painter.setBrush(Qt::NoBrush);
            painter.setPen(otherPen);
        }
        painter.drawPolygon(poly);
        painter.restore();
    }

    mCanvasView->setPixmap(pixmap);
}


//======================================================================
//  geometricOptimisationStrategy
//======================================================================

QDEGeometricOptimisationPanel::geometricOptimisationStrategy::
geometricOptimisationStrategy(QRectF         canvas,
    QList<QPointF> obstacles,
    QPolygonF      basePolygon)
    : de::SolutionStrategy(
        sTitle,
        sSummary,
        QDEGeometricOptimisationPanel::sDescription.toStdString())
    , mCanvas(std::move(canvas))
    , mObstacles(std::move(obstacles))
    , mBasePolygon(std::move(basePolygon))
{
    mSolutionDomain.resize(4);
    mSolutionDomain[0].set(0.0, mCanvas.width());
    mSolutionDomain[1].set(0.0, mCanvas.height());
    mSolutionDomain[2].set(0.0, 360.0);
    mSolutionDomain[3].set(0.0, 1.0);

    setOptimizationStrategy(new de::OptimizationMaximization);
    setFitnessStrategy(new de::FitnessIdentity);
}

double QDEGeometricOptimisationPanel::geometricOptimisationStrategy::process(
    de::Solution const& solution)
{
    double const tx = solution[0];
    double const ty = solution[1];
    double const angle = solution[2];
    double const s = solution[3];

    QTransform tr;
    tr.translate(tx, ty);
    tr.rotate(angle);
    tr.scale(s, s);

    QPolygonF transformed = tr.map(mBasePolygon);

    if (!mCanvas.contains(transformed.boundingRect()))
        return -s;

    for (QPointF const& p : mObstacles) {
        if (transformed.containsPoint(p, Qt::OddEvenFill))
            return -s;
    }

    return s;
}

std::string QDEGeometricOptimisationPanel::geometricOptimisationStrategy::toString(
    de::Solution const& solution) const
{
    double const tx = solution[0];
    double const ty = solution[1];
    double const angle = solution[2];
    double const s = solution[3];

    return std::format(
        "Taille du canevas : {:.1f} x {:.1f}\n"
        "Translation finale : ({:.3f}, {:.3f})\n"
        "Rotation finale    : {:.3f} deg\n"
        "Echelle finale     : {:.6f}",
        mCanvas.width(), mCanvas.height(),
        tx, ty, angle, s);
}
