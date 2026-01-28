#include "QtApp.h"
#include "adapters/QtCliAdapter.h"
#include "ui/OutputView.h"
#include "commands/CommandDescriptor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QWidget>

QtApp::QtApp(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Nanma NX-MediaSuite");
    resize(800, 600);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Header Area
    QLabel* headerLabel = new QLabel("Nanma NX-MediaSuite", this);
    headerLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(headerLabel);
    
    // Actions Panel
    QGroupBox* actionsGroup = new QGroupBox("Actions", this);
    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsGroup);
    
    m_versionButton = new QPushButton("Run Version", this);
    m_helpButton = new QPushButton("Show Help", this);
    
    actionsLayout->addWidget(m_versionButton);
    actionsLayout->addWidget(m_helpButton);
    
    mainLayout->addWidget(actionsGroup);
    
    // Output Panel
    QGroupBox* outputGroup = new QGroupBox("Output", this);
    QVBoxLayout* outputLayout = new QVBoxLayout(outputGroup);
    
    m_outputView = new OutputView(this);
    outputLayout->addWidget(m_outputView);
    
    mainLayout->addWidget(outputGroup);
    
    connect(m_versionButton, &QPushButton::clicked, this, &QtApp::onRunVersion);
    connect(m_helpButton, &QPushButton::clicked, this, &QtApp::onShowHelp);
}

void QtApp::onRunVersion()
{
    auto command = getCommand(CommandId::Version);
    auto result = m_adapter.run(command);
    
    QString output = QString::fromStdString(result.stdout_text) +
                    QString::fromStdString(result.stderr_text) +
                    QString::number(result.exit_code) + "\n";
    
    m_outputView->setOutput(output);
}

void QtApp::onShowHelp()
{
    auto command = getCommand(CommandId::Help);
    auto result = m_adapter.run(command);
    
    QString output = QString::fromStdString(result.stdout_text) +
                    QString::fromStdString(result.stderr_text) +
                    QString::number(result.exit_code) + "\n";
    
    m_outputView->setOutput(output);
}