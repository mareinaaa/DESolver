#pragma once

#include <QtWidgets/QMainWindow>

#include "QDEControllerPanel.h"
#include "QDEEngineParametersPanel.h"
#include "QDEBestResultPanel.h"
#include "qsplitter.h"
#include "QDESolutionTabPanel.h"
#include "QDESolutionPanel.h"
#include "QDEHistoryChartPanel.h"


class GPA434Lab3DESolver : public QMainWindow
{
    Q_OBJECT

public:
    GPA434Lab3DESolver(QWidget *parent = nullptr);
    ~GPA434Lab3DESolver() override;

private:
    void setupGUI(); // Orientation, range
    void assemblingAndLayouting(); // add
    void establishConnections();
    void addSolution();

    QDEAdapter mAdapter{ nullptr };
    // PAS POINTEUR : QDEAdapter* mAdapter{ nullptr };

    QWidget* mParametersWidget{ nullptr };
    QDEControllerPanel* mControllerPanel{ nullptr };
    QDEEngineParametersPanel* mEngineParametersPanel{ nullptr };
    QDEBestResultPanel* mBestResultPanel{ nullptr };

    QSplitter* mPanelSplitter{ nullptr };
    QDESolutionTabPanel* mSolutionTabPanel{ nullptr };
    QDEHistoryChartPanel* mHistoryChartPanel{ nullptr };
};
