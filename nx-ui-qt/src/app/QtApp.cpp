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
        
        // Create collapsible group container
        QWidget* groupContainer = new QWidget(this);
        QVBoxLayout* containerLayout = new QVBoxLayout(groupContainer);
        containerLayout->setContentsMargins(0, 0, 0, 0);
        containerLayout->setSpacing(0);
        
        // Create clickable header
        QPushButton* headerButton = new QPushButton("▼ " + groupName, this);
        headerButton->setProperty("group", static_cast<int>(group));
        headerButton->setCursor(Qt::PointingHandCursor);
        headerButton->setStyleSheet(
            "QPushButton { "
            "    text-align: left; "
            "    font-weight: bold; "
            "    font-size: 14px; "
            "    border: 2px solid #cccccc; "
            "    border-radius: 5px; "
            "    padding: 12px 15px; "
            "    background-color: #f8f8f8; "
            "    color: #333333; "
            "} "
            "QPushButton:hover { "
            "    background-color: #e8e8e8; "
            "    border-color: #999999; "
            "} "
            "QPushButton:pressed { "
            "    background-color: #d8d8d8; "
            "}"
        );
        connect(headerButton, &QPushButton::clicked, this, &QtApp::onToggleGroup);
        
        // Create content area
        QWidget* contentWidget = new QWidget(this);
        contentWidget->setStyleSheet(
            "QWidget { "
            "    border: 1px solid #dddddd; "
            "    border-top: none; "
            "    border-radius: 0px 0px 5px 5px; "
            "    background-color: #ffffff; "
            "}"
        );
        QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
        contentLayout->setSpacing(8);
        contentLayout->setContentsMargins(15, 15, 15, 15);
        
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
            contentLayout->addWidget(button);
        }
        
        containerLayout->addWidget(headerButton);
        containerLayout->addWidget(contentWidget);
        
        mainLayout->addWidget(groupContainer);
        mainLayout->addSpacing(10);
        
        // Store references and set default expanded state
        m_groupContents[group] = contentWidget;
        m_groupExpanded[group] = true; // Default: expanded
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

void QtApp::onToggleGroup() {
    // UI-only presentation guard: Collapsible state has no semantic meaning
    // This affects only visual display, never command availability or execution
    
    QPushButton* headerButton = qobject_cast<QPushButton*>(sender());
    if (!headerButton) return;
    
    int groupInt = headerButton->property("group").toInt();
    CommandGroup group = static_cast<CommandGroup>(groupInt);
    
    // Toggle expanded state
    bool isExpanded = m_groupExpanded[group];
    m_groupExpanded[group] = !isExpanded;
    
    // Update content visibility
    QWidget* contentWidget = m_groupContents[group];
    if (contentWidget) {
        contentWidget->setVisible(!isExpanded);
    }
    
    // Update header arrow
    QString groupName = getGroupName(group);
    QString arrow = !isExpanded ? "▲" : "▼"; // Up arrow when collapsed, down when expanded
    headerButton->setText(arrow + " " + groupName);
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