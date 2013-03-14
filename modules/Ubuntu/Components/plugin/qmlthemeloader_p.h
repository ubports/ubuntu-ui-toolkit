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

class PropertyMap {
public:
    PropertyMap() : normalized(false){}
    bool normalized;
    PropertyHash properties;
    inline bool merge(const PropertyMap &other, bool overrides)
    {
        bool result = false;
        QHashIterator<QString, QString> i(other.properties);
        while (i.hasNext()) {
            i.next();
            if (overrides || !properties.contains(i.key())) {
                properties.insert(i.key(), i.value());
                result = true;
            }
        }
        return result;
    }
};

class QmlThemeLoader : public ThemeLoader {
    Q_INTERFACES(ThemeLoader)
public:
    QmlThemeLoader(QQmlEngine *engine);
    virtual ~QmlThemeLoader(){}
    bool loadTheme(const QUrl &path, QStringList &themeFiles, StyleCache &cache);

private:
    QStringList themeFiles;

private:

    static QString urlMacro(const QString &param, const QTextStream &stream);
    static QString readChar(QTextStream &stream, const QRegExp &bypassTokens = QRegExp("[ \t\r\n]"));
    static QString readTillToken(QTextStream &stream, const QRegExp &tokens, const QRegExp &bypassTokens = QRegExp(), bool excludeToken = true);
    static QString readDeclarationBlock(QTextStream &stream);
    static void parseDeclarationBlock(const QString &blockData, PropertyMap &properties, const QTextStream &stream);
    static void patchDeclarationValue(QString &value, const QTextStream &stream);
    void handleSelector(const Selector &path, const PropertyMap &newProperties);
    void normalizeStyles();
    bool updateRuleProperties(Selector &selector, PropertyMap &propertyMap, bool override);
    bool normalizeSelector(const Selector &selector);
    bool parseTheme(const QUrl &url);
    bool parseAtRules(QTextStream &stream);
    bool parseDeclarations(QString &data, QTextStream &stream);
    bool generateStyleQml(StyleCache &cache);
    QPair<QString, QString> selectorMapping(const Selector &selector);
    void buildStyleAndDelegate(Selector &selector, PropertyHash &properties, QString &style, QString &delegate);

    // @-rule handlers
    static bool handleImport(QmlThemeLoader *loader, QTextStream &stream);
    static bool handleQmlMapping(QmlThemeLoader *loader, QTextStream &stream);
    static bool handleQmlImport(QmlThemeLoader *loader, QTextStream &stream);

    QString imports;
    QString ruleString;
    QHash<QString, ParserCallback> rules;
    QHash<QString, QPair<QString, QString> > qmlMap;
    QHash<Selector, PropertyMap > selectorTable;
};



#endif // QMLTHEMELOADER_P_H
