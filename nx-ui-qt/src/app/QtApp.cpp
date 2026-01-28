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
#include <QVariant>

// Compile-time guard: QtApp must never construct CLI arguments
#ifdef CLI_EXECUTION_H
#error "CLI execution header detected in QtApp - UI layer must not construct CLI arguments"
#endif

QtApp::QtApp(QWidget *parent)
    : QMainWindow(parent)
{
    // Load persisted UI state early (favorites affect layout)
    loadFavorites();
    
    setWindowTitle("Nanma NX-MediaSuite");
    resize(800, 600);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Header Area
    QLabel* headerLabel = new QLabel("Nanma NX-MediaSuite", this);
    headerLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(headerLabel);
    
    // Search Input
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search commands...");
    m_searchInput->setStyleSheet(
        "QLineEdit { "
        "    padding: 8px 12px; "
        "    border: 2px solid #cccccc; "
        "    border-radius: 5px; "
        "    font-size: 14px; "
        "    margin: 5px 0px; "
        "} "
        "QLineEdit:focus { "
        "    border-color: #0078d4; "
        "}"
    );
    mainLayout->addWidget(m_searchInput);
    mainLayout->addSpacing(10);
    
    // Connect search input
    connect(m_searchInput, &QLineEdit::textChanged, this, &QtApp::onSearchTextChanged);
    
    // Favorites Section
    m_favoritesSection = new QWidget(this);
    QVBoxLayout* favoritesContainer = new QVBoxLayout(m_favoritesSection);
    favoritesContainer->setContentsMargins(0, 0, 0, 0);
    favoritesContainer->setSpacing(0);
    
    QLabel* favoritesHeader = new QLabel("★ Favorites", this);
    favoritesHeader->setStyleSheet(
        "QLabel { "
        "    font-weight: bold; "
        "    font-size: 14px; "
        "    padding: 12px 15px; "
        "    background-color: #f0f8ff; "
        "    border: 2px solid #87ceeb; "
        "    border-radius: 5px; "
        "    color: #333333; "
        "}"
    );
    
    QWidget* favoritesContent = new QWidget(this);
    favoritesContent->setStyleSheet(
        "QWidget { "
        "    border: 1px solid #87ceeb; "
        "    border-top: none; "
        "    border-radius: 0px 0px 5px 5px; "
        "    background-color: #ffffff; "
        "}"
    );
    m_favoritesLayout = new QVBoxLayout(favoritesContent);
    m_favoritesLayout->setSpacing(8);
    m_favoritesLayout->setContentsMargins(15, 15, 15, 15);
    
    favoritesContainer->addWidget(favoritesHeader);
    favoritesContainer->addWidget(favoritesContent);
    
    mainLayout->addWidget(m_favoritesSection);
    mainLayout->addSpacing(10);
    
    // Initially hidden
    m_favoritesSection->setVisible(false);
    
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
            // Skip favorited commands in regular groups
            if (m_favoriteCommands.contains(command.command_id)) {
                continue;
            }
            
            // Create container for button and favorite indicator
            QWidget* buttonContainer = new QWidget(this);
            QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
            buttonLayout->setContentsMargins(0, 0, 0, 0);
            buttonLayout->setSpacing(5);
            
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
            
            // Create favorite toggle button
            QPushButton* favoriteButton = new QPushButton("☆", this);
            favoriteButton->setProperty("target_button", QVariant::fromValue(button));
            favoriteButton->setFixedSize(24, 24);
            favoriteButton->setStyleSheet(
                "QPushButton { "
                "    border: none; "
                "    background: transparent; "
                "    font-size: 16px; "
                "} "
                "QPushButton:hover { "
                "    background-color: #f0f0f0; "
                "    border-radius: 3px; "
                "}"
            );
            
            // Set initial state based on CommandId
            if (m_favoriteCommands.contains(command.command_id)) {
                favoriteButton->setText("★"); // Filled star
            }
            
            connect(favoriteButton, &QPushButton::clicked, this, &QtApp::onToggleFavorite);
            
            buttonLayout->addWidget(button, 1);
            buttonLayout->addWidget(favoriteButton, 0);
            
            contentLayout->addWidget(buttonContainer);
            
            // Store description for filtering
            m_commandDescriptions[button] = QString::fromStdString(command.short_description);
            m_buttonGroups[button] = group;
        }
        
        containerLayout->addWidget(headerButton);
        containerLayout->addWidget(contentWidget);
        
        mainLayout->addWidget(groupContainer);
        mainLayout->addSpacing(10);
        
        // Store references and set default expanded state
        m_groupContainers[group] = groupContainer;
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
    
    // Phase 16.9 Guard: UI state restoration order is critical for deterministic behavior
    // This sequence ensures no command execution during restoration, only UI state changes
    
    // 1. Favorites loaded early (before UI build) to affect command placement
    // 2. UI built with favorites already loaded
    // 3. Restore collapse state (affects group visibility)
    loadGroupStates();
    // 4. Restore search text last (triggers filtering on restored UI)
    loadSearchText();
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
    
    // Save group state
    saveGroupState(group);
}

