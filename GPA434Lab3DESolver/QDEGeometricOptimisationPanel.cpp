#include "QDEGeometricOptimisationPanel.h"

#include "QImageViewer.h"
#include "Polygon.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>
#include <QtMath> /// COULD REMOVE
#include <QTimer>


QDEGeometricOptimisationPanel::QDEGeometricOptimisationPanel(QWidget* parent)
    : QDESolutionPanel(parent)
    , mVisualizationLabel{ new QImageViewer }
    , mObstacleScrollBar{ new QScrollBar }
    , mVertexScrollBar{ new QScrollBar }
    , mShapeComboBox{ new QComboBox }
    , regenerateButton { new QPushButton }
    , mCanvasRect{}
    , mCanvasColor(21, 23, 43)
    , mObstacleColor(90, 96, 120)
    , mShapeFillColor(205, 164, 52, 200)
    , mShapeEdgeColor(244, 197, 66)
{
    // Create the 'input parameters' group box and its contents
    QGroupBox* parameterGroupBox{ new QGroupBox("Paramètres") };
    QFormLayout* parameterLayout{ new QFormLayout(parameterGroupBox)};

    // 
    QHBoxLayout* obstaclesLayout{ new QHBoxLayout };
    obstaclesLayout->addWidget(buildScrollBarWidget(mObstacleScrollBar, 0, 360, 120, " "));
    //QPushButton* regenerateButton{ new QPushButton("Regenerer") };
    regenerateButton->setText(QString("Regenerer"));
    obstaclesLayout->addWidget(regenerateButton);
    parameterLayout->addRow("Nombre d'obstacles", obstaclesLayout);

    //
    mShapeComboBox->addItem("Polygone regulier");
    mShapeComboBox->addItem("Polygone convexe");
    mShapeComboBox->addItem("Polygone etoile");

    QWidget* vertexRow{ buildScrollBarWidget(mVertexScrollBar, 3, 20, 14, " sommets") };
    QHBoxLayout* shapeLayout{ new QHBoxLayout };
    shapeLayout->addWidget(mShapeComboBox);
    shapeLayout->addWidget(vertexRow);
    parameterLayout->addRow("Forme geometrique", shapeLayout);



    // Groupe "Visualisation"
    QGroupBox* visualizationGroupBox{ new QGroupBox("Visualisation") };
    QVBoxLayout* visualizationLayout{ new QVBoxLayout };
    visualizationLayout->addWidget(mVisualizationLabel);
    visualizationGroupBox->setLayout(visualizationLayout);

    // Layout principal
    QVBoxLayout* mainLayout{ new QVBoxLayout(this) };
    mainLayout->addWidget(parameterGroupBox);
    mainLayout->addWidget(visualizationGroupBox);

    // --------------------------------------------------------------
    // Connexions
    // --------------------------------------------------------------
    establishConnections();


    // Initialisation differ?e pour avoir la taille finale du canevas
    // "Run this code after the event loop returns, once the layout has been applied and the widget has a valid size."
    QTimer::singleShot(0, this, 
        [this]()
        {
            updateCanvasRect();
            regenerateObstacles();
            rebuildPolygons();
        });
}

/// MARIA ADDED -----------------------------------------

double QDEGeometricOptimisationPanel::obstacleCount() const
{
    return static_cast<double>(mObstacleScrollBar->value());
}

double QDEGeometricOptimisationPanel::vertexCount() const
{
    return static_cast<double>(mVertexScrollBar->value());
}

//
//de::SolutionStrategy* QDEOpenBoxPanel::buildSolution() const
//{
//    return new OpenBoxStrategy(mWidthScrollBar->value(), mHeightScrollBar->value());
//}
/// MARIA ADDED -----------------------------------------


//======================================================================
//  Outils internes
//======================================================================

QWidget* QDEGeometricOptimisationPanel::buildScrollBarWidget(
    QScrollBar*& sb, int minValue, int maxValue, int defValue, QString const& suffix)
{
    QWidget* widget = new QWidget();
    QHBoxLayout* layout{ new QHBoxLayout(widget)};

    sb->setOrientation(Qt::Horizontal);
    sb->setRange(minValue, maxValue);
    sb->setValue(defValue);
    sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sb->setMinimumHeight(20);
    QLabel* label{ new QLabel(QString::number(defValue)) };
    //QLabel* label{ new QLabel };
    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    layout->addWidget(sb);
    layout->addWidget(label);

    label->setText(QString::number(sb->value()) + suffix);// Init
    
    // value envoyée par le signal valueChanged
    connect(sb, &QScrollBar::valueChanged, label,
        [=](int value) {label->setText(QString::number(value) + suffix); });

    return widget;
}


