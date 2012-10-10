/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef THEMEENGINE_P_H
#define THEMEENGINE_P_H

#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QObject>
#include "themeengine.h"
#include "themesettings_p.h"
#include "suffixtree_p.h"

class QQuickItem;
class QQmlComponent;
class ThemeLoader;

typedef QHash<QString, StyledItem*> InstanceHash;

extern const char *appUseGlobalThemeKey;
extern const char *appThemeFileKey;
extern const char *systemThemePath;

#define SELECTOR_IGNORE_RELATIONSHIP    0x01
#define SELECTOR_IGNORE_STYLEID         0x02
#define SELECTOR_IGNORE_ALL             (SELECTOR_IGNORE_RELATIONSHIP | SELECTOR_IGNORE_STYLEID)

// Private functionality of the theme engine
class ThemeEnginePrivate
{
    Q_DECLARE_PUBLIC(ThemeEngine)
public:
    ThemeEnginePrivate(ThemeEngine *qq);
    ~ThemeEnginePrivate();

public: //members
    ThemeEngine *q_ptr;
    static ThemeEngine *themeEngine;

    QQmlEngine *m_engine;
    // suffix tree for the styles
    StyleTreeNode *m_styleTree;
    QHash<Selector, StyleRule*> m_styleCache;
    InstanceHash m_instanceCache;
    QStringList importPaths;
    QMap<QString, ThemeLoader*> themeLoaders;
    ThemeSettings themeSettings;
    // needed for theme loading
    QString errorString;
    QUrl currentTheme;

    // public functions on instance
public:
    void loadTheme(const QUrl &themeFile);
    Selector getSelector(const StyledItem *obj, bool forceClassName) const;
    StyleRule *styleRuleForPath(const Selector &path);

    // utility functions that are independent from the instance
    static void setError(const QString &error);
    static QString selectorToString(const Selector &path);
    static QList<Selector> parseSelector(const QString &selectorString, unsigned char ignore = 0);

// private slots
    void _q_updateTheme();
};

#endif // THEMEENGINE_P_H
