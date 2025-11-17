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

GPA434Lab3DESolver::GPA434Lab3DESolver(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Differential Evolution Solver");
    setWindowIcon(QIcon(":/GPA434Lab3DESolver/dna-icon"));

    QWidget* mMainWidget = new QWidget(this);
    setCentralWidget(mMainWidget);

    mAdapter = new QDEAdapter(this);

    mParametersWidget = new QWidget;
    mControllerPanel = new QDEControllerPanel(*mAdapter, mParametersWidget);
    mEngineParametersPanel = new QDEEngineParametersPanel(*mAdapter, mParametersWidget);
    mBestResultPanel = new QDEBestResultPanel(*mAdapter, mParametersWidget);

    mPanelSplitter = new QSplitter(Qt::Orientation::Vertical,mMainWidget);
    mSolutionTabPanel = new QDESolutionTabPanel(*mAdapter, mPanelSplitter);
    mHistoryChartPanel = new QDEHistoryChartPanel(*mAdapter, mPanelSplitter);

    setupGUI();
}

void GPA434Lab3DESolver::setupGUI()
{
    QWidget* mainWidget = centralWidget();

    QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);
    QVBoxLayout* parametersLayout = new QVBoxLayout(mParametersWidget);


    parametersLayout->addWidget(mControllerPanel);
    parametersLayout->addWidget(mEngineParametersPanel);
    parametersLayout->addWidget(mBestResultPanel);
    mParametersWidget->setLayout(parametersLayout);

    mParametersWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));

    //mPanelSplitter->addWidget(mSolutionTabPanel);
    //mPanelSplitter->addWidget(mHistoryChartPanel);
    mPanelSplitter->setStretchFactor(0, 60);
    mPanelSplitter->setStretchFactor(1, 1);


    mainLayout->addWidget(mParametersWidget);
    mainLayout->addWidget(mPanelSplitter);
    mainWidget->setLayout(mainLayout);


}

GPA434Lab3DESolver::~GPA434Lab3DESolver()
{
}
