#include "UIStateStore.h"

// Phase 16.9 Guard: UI state persistence is presentation-only, never affects command execution
// This class only manages UI preferences, preserving all CLI behavior and adapter logic

const QString UIStateStore::FAVORITES_KEY = "ui/favorites";
const QString UIStateStore::GROUP_STATE_PREFIX = "ui/groups/";
const QString UIStateStore::SEARCH_TEXT_KEY = "ui/search";

UIStateStore::UIStateStore() 
    : m_settings("Nanma", "NX-MediaSuite") {
    // Explicit application/organization name for proper QSettings namespacing
}

void UIStateStore::saveFavorites(const QStringList& favoriteIds) {
    m_settings.setValue(FAVORITES_KEY, favoriteIds);
}

QStringList UIStateStore::loadFavorites() const {
    return m_settings.value(FAVORITES_KEY, QStringList()).toStringList();
}

void UIStateStore::saveGroupState(const QString& groupName, bool expanded) {
    m_settings.setValue(GROUP_STATE_PREFIX + groupName, expanded);
}

bool UIStateStore::loadGroupState(const QString& groupName, bool defaultExpanded) const {
    return m_settings.value(GROUP_STATE_PREFIX + groupName, defaultExpanded).toBool();
}

void UIStateStore::saveSearchText(const QString& searchText) {
    m_settings.setValue(SEARCH_TEXT_KEY, searchText);
}

QString UIStateStore::loadSearchText() const {
    return m_settings.value(SEARCH_TEXT_KEY, QString()).toString();
}