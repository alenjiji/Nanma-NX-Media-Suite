#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include "../adapters/QtCliAdapter.h"
#include "../commands/CommandGroup.h"
#include "../../nx-cli/src/CommandId.h"

class OutputView;

class QtApp : public QMainWindow
{
    Q_OBJECT

public:
    QtApp(QWidget *parent = nullptr);

private slots:
    void onRunVersion();
    void onShowHelp();
    void onRunCommand();

private:
    QString getGroupName(CommandGroup group);
    std::vector<std::string> getArgsForCommand(nx::cli::CommandId commandId);
    
    QPushButton* m_versionButton;
    QPushButton* m_helpButton;
    OutputView* m_outputView;
    QtCliAdapter m_adapter;
};