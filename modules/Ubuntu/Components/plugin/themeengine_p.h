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
#include <QtCore/QCoreApplication>
#include "themeengine.h"
#include "themesettings_p.h"
#include "suffixtree_p.h"

#include <QDebug>

class QQuickItem;
class QQmlComponent;
class ThemeLoader;

typedef QHash<QString, QQuickItem*> InstanceHash;

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
    QHash<Selector, Rule*> m_styleCache;
    InstanceHash m_instanceCache;
    QStringList importPaths;
    QMap<QString, ThemeLoader*> themeLoaders;
    ThemeSettings themeSettings;
    // needed for theme loading
    QString errorString;
    QUrl currentTheme;
    bool firstThemeLoaded;

    // public functions on instance
public:
    void loadTheme(const QUrl &themeFile);
    Selector getSelector(QQuickItem *obj, bool forceClassName) const;
    Rule *styleRuleForPath(const Selector &path);

    // utility functions that are independent from the instance
    static void setError(const QString &error);
    static ItemStyleAttached *attachedStyle(QObject *obj);
    static QString selectorToString(const Selector &path);
    static QList<Selector> parseSelector(const QString &selectorString, SelectorNode::NodeSensitivity sensitivity = SelectorNode::Normal);

// private slots
    void _q_updateTheme();
};

inline QString systemFolder()
{
    QString env = QLatin1String(getenv("UITK_THEME_PATH"));
    env = env.isEmpty() ? QString("/usr/share/themes") : env + "/themes";
    return env;
}

#endif // THEMEENGINE_P_H
