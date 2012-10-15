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

#ifndef QTHMTHEMELOADER_P_H
#define QTHMTHEMELOADER_P_H

#include "themeengine_p.h"
#include "themeloader_p.h"
#include <QtCore/QTextStream>

// CSS-like theme loader
class QthmThemeLoader;
typedef bool (*QthmThemeParserCallback)(QthmThemeLoader *loader, QTextStream &stream);

class QthmThemeLoader : public ThemeLoader {
    Q_INTERFACES(ThemeLoader)
public:
    QthmThemeLoader(QQmlEngine *engine);
    virtual ~QthmThemeLoader(){}
    StyleTreeNode *loadTheme(const QUrl &path);

private:
    StyleTreeNode *styleTree;

private:

    static QString readChar(QTextStream &stream, const QRegExp &bypassTokens = QRegExp("[ \t\r\n]"));
    static QString readTillToken(QTextStream &stream, const QRegExp &tokens, const QRegExp &bypassTokens = QRegExp(), bool excludeToken = true);
    bool handleSelector(const Selector &path, const QString &declarator);
    void normalizeStyles();
    bool parseTheme(const QUrl &url);
    bool generateStyleQml();
    bool buildStyleTree(const QUrl &url);

    // @-rule handlers
    static bool handleImport(QthmThemeLoader *loader, QTextStream &stream);
    static bool handleQmlMapping(QthmThemeLoader *loader, QTextStream &stream);
    static bool handleQmlImport(QthmThemeLoader *loader, QTextStream &stream);

    QString imports;
    QString ruleString;
    QHash<QString, QthmThemeParserCallback> rules;
    QHash<QString, QPair<QString, QString> > qmlMap;
    QHash<Selector, QHash<QString, QString> > selectorTable;
};



#endif // QTHMTHEMELOADER_P_H
