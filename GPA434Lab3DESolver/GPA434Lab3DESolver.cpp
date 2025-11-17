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
    establishConnections();
}

GPA434Lab3DESolver::~GPA434Lab3DESolver(){}



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

void GPA434Lab3DESolver::establishConnections()
{
    //connect(  const typename QtPrivate::FunctionPointer<Func1>::Object * sender, 
    //          Func1 signal,
    //          const typename QtPrivate::ContextTypeForFunctor<Func2>::ContextType * context, 
    //          Func2 && slot,
    //          Qt::ConnectionType type = Qt::AutoConnection)

    connect(mEngineParametersPanel, &QDEEngineParametersPanel::parameterChanged,    mControllerPanel,       &QDEControllerPanel::resetSimulation);
    connect(mEngineParametersPanel, &QDEEngineParametersPanel::parameterChanged,    mSolutionTabPanel,      &QDESolutionTabPanel::updateVisualization);

    connect(mSolutionTabPanel,      &QDESolutionTabPanel::solutionChanged,          mEngineParametersPanel, &QDEEngineParametersPanel::setParametersFromSolution);
    connect(mSolutionTabPanel,      &QDESolutionTabPanel::solutionChanged,          mControllerPanel,       &QDEControllerPanel::resetSimulation);

    connect(mControllerPanel,       &QDEControllerPanel::evolutionStarted,          mEngineParametersPanel, &QDEEngineParametersPanel::disable);
    connect(mControllerPanel,       &QDEControllerPanel::evolutionStopped,          mEngineParametersPanel, &QDEEngineParametersPanel::enable);
    connect(mControllerPanel,       &QDEControllerPanel::evolutionStarted,          mSolutionTabPanel,      &QDESolutionTabPanel::disable);
    connect(mControllerPanel,       &QDEControllerPanel::evolutionStopped,          mSolutionTabPanel,      &QDESolutionTabPanel::enable);
}