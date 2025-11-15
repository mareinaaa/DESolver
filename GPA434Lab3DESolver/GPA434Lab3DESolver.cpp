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
    QHBoxLayout* mainLayout = new QHBoxLayout; 

    QWidget* parametersWidget = new QWidget;
    QVBoxLayout* parametersLayout = new QVBoxLayout;

    QDEAdapter* adapter = new QDEAdapter(this);
    QDEControllerPanel* controllerPanel = new QDEControllerPanel(*adapter, parametersWidget);
    //QDEEngineParametersPanel engineParametersPanel = new QDEEngineParametersPanel();
    //QDEBestResultPanel bestResultPanel = new QDEBestResultPanel();

    parametersLayout->addWidget(controllerPanel);
    controllerPanel->setLayout(parametersLayout);

    parametersWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    parametersWidget->setMinimumSize(1000, 1000);
    controllerPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(controllerPanel);

    mainWidget->setLayout(mainLayout);
    setLayout(mainLayout);
}

GPA434Lab3DESolver::~GPA434Lab3DESolver()
{
}