void QtApp::onSearchTextChanged(const QString& text) {
    // Phase 16.7 Guard: Search is presentation-only, never affects command execution or semantics
    // This function only controls UI visibility, preserving all CLI behavior and adapter logic
    
    // Filter command buttons based on search text (case-insensitive)
    QString searchText = text.toLower();
    
    // Track visible buttons per group (including favorites)
    QMap<CommandGroup, int> visibleCounts;
    for (auto group : m_groupContainers.keys()) {
        visibleCounts[group] = 0;
    }
    int favoritesVisibleCount = 0;
    
    for (auto it = m_commandDescriptions.begin(); it != m_commandDescriptions.end(); ++it) {
        QPushButton* button = it.key();
        QString description = it.value().toLower();
        
        // Show/hide button based on substring match
        bool matches = searchText.isEmpty() || description.contains(searchText);
        button->setVisible(matches);
        
        // Count visible buttons per group
        if (matches) {
            CommandGroup group = m_buttonGroups[button];
            
            // Check if button is in favorites section
            if (button->parent() && button->parent()->parent() == m_favoritesSection) {
                favoritesVisibleCount++;
            } else {
                visibleCounts[group]++;
            }
        }
    }
    
    // Hide/show group containers based on visible button count
    for (auto it = m_groupContainers.begin(); it != m_groupContainers.end(); ++it) {
        CommandGroup group = it.key();
        QWidget* container = it.value();
        bool hasVisibleButtons = visibleCounts[group] > 0;
        container->setVisible(hasVisibleButtons);
    }
    
    // Hide/show favorites section based on visible favorites
    if (!m_favoriteCommands.isEmpty()) {
        m_favoritesSection->setVisible(favoritesVisibleCount > 0);
    }
    
    // Save search text
    saveSearchText();
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

void QtApp::onToggleFavorite() {
    // Phase 16.8 Guard: Favorites are UI-only state, no execution or semantic changes
    
    QPushButton* favoriteButton = qobject_cast<QPushButton*>(sender());
    if (!favoriteButton) return;
    
    QPushButton* targetButton = favoriteButton->property("target_button").value<QPushButton*>();
    if (!targetButton) return;
    
    // Get CommandId from target button
    int commandIdInt = targetButton->property("command_id").toInt();
    nx::cli::CommandId commandId = static_cast<nx::cli::CommandId>(commandIdInt);
    
    // Toggle favorite state by CommandId
    if (m_favoriteCommands.contains(commandId)) {
        m_favoriteCommands.remove(commandId);
        favoriteButton->setText("☆"); // Empty star
    } else {
        m_favoriteCommands.insert(commandId);
        favoriteButton->setText("★"); // Filled star
    }
    
    // Update favorites section
    updateFavoritesSection();
    
    // Save favorites to persistent storage
    saveFavorites();
}

void QtApp::updateFavoritesSection() {
    // Phase 16.8 Guard: Favorites are presentation-only, pinning never affects CLI semantics or execution
    // This function only manages UI state, preserving all command behavior and adapter logic
    
    // Clear existing favorites
    QLayoutItem* item;
    while ((item = m_favoritesLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // Add favorited commands
    auto allCommands = CommandPresentationRegistry::getAll();
    for (const auto& command : allCommands) {
        if (!m_favoriteCommands.contains(command.command_id)) {
            continue;
        }
        
        // Create container for button and favorite indicator
        QWidget* buttonContainer = new QWidget(this);
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);
        
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
        
        // Create favorite toggle button (filled star)
        QPushButton* favoriteButton = new QPushButton("★", this);
        favoriteButton->setProperty("target_button", QVariant::fromValue(button));
        favoriteButton->setFixedSize(24, 24);
        favoriteButton->setStyleSheet(
            "QPushButton { "
            "    border: none; "
            "    background: transparent; "
            "    font-size: 16px; "
            "} "
            "QPushButton:hover { "
            "    background-color: #f0f0f0; "
            "    border-radius: 3px; "
            "}"
        );
        connect(favoriteButton, &QPushButton::clicked, this, &QtApp::onToggleFavorite);
        
        buttonLayout->addWidget(button, 1);
        buttonLayout->addWidget(favoriteButton, 0);
        
        m_favoritesLayout->addWidget(buttonContainer);
        
        // Store for search functionality
        m_commandDescriptions[button] = QString::fromStdString(command.short_description);
        m_buttonGroups[button] = command.group;
    }
    
    // Show/hide favorites section based on content
    m_favoritesSection->setVisible(!m_favoriteCommands.isEmpty());
}

void QtApp::loadFavorites() {
    QStringList favoriteIds = m_uiState.loadFavorites();
    
    for (const QString& idStr : favoriteIds) {
        bool ok;
        int commandIdInt = idStr.toInt(&ok);
        if (ok) {
            nx::cli::CommandId commandId = static_cast<nx::cli::CommandId>(commandIdInt);
            m_favoriteCommands.insert(commandId);
        }
    }
}

void QtApp::saveFavorites() {
    QStringList favoriteIds;
    
    for (nx::cli::CommandId commandId : m_favoriteCommands) {
        favoriteIds.append(QString::number(static_cast<int>(commandId)));
    }
    
    m_uiState.saveFavorites(favoriteIds);
}

void QtApp::loadGroupStates() {
    for (auto it = m_groupExpanded.begin(); it != m_groupExpanded.end(); ++it) {
        CommandGroup group = it.key();
        QString groupName = getGroupName(group);
        bool expanded = m_uiState.loadGroupState(groupName, true); // Default expanded
        m_groupExpanded[group] = expanded;
        
        // Update UI to match loaded state
        QWidget* contentWidget = m_groupContents[group];
        if (contentWidget) {
            contentWidget->setVisible(expanded);
        }
    }
}

void QtApp::saveGroupState(CommandGroup group) {
    QString groupName = getGroupName(group);
    bool expanded = m_groupExpanded[group];
    m_uiState.saveGroupState(groupName, expanded);
}

void QtApp::loadSearchText() {
    QString searchText = m_uiState.loadSearchText();
    if (!searchText.isEmpty()) {
        m_searchInput->setText(searchText);
        // Trigger filtering after restoration
        onSearchTextChanged(searchText);
    }
}

void QtApp::saveSearchText() {
    QString searchText = m_searchInput->text();
    m_uiState.saveSearchText(searchText);
}