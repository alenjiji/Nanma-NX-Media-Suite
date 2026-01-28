#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QLabel>
#include "adapters/QtCliAdapter.h"

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
    QPlainTextEdit* m_outputText;
    QtCliAdapter m_adapter;
};