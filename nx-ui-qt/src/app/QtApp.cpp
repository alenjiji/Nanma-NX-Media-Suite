#include "QtApp.h"
#include "adapters/QtCliAdapter.h"
#include "ui/OutputView.h"
#include "commands/CommandDescriptor.h"
#include "commands/CommandPresentationRegistry.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QWidget>
#include <QPushButton>

// Compile-time guard: QtApp must never construct CLI arguments
#ifdef CLI_EXECUTION_H
#error "CLI execution header detected in QtApp - UI layer must not construct CLI arguments"
#endif

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
    
    // Command Groups
    auto groupedCommands = CommandPresentationRegistry::getGroupedCommands();
    for (const auto& [group, commands] : groupedCommands) {
        QString groupName = getGroupName(group);
        QGroupBox* groupBox = new QGroupBox(groupName, this);
        groupBox->setStyleSheet(
            "QGroupBox { "
            "    font-weight: bold; "
            "    border: 2px solid #cccccc; "
            "    border-radius: 5px; "
            "    margin-top: 10px; "
            "    padding-top: 10px; "
            "} "
            "QGroupBox::title { "
            "    subcontrol-origin: margin; "
            "    left: 10px; "
            "    padding: 0 5px 0 5px; "
            "}"
        );
        
        QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
        groupLayout->setSpacing(8);
        groupLayout->setContentsMargins(15, 15, 15, 15);
        
        for (const auto& command : commands) {
            QPushButton* button = new QPushButton(QString::fromStdString(command.short_description), this);
            button->setProperty("command_id", static_cast<int>(command.command_id));
            button->setStyleSheet(
                "QPushButton { "
                "    text-align: left; "
                "    padding: 8px 12px; "
                "    border: 1px solid #999999; "
                "    border-radius: 3px; "
                "    background-color: #f5f5f5; "
                "} "
                "QPushButton:hover { "
                "    background-color: #e5e5e5; "
                "    border-color: #666666; "
                "} "
                "QPushButton:pressed { "
                "    background-color: #d5d5d5; "
                "}"
            );
            connect(button, &QPushButton::clicked, this, &QtApp::onRunCommand);
            groupLayout->addWidget(button);
        }
        
        mainLayout->addWidget(groupBox);
        mainLayout->addSpacing(10);
    }
    
    // Legacy Actions Panel (for Version/Help)
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

QString QtApp::getGroupName(CommandGroup group) {
    switch (group) {
        case CommandGroup::CoreOperations: return "Core Operations";
        case CommandGroup::AudioProcessing: return "Audio Processing";
        case CommandGroup::VideoProcessing: return "Video Processing";
        case CommandGroup::MetadataAndContainer: return "Metadata & Container";
        case CommandGroup::BatchAndAutomation: return "Batch & Automation";
        case CommandGroup::InspectionAndValidation: return "Inspection & Validation";
        case CommandGroup::SystemAndInfrastructure: return "System & Infrastructure";
        case CommandGroup::HelpAndInformation: return "Help & Information";
    }
    return "Unknown";
}

void QtApp::onRunCommand() {
    // Architecture guard: UI must only dispatch CommandId, never construct CLI args
    static_assert(sizeof(nx::cli::CommandId) > 0, "CommandId must be opaque to UI layer");
    
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    int commandIdInt = button->property("command_id").toInt();
    nx::cli::CommandId commandId = static_cast<nx::cli::CommandId>(commandIdInt);
    
    // All CLI execution must route through adapter - no direct CLI calls allowed
    auto result = m_adapter.runCommand(commandId);
    
    QString output = QString::fromStdString(result.stdout_text) +
                    QString::fromStdString(result.stderr_text) +
                    "Exit code: " + QString::number(result.exit_code) + "\n";
    
    m_outputView->setOutput(output);
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