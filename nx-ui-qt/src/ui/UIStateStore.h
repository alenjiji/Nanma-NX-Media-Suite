#pragma once

#include <QSettings>
#include <QStringList>
#include <QVariant>

class UIStateStore {
public:
    UIStateStore();
    
    // Favorites persistence
    void saveFavorites(const QStringList& favoriteIds);
    QStringList loadFavorites() const;
    
    // Group collapse state persistence
    void saveGroupState(const QString& groupName, bool expanded);
    bool loadGroupState(const QString& groupName, bool defaultExpanded = true) const;
    
    // Search text persistence
    void saveSearchText(const QString& searchText);
    QString loadSearchText() const;
    
private:
    QSettings m_settings;
    
    static const QString FAVORITES_KEY;
    static const QString GROUP_STATE_PREFIX;
    static const QString SEARCH_TEXT_KEY;
};