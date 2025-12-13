#include "QDEGeometricOptimisationPanel.h"

#include "QImageViewer.h"

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
#include <QPainterPath>


QDEGeometricOptimisationPanel::QDEGeometricOptimisationPanel(QWidget* parent)
    : QDESolutionPanel(parent)
    , mVisualizationLabel{ new QImageViewer }
    , mObstacleScrollBar{ new QScrollBar }
    , mVertexScrollBar{ new QScrollBar }
    , mShapeComboBox{ new QComboBox }
    , regenerateButton{ new QPushButton }
    , mCanvasRect{}
    , mCanvasColor(21, 23, 43)
    , mObstacleColor(90, 96, 120)
    , mShapeFillColor(205, 164, 52, 200)
    , mShapeEdgeColor(244, 197, 66)
    , mCurrentPolygon{ nullptr }
{
    // Create the 'input parameters' group box and its contents
    QGroupBox* parameterGroupBox{ new QGroupBox("Définitien des paramètres") };
    QFormLayout* parameterLayout{ new QFormLayout(parameterGroupBox) };

    // 
    QHBoxLayout* obstaclesLayout{ new QHBoxLayout };
    obstaclesLayout->addWidget(buildScrollBarWidget(mObstacleScrollBar, 0, 360, 120, " "));
    //QPushButton* regenerateButton{ new QPushButton("Regenerer") };
    regenerateButton->setText(QString("Regénérer"));
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
    parameterLayout->addRow("Forme géométrique", shapeLayout);



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

    QTimer::singleShot(0, this,
        [this]()
        {
            updateCanvasRect();
            regenerateObstacles();
            updateSelectedPolygon();
            emit parameterChanged();
        });

}


int QDEGeometricOptimisationPanel::obstacleCount() const
{
    return static_cast<int>(mObstacleScrollBar->value());
}

int QDEGeometricOptimisationPanel::vertexCount() const
{
    return static_cast<int>(mVertexScrollBar->value());
}


//======================================================================
//  Outils internes
//======================================================================

QWidget* QDEGeometricOptimisationPanel::buildScrollBarWidget(
    QScrollBar*& sb, int minValue, int maxValue, int defValue, QString const& suffix)
{
    QWidget* widget = new QWidget();
    QHBoxLayout* layout{ new QHBoxLayout(widget) };

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
    connect(mVertexScrollBar, &QScrollBar::valueChanged,
        this, &QDESolutionPanel::parameterChanged);
    connect(mShapeComboBox, &QComboBox::currentIndexChanged,
        this, &QDESolutionPanel::parameterChanged);


    connect(regenerateButton, &QPushButton::clicked, this,
        [this]()
        {
            regenerateObstacles();
            emit parameterChanged();
        });

    connect(mVertexScrollBar, &QScrollBar::valueChanged,
        this, &QDEGeometricOptimisationPanel::updateSelectedPolygon);
    connect(mShapeComboBox, &QComboBox::currentIndexChanged,
        this, &QDEGeometricOptimisationPanel::updateSelectedPolygon);

    connect(mVisualizationLabel, &QImageViewer::resized, this,
        [this]()
        {
            constexpr double kAspect = 4.0;
            int h = int(mVisualizationLabel->width() / kAspect);
            if (mVisualizationLabel->height() != h) mVisualizationLabel->setFixedHeight(h);

            updateCanvasRect();
            drawPreview();
        });

    connect(mObstacleScrollBar, &QScrollBar::valueChanged, this,
        [this](int)
        {
            regenerateObstacles();
            emit parameterChanged();
        });
}










void QDEGeometricOptimisationPanel::updateCanvasRect()
{
    // NOTE : QWidget size is integer-based
    const int w = mVisualizationLabel->size().width() - 1;
    const int h = mVisualizationLabel->size().height() - 1;

    mCanvasRect = QRectF(QPointF(0.0, 0.0), QSize((w < 1) ? 1 : w, (h < 1) ? 1 : h));
}