void QDEGeometricOptimisationPanel::establishConnections()
{

    connect(mObstacleScrollBar, &QScrollBar::valueChanged, this, &QDESolutionPanel::parameterChanged);
    connect(mVertexScrollBar, &QScrollBar::valueChanged, this, &QDESolutionPanel::parameterChanged);
    connect(mShapeComboBox, &QComboBox::currentIndexChanged, this, &QDESolutionPanel::parameterChanged);

    // Obstacles : ne se regenerent que sur clic du bouton
    connect(regenerateButton, &QPushButton::clicked, this, &QDEGeometricOptimisationPanel::regenerateObstacles);

    // Polygones : se reconstruisent lorsque le nombre de sommets ou le type change
    connect(mVertexScrollBar, &QScrollBar::valueChanged, this, &QDEGeometricOptimisationPanel::rebuildPolygons);
    connect(mShapeComboBox, &QComboBox::currentIndexChanged, this, &QDEGeometricOptimisationPanel::updateSelectedPolygon);

    // Redessiner automatiquement lorsque le canevas est redimensionn?
    connect(mVisualizationLabel, &QImageViewer::resized, this, 
        [this]()
        {
            updateCanvasRect();
            drawPreview();
        });
}









void QDEGeometricOptimisationPanel::updateCanvasRect()
{
    // NOTE : QWidget size is integer-based
    const int w = mVisualizationLabel->size().width() - 1;
    const int h = mVisualizationLabel->size().height() - 1;

    QSize size((w < 1) ? 1 : w, (h < 1) ? 1 : h);

    mCanvasRect = QRectF(QPointF(0.0, 0.0), size);
}

QDEGeometricOptimisationPanel::polygoneMode QDEGeometricOptimisationPanel::currentShapeKind() const
{
    switch (mShapeComboBox->currentIndex()) {
        case 1:  return polygoneMode::Convex;
        case 2:  return polygoneMode::Star;
        default: return polygoneMode::Regular;
    }
}

Polygon* QDEGeometricOptimisationPanel::currentPolygone() const
{
    const int index = mShapeComboBox->currentIndex();
    const int count = static_cast<int>(mPolygones.size());
    // NOTE : Comparing index >= mPolygones.size() causes a signed/unsigned warning.

    return (index >= 0 && index < count) ? mPolygones[index] : nullptr;
}

void QDEGeometricOptimisationPanel::regenerateObstacles()
{
    const int obstacleCount = mObstacleScrollBar->value();

    mObstaclePoints.clear();

    if (obstacleCount > 0 && mCanvasRect.isValid()) {
        const double w = mCanvasRect.width();
        const double h = mCanvasRect.height();
        auto* rgg = QRandomGenerator::global();

        mObstaclePoints.reserve(obstacleCount); // Reserve enough memory to avoid reallocation

        for (int i = 0; i < obstacleCount; ++i) {
            mObstaclePoints.append(QPointF(rgg->bounded(w), rgg->bounded(h)));
        }
    }
    drawPreview();
}

void QDEGeometricOptimisationPanel::rebuildPolygons()
{
    for (Polygon* p : mPolygones)
        delete p;
    mPolygones.clear();

    size_t n = static_cast<size_t>(mVertexScrollBar->value());

    mPolygones.push_back(new PolygoneRegulier(n, mShapeFillColor, mShapeEdgeColor));
    mPolygones.push_back(new PolygoneConvexe(n, mShapeFillColor, mShapeEdgeColor));
    mPolygones.push_back(new PolygoneEtoile(n, mShapeFillColor, mShapeEdgeColor));

    updateSelectedPolygon();
}

void QDEGeometricOptimisationPanel::updateSelectedPolygon()
{
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

    mVisualizationLabel->setPixmap(pixmap);
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

    mVisualizationLabel->setPixmap(pixmap);
}










#include <OptimizationMaximization.h>
#include <FitnessIdentity.h>
#include <format>

// Pour de::SolutionStrategy
static const std::string gopTitle{ "Optimisation géométrique" };
static const std::string gopSummary{ "<p>Ce probleme consiste a determiner les parametres de transformation affine afin qu'une forme maximise sa taille sans depasser le canevas ou englober un obstacle.</p>" };
static const std::string gop_description{ "<p>DESCRIPTION.</p>" };

const QString QDEGeometricOptimisationPanel::gop_description =
QStringLiteral(
    "<p>La solution est representee dans un espace … quatre dimensions, o— "
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
//  geometricOptimisationStrategy
//======================================================================

QDEGeometricOptimisationPanel::geometricOptimisationStrategy::
geometricOptimisationStrategy(QRectF canvas,
    QList<QPointF> obstacles,
    QPolygonF      basePolygon)
    : de::SolutionStrategy(
        gopTitle,
        gopSummary,
        QDEGeometricOptimisationPanel::gop_description.toStdString())
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
        mCanvas.width(), mCanvas.height(), tx, ty, angle, s);
}
