#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QWidget>
#include <QMap>
#include <QSet>
#include <QLineEdit>
class QVBoxLayout;
#include "../adapters/QtCliAdapter.h"
#include "../ui/UIStateStore.h"
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
    void onToggleGroup();
    void onSearchTextChanged(const QString& text);
    void onToggleFavorite();

private:
    QString getGroupName(CommandGroup group);
    void updateFavoritesSection();
    void loadFavorites();
    void saveFavorites();
    void loadGroupStates();
    void saveGroupState(CommandGroup group);
    void loadSearchText();
    void saveSearchText();
    
    QPushButton* m_versionButton;
    QPushButton* m_helpButton;
    OutputView* m_outputView;
    QtCliAdapter m_adapter;
    QMap<CommandGroup, QWidget*> m_groupContents;
    QMap<CommandGroup, QWidget*> m_groupContainers;
    QMap<CommandGroup, bool> m_groupExpanded;
    QLineEdit* m_searchInput;
    QMap<QPushButton*, QString> m_commandDescriptions;
    QMap<QPushButton*, CommandGroup> m_buttonGroups;
    QSet<nx::cli::CommandId> m_favoriteCommands;
    QWidget* m_favoritesSection;
    QVBoxLayout* m_favoritesLayout;
    UIStateStore m_uiState;
};