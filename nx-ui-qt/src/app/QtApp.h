#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include "adapters/QtCliAdapter.h"
#include "ui/OutputView.h"

class QtApp : public QMainWindow
{
    Q_OBJECT

public:
    QtApp(QWidget *parent = nullptr);

private slots:
    void onRunVersion();
    void onShowHelp();

private:
    QPushButton* m_versionButton;
    QPushButton* m_helpButton;
    OutputView* m_outputView;
    QtCliAdapter m_adapter;
};