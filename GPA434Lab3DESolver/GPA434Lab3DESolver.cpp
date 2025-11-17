#include "GPA434Lab3DESolver.h"

#include <QWidget>
#include <QLabel>
#include <QScrollBar>
#include <QHBoxLayout>

#include "QDEControllerPanel.h"
#include "QDEEngineParametersPanel.h"
#include "QDEBestResultPanel.h"
#include "qsplitter.h"
#include "QDESolutionTabPanel.h"
#include "QDESolutionPanel.h"
#include "QDEHistoryChartPanel.h"

GPA434Lab3DESolver::GPA434Lab3DESolver(QWidget* parent)
    : QMainWindow(parent)
    , mAdapter{ new QDEAdapter(this) }
    , mParametersWidget{ new QWidget }
    , mControllerPanel{ new QDEControllerPanel(*mAdapter, this) }
    , mEngineParametersPanel{ new QDEEngineParametersPanel(*mAdapter, this) }
    , mBestResultPanel{ new QDEBestResultPanel(*mAdapter, mParametersWidget) }
    , mPanelSplitter{ new QSplitter(Qt::Orientation::Vertical, this) }
    , mSolutionTabPanel{ new QDESolutionTabPanel(*mAdapter, this) }
    , mHistoryChartPanel{ new QDEHistoryChartPanel(*mAdapter, this) }
{
    setWindowTitle("Differential Evolution Solver");
    setWindowIcon(QIcon(":/GPA434Lab3DESolver/dna-icon"));

    setupGUI();
    assemblingAndLayouting();
}

void GPA434Lab3DESolver::setupGUI()
{
    mParametersWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    //mPanelSplitter->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
}

void GPA434Lab3DESolver::assemblingAndLayouting()
{
    QWidget* mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);

    QVBoxLayout* parametersLayout = new QVBoxLayout(mParametersWidget);
    parametersLayout->addWidget(mControllerPanel);
    parametersLayout->addWidget(mEngineParametersPanel);
    parametersLayout->addWidget(mBestResultPanel);
    mParametersWidget->setLayout(parametersLayout);

    mPanelSplitter->addWidget(mSolutionTabPanel);
    mPanelSplitter->addWidget(mHistoryChartPanel);
    mPanelSplitter->setStretchFactor(0, 60);
    mPanelSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(mParametersWidget, 1);
    mainLayout->addWidget(mPanelSplitter, 4);
    mainWidget->setLayout(mainLayout);
}

GPA434Lab3DESolver::~GPA434Lab3DESolver()
{
}
