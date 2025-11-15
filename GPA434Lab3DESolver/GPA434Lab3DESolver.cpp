#include "GPA434Lab3DESolver.h"

#include <QWidget>
#include <QLabel>
#include <QScrollBar>
#include <QHBoxLayout>

#include "QDEControllerPanel.h"
#include "QDEEngineParametersPanel.h"
#include "QDEBestResultPanel.h"

GPA434Lab3DESolver::GPA434Lab3DESolver(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Differential Evolution Solver");
    setWindowIcon(QIcon(":/GPA434Lab3DESolver/dna-icon"));

    QWidget* mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);

    QWidget* parametersWidget = new QWidget;
    QVBoxLayout* parametersLayout = new QVBoxLayout(parametersWidget);

    QDEAdapter* adapter = new QDEAdapter(this);
    QDEControllerPanel* controllerPanel = new QDEControllerPanel(*adapter, parametersWidget);
    QDEEngineParametersPanel* engineParametersPanel = new QDEEngineParametersPanel(*adapter, parametersWidget);
    QDEBestResultPanel* bestResultPanel = new QDEBestResultPanel(*adapter, parametersWidget);

    parametersLayout->addWidget(controllerPanel);
    parametersLayout->addWidget(engineParametersPanel);
    parametersLayout->addWidget(bestResultPanel);
    parametersWidget->setLayout(parametersLayout);

    mainLayout->addWidget(parametersWidget);
    mainWidget->setLayout(mainLayout);
}

GPA434Lab3DESolver::~GPA434Lab3DESolver()
{
}
