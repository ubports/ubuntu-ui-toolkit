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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#ifndef THEMEENGINE_H
#define THEMEENGINE_H

#include <QObject>
#include <QHash>
#include <QVector>
#include <QUrl>
#include <QFileSystemWatcher>

class Style;
class QDeclarativeEngine;
class QDeclarativeItem;
class QDeclarativeComponent;
class StyledItem;

typedef QHash<QString, StyledItem*> InstanceHash;

class StylePathNode {
    public:
    enum Relationship {Child, Descendant, Sibling};
    StylePathNode(const QString &styleClass, const QString &styleId, Relationship relationship);
    bool operator==(const StylePathNode &other);
    QString styleClass;
    QString styleId;
    Relationship relationship;
};
typedef QList<StylePathNode> StylePath;

class StyleComponent {
    public:
    StyleComponent(StylePath stylePath, Style *style);
    StylePath stylePath;
    Style *style;
    bool operator==(const StyleComponent &other);
};
typedef QList<StyleComponent> StyleCache;

class ThemeEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool debug READ debug WRITE setDebug)
public:
    explicit ThemeEngine(QObject *parent = 0);
    ~ThemeEngine();

public: //getter/setter
    bool debug();
    void setDebug(bool debug);

public: // utility methods

    static ThemeEngine* instance();
    static QDeclarativeEngine *engine();
    static void initialize(QDeclarativeEngine *engine);
    static Style *lookupStyle(StyledItem *item, bool forceClassName = false);
    static bool registerInstanceId(StyledItem *item, const QString &newId);

    // public functions on instance
    void buildStyleCache(QObject *theme);
    QList<StylePath> parseSelector(Style *style, const StylePath &parentPath) const;
    QString stylePathToString(const StylePath &path) const;
    StylePath getStylePath(const StyledItem *obj, bool forceClassName) const;
    StyleComponent match(const StylePath &srcStylePath);

signals:
    void themeChanged();

public slots:
    void loadTheme(const QUrl &themeFile);

private slots:
    void completeThemeLoading();
    void changeTheme();

private: //members
    bool m_debug;
    QDeclarativeEngine *m_engine;
    StyleCache m_styleCache;
    QVector<int> m_maybeMatchList;
    InstanceHash m_instanceCache;
    // needed for theme loading
    QDeclarativeComponent *m_themeComponent;
    QFileSystemWatcher m_themeWatcher;
    bool m_updateTheme;
};

#endif // THEMEENGINE_H
