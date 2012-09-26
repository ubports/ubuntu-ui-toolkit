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

class StyleRule;
class QDeclarativeEngine;
class QDeclarativeItem;
class QDeclarativeComponent;
class StyledItem;

typedef QHash<QString, StyledItem*> InstanceHash;

class StylePathNode {
    public:
    enum Relationship {Child, Descendant, Sibling};
    StylePathNode();
    StylePathNode(const QString &styleClass, const QString &styleId, Relationship relationship);
    QString toString() const;
    bool operator==(const StylePathNode &other);
    QString styleClass;
    QString styleId;
    Relationship relationship;
};
typedef QList<StylePathNode> StylePath;

class StyleComponent {
    public:
    StyleComponent(StylePath stylePath, StyleRule *style);
    StylePath stylePath;
    StyleRule *style;
    bool operator==(const StyleComponent &other);
};
typedef QList<StyleComponent> StyleCache;


class StyleTreeNode {
public:
    StyleTreeNode(StyleTreeNode *parent = 0);
    StyleTreeNode(StyleTreeNode *parent, const StylePathNode &node, StyleRule *styleRule);
    ~StyleTreeNode();
    void addStyleRule(const StylePath &path, StyleRule *styleRule);
    StyleRule *lookupStyleRule(const StylePath &path, bool strict = false);
    StyleRule *testNode(StylePathNode &nextNode, const StylePath &sparePath, bool &strict);
    void listTree(const QString &prefix = QString());

public:
    StyleTreeNode *parent;
    StylePathNode styleNode;
    StyleRule *styleRule;
    // the key is the next CSS node's styleClass#styleId combination, without the relationship
    // therefore we need a multi-hash that accepts multiple values for the same key
    QHash<QString, StyleTreeNode*> children;
};

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
    static StyleRule *lookupStyleRule(StyledItem *item, bool forceClassName = false);
    static bool registerInstanceId(StyledItem *item, const QString &newId);

    // public functions on instance
    void buildStyleCache(QObject *theme);
    QList<StylePath> parseSelector(StyleRule *styleRule, const StylePath &parentPath) const;
    QString stylePathToString(const StylePath &path) const;
    StylePath getStylePath(const StyledItem *obj, bool forceClassName) const;
    StyleRule *styleRuleForPath(const StylePath &path);
    StyleComponent match(const StylePath &srcStylePath);

signals:
    void themeChanged();

public slots:
    void loadTheme(const QUrl &themeFile);

private slots:
    void completeThemeLoading();
    void changeTheme();

private: //members
    QDeclarativeEngine *m_engine;
    StyleCache m_styleCache;
    // suffix tree for the styles
    StyleTreeNode *m_styleTree;
    QVector<int> m_maybeMatchList;
    InstanceHash m_instanceCache;
    // needed for theme loading
    QDeclarativeComponent *m_themeComponent;
    QFileSystemWatcher m_themeWatcher;
    bool m_updateTheme;
};

#endif // THEMEENGINE_H