QDEGeometricOptimisationPanel::polygoneMode QDEGeometricOptimisationPanel::currentShapeKind() const
{
    switch (mShapeComboBox->currentIndex()) {
    case 1:  return polygoneMode::Convex;
    case 2:  return polygoneMode::Star;
    default: return polygoneMode::Regular;
    }
}

void QDEGeometricOptimisationPanel::regenerateObstacles()
{
    const int count = obstacleCount();

    mObstaclePoints.clear();

    if (count > 0 && mCanvasRect.isValid()) {
        const double w = mCanvasRect.width();
        const double h = mCanvasRect.height();
        auto* rgg = QRandomGenerator::global();

        mObstaclePoints.reserve(count); // Reserve enough memory to avoid reallocation

        for (int i = 0; i < count; ++i) {
            mObstaclePoints.append(QPointF(rgg->bounded(w), rgg->bounded(h)));
        }
    }
    drawPreview();
}

void QDEGeometricOptimisationPanel::updateSelectedPolygon()
{
    polygoneMode polygonType = currentShapeKind();
    const int vertexSize = vertexCount();

    switch (polygonType)
    {
    case polygoneMode::Convex:
        mCurrentPolygon = std::unique_ptr<Polygon>(new ConvexPolygon(vertexSize, mShapeFillColor, mShapeEdgeColor));
        break;
    case polygoneMode::Star:
        mCurrentPolygon = std::unique_ptr<Polygon>(new StarPolygon(vertexSize, mShapeFillColor, mShapeEdgeColor));
        break;
    default:
        mCurrentPolygon = std::unique_ptr<Polygon>(new RegularPolygon(vertexSize, mShapeFillColor, mShapeEdgeColor));
        break;
    }

    mBasePolygon = mCurrentPolygon->basePolygon();
    drawPreview();
}

void QDEGeometricOptimisationPanel::renderScene(DrawMode mode, const QDEAdapter* adapter)
{
    if (!mCurrentPolygon)
        return;   // or drawPreview();

    // Assure que le rectangle du canevas reflète la vraie taille actuelle du widget
    updateCanvasRect();

    // Calcul de la taille de l’image dans laquelle on va dessiner
    const QSize size = mCanvasRect.size().toSize();
    if (!size.isValid())
        return;

    // Pixmap est l'image temporaire en mémoire, sur laquelle tout sera dessiné
    QPixmap pixmap(size);
    pixmap.fill(mCanvasColor);

    // QPainter est l’outil principal de dessin
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // --- Dessin des obstacles ---
    painter.setPen(Qt::NoPen);
    painter.setBrush(mObstacleColor);
    const double obstacleRadius = 3.0;

    for (const QPointF& pos : mObstaclePoints)
        painter.drawEllipse(pos, obstacleRadius, obstacleRadius);

    // --- Dessin du polygone statique ---
    if (mode == DrawMode::Preview && !mBasePolygon.isEmpty()) {
        painter.save(); // sauvegarde l’état (position, rotation, scale…)

        // On dessine le polygone centré
        painter.translate(mCanvasRect.center());

        painter.setBrush(mShapeFillColor);

        QPen pen(mShapeEdgeColor, 0.0);
        pen.setCosmetic(true);
        painter.setPen(pen);

        const double max = std::min(mCanvasRect.width(), mCanvasRect.height());
        const double scale = 0.4 * max;

        painter.scale(scale, scale);
        painter.drawPolygon(mBasePolygon);

        painter.restore(); // revient à l’état précédant le translate+scale
    }

    // Dessin de la simulation 
    if (mode == DrawMode::Simulation && adapter != nullptr)
    {
        const auto& population = adapter->actualPopulation();

        for (size_t i = 0; i < population.size(); ++i)
        {
            double tx = population[i][0];
            double ty = population[i][1];
            double angle = population[i][2];
            double s = population[i][3];

            // Matrice de transformation 2D (translation + rotation + scale)
            // Applique la transformation au polygone de base.
            QPolygonF transformed = mCurrentPolygon->applyTransform(tx, ty, angle, s);

            painter.save();

            if (i == 0)
            {
                // Meilleure solution  dessin plus visible
                painter.setBrush(mShapeFillColor);
                QPen bestPen(mShapeEdgeColor, 1.5);
                bestPen.setCosmetic(true);
                painter.setPen(bestPen);
            }
            else {
                painter.setBrush(Qt::NoBrush);
                QPen pen(QColor(180, 180, 180, 60), 1.0, Qt::SolidLine);
                pen.setCosmetic(true);
                painter.setPen(pen);
            }

            painter.drawPolygon(transformed);
            painter.restore();
        }
    }

    mVisualizationLabel->setPixmap(pixmap);
}

