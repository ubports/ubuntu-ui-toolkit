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

#include <QFileSystemWatcher>
#include <QHash>
#include <QTextStream>
#include <QRegExp>
#include "themeengine.h"

class QDeclarativeItem;
class QDeclarativeComponent;

typedef QHash<QString, StyledItem*> InstanceHash;

#define SELECTOR_IGNORE_RELATIONSHIP    0x01
#define SELECTOR_IGNORE_STYLEID         0x02
#define SELECTOR_IGNORE_ALL             (SELECTOR_IGNORE_RELATIONSHIP | SELECTOR_IGNORE_STYLEID)

// node of a selector
class SelectorNode {
    public:
    enum Relationship {Child, Descendant, Sibling};
    SelectorNode();
    SelectorNode(const QString &styleClass, const QString &styleId, Relationship relationship, unsigned char ignore = 0);
    QString toString() const;
    bool operator==(const SelectorNode &other);
    QString styleClass;
    QString styleId;
    Relationship relationship;
    unsigned char ignore;
};

// selector type
typedef QList<SelectorNode> Selector;
uint qHash(const Selector &key);

// style rule tree
class StyleTreeNode {
public:
    StyleTreeNode(StyleTreeNode *parent = 0);
    StyleTreeNode(StyleTreeNode *parent, const SelectorNode &node, StyleRule *styleRule);
    ~StyleTreeNode();
    void clear();
    void addStyleRule(const Selector &path, StyleRule *styleRule);
    StyleRule *lookupStyleRule(const Selector &path, bool strict = false);
    StyleRule *testNode(SelectorNode &nextNode, const Selector &sparePath, bool &strict);
    void listTree(const QString &prefix = QString());

public:
    StyleTreeNode *parent;
    SelectorNode styleNode;
    StyleRule *styleRule;
    // the key is the next CSS node's "relationship styleClass#styleId" combination
    QHash<QString, StyleTreeNode*> children;
};

// QML theme loader
class QmlTheme {
public:
    QmlTheme() : themeComponent(0), styleTree(0){}

    bool loadQmlTheme(const QUrl &path, QDeclarativeEngine *engine, StyleTreeNode *styleTree);
    bool finalizeThemeLoading();
private:
    QDeclarativeComponent *themeComponent;
    StyleTreeNode *styleTree;
};

// CSS-like theme loader
class CssTheme;
typedef bool (*CssThemeParserCallback)(CssTheme *css, QTextStream &stream);
class CssTheme {
public:
    CssTheme();
    ~CssTheme(){}

    bool loadCssTheme(const QUrl &url, QDeclarativeEngine *engine, StyleTreeNode *styleTree);
private:

    QString readChar(QTextStream &stream, const QRegExp &bypassTokens = QRegExp("[ \t\r\n]"));
    QString readTillToken(QTextStream &stream, const QRegExp &tokens, const QRegExp &bypassTokens = QRegExp(), bool excludeToken = true);
    static bool handleImport(CssTheme *css, QTextStream &stream);
    static bool handleQmlMapping(CssTheme *css, QTextStream &stream);
    static bool handleQmlImport(CssTheme *css, QTextStream &stream);
    bool handleSelector(const Selector &path, const QString &declarator);
    void normalizeStyles();
    bool parseCssTheme(const QUrl &url);
    bool buildStyleTree();

    QDeclarativeEngine *engine;
    StyleTreeNode *styleTree;
    QString imports;
    QHash<QString, CssThemeParserCallback> rules;
    QHash<QString, QPair<QString, QString> > qmlMap;
    QHash<Selector, QHash<QString, QString> > selectorTable;
};

// Private functionality of the theme engine
class ThemeEnginePrivate
{
    Q_DECLARE_PUBLIC(ThemeEngine)
public:
    ThemeEnginePrivate(ThemeEngine *qq);
    ~ThemeEnginePrivate();

public: //members
    ThemeEngine *q_ptr;
    bool initialized;
    QFileSystemWatcher m_themeWatcher;
    QDeclarativeEngine *m_engine;
    // suffix tree for the styles
    StyleTreeNode *m_styleTree;
    QHash<Selector, StyleRule*> m_styleCache;
    InstanceHash m_instanceCache;
    QmlTheme m_qmlThemeLoader;
    CssTheme m_cssThemeLoader;
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
    void _q_continueThemeLoading();
    void _q_updateTheme();
};

#endif // THEMEENGINE_P_H
