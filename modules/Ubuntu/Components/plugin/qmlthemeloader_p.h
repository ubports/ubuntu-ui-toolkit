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

#ifndef QMLTHEMELOADER_P_H
#define QMLTHEMELOADER_P_H

#include "themeengine_p.h"
#include "themeloader_p.h"
#include <QtCore/QTextStream>

// CSS-like theme loader
class QmlThemeLoader;
typedef bool (*ParserCallback)(QmlThemeLoader *loader, QTextStream &stream);

typedef QHash<QString, QString> PropertyHash;

class QmlThemeLoader : public ThemeLoader {
    Q_INTERFACES(ThemeLoader)
public:
    QmlThemeLoader(QQmlEngine *engine);
    virtual ~QmlThemeLoader(){}
    StyleTreeNode *loadTheme(const QUrl &path);

private:
    StyleTreeNode *styleTree;

private:

    static QString urlMacro(const QString &param, const QTextStream &stream);
    static QString readChar(QTextStream &stream, const QRegExp &bypassTokens = QRegExp("[ \t\r\n]"));
    static QString readTillToken(QTextStream &stream, const QRegExp &tokens, const QRegExp &bypassTokens = QRegExp(), bool excludeToken = true);
    bool handleSelector(const Selector &path, const QString &declarator, QTextStream &stream);
    void normalizeStyles();
    bool updateRuleProperties(Selector &selector, QHash<QString, QString> &propertyMap);
    bool parseTheme(const QUrl &url);
    bool parseAtRules(QTextStream &stream);
    bool parseDeclarations(QString &data, QTextStream &stream);
    bool generateStyleQml();
    void buildStyleAndDelegate(Selector &selector, PropertyHash &properties, QString &style, QString &delegate);

    // @-rule handlers
    static bool handleImport(QmlThemeLoader *loader, QTextStream &stream);
    static bool handleQmlMapping(QmlThemeLoader *loader, QTextStream &stream);
    static bool handleQmlImport(QmlThemeLoader *loader, QTextStream &stream);

    QString imports;
    QString ruleString;
    QHash<QString, ParserCallback> rules;
    QHash<QString, QPair<QString, QString> > qmlMap;
    QHash<Selector, PropertyHash > selectorTable;
};



#endif // QMLTHEMELOADER_P_H
