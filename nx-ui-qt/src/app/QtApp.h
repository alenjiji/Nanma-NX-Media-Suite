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
    void onRunCliTest();

private:
    QPushButton* m_runButton;
    QPlainTextEdit* m_outputText;
    QtCliAdapter m_adapter;
};