void QDEGeometricOptimisationPanel::drawPreview()
{
    renderScene(DrawMode::Preview, nullptr);
}

void QDEGeometricOptimisationPanel::updateVisualization(QDEAdapter const& de)
{
    if (de.currentGeneration() == 0) {
        drawPreview();
        return;
    }

    renderScene(DrawMode::Simulation, &de);
}



// buildSolution() is called when the Differential Evolution solver wants to 
// start optimizing your geometric transformation (scale, rotation, translation) 
// for your polygon.
de::SolutionStrategy* QDEGeometricOptimisationPanel::buildSolution() const
{
    auto* self = const_cast<QDEGeometricOptimisationPanel*>(this);
    self->updateCanvasRect();

    return new geometricOptimisationStrategy(self->mCanvasRect, self->mObstaclePoints, self->mBasePolygon);
}



#include <OptimizationMaximization.h>
#include <FitnessIdentity.h>
#include <format>

static const std::string gopTitle("Maximisation de forme");
static const std::string gopSummary("<p>Ce probleme consiste a determiner les parametres de transformation affine afin qu'une forme maximise sa taille sans depasser le canevas ou englober un obstacle.</p>");
static const std::string gop_description(R".(<p>La solution est representee dans un espace a quatre dimensions, où chaque composante correspond à un paramètre de transformation affine :</p><ul><li>x : déplacement horizontal;</li><li>y : déplacement vertical;</li><li>angle : rotation en degrés;</li><li>s : facteur d'échelle uniforme.</li></ul><p>La fonction objective utilise directement la valeur de s. Une pénalité négative est appliquée lorsqu'une transformation entraîne un dépassement du canevas ou recouvre un obstacle.</p>).");

const QString QDEGeometricOptimisationPanel::gop_description =
QString::fromUtf8(R".(<p>La solution est representee dans un espace a quatre dimensions, où chaque composante correspond à un paramètre de transformation affine :</p><ul><li>x : déplacement horizontal;</li><li>y : déplacement vertical;</li><li>angle : rotation en degrés;</li><li>s : facteur d'échelle uniforme.</li></ul><p>La fonction objective utilise directement la valeur de s. Une pénalité négative est appliquée lorsqu'une transformation entraîne un dépassement du canevas ou recouvre un obstacle.</p>).");



QDEGeometricOptimisationPanel::geometricOptimisationStrategy::geometricOptimisationStrategy
(const QRectF& canvas, const QList<QPointF>& obstacles, const QPolygonF& basePolygon)
    : de::SolutionStrategy(
        gopTitle,
        gopSummary,
        QDEGeometricOptimisationPanel::gop_description.toStdString())
    , mCanvas(canvas)
    , mObstacles(obstacles)
    , mBasePolygon(basePolygon)
{
    mSolutionDomain.resize(4);
    // 4 parameters in domain(x, y, angle, scale)
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

    // Polygone dans la zone affichable
    if (!mCanvas.contains(transformed.boundingRect()))
        return -s;

    // Obstacles traités comme des points : un obstacle est interdit seulement s'il est à l'intérieur du polygone
    QRectF const br = transformed.boundingRect(); // Filtre rapide avant containsPoint()

    for (QPointF const& p : mObstacles)
    {
        if (!br.contains(p))
            continue;

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
