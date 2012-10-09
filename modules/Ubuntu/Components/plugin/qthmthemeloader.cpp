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

#include "themeengine.h"
#include "themeengine_p.h"
#include "qthmthemeloader_p.h"
#include "style.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

#include <QDebug>

/*!
  \page QTHM file parser
  \internal
  CSS-like theme file parser (QTHM parser)

  The parsing steps are:
  1. load file and build up selectorTable
  2. normalize selectorTable by updating each selector with the non-overridden
     properties from the base selector
  3. build ThemeEngine's styleTree by creating Rule elements using the styles,
     mappings and imports specified.

  TODOs:
  - optimization
  - merge steps 1 and 2
  - export into QML theme file for comparison in between CSS-like and QML theme
    loading
*/

const char *styleRuleComponent =  \
        "import QtQuick 2.0\n"
        "%1\n"
        "%2\n";

/*!
  \internal
  Resets the ignore flag for the selector nodes so the selector is interpreted
  as declared in CSS. Ignore flags are set by the parser to ease searching in
  the selector hash for the base selectors (subsets of the current selector).
*/
void normalizeSelector(Selector &selector)
{
    for (int i = 0; i < selector.count(); i++)
        selector[i].ignore = 0;
}

/*!
  \internal
    Returns a subset from the given selector and configures it to ignore relation
    and instanceId.
  */
Selector selectorSubset(const Selector &path, int elements)
{
    Selector result;
    while (elements > 0) {
        result << path[path.length() - elements];
        result.last().ignore = SELECTOR_IGNORE_ALL;
        elements--;
    }
    return result;
}

/*!
  \internal
  Reads the stream character by character skipping characters specified in \a bypassTokens
  parameter and comments. Returns a string which in the most cases contains a single character
  except when a '/' chracter was followed by a valid one, when returns two chars.
  */
QString QthmThemeLoader::readChar(QTextStream &stream, const QRegExp &bypassTokens)
{
    QString data;
    while (!stream.atEnd()) {

        data += stream.read(1);

        // skip comments by default; as we read character by character, it's easier to
        // implement by checking the comment limitators than using QRegExp, which would
        // ease our life if we'd work with strings.
        if (data[0] == '/') {
            if (data == "/*") {
                // read till we get the comment's end mark
                QChar charData;
                // consume comment
                while ((charData = stream.read(1)[0]) != '/') {
                    while (charData != '*')
                        charData = stream.read(1)[0];
                }
                data.clear();
                continue;
            } else if (data.length() <= 1)
                // we don't know yet the next character, so stay in
                continue;
            else
                // we have the next one and that's not a comment marker, so return it
                return data;
        }

        // check if we have something to skip
        if (!data.contains(bypassTokens))
            return data;
        else
            data.clear();
    }
    return QString();
}

/*!
  \internal
  Reads the stream content till one of the \a tokens is reached. The characters
  specified in \a bypassTokens will be skipped. \a excludeToken specifies wherher
  the limiting token reached should be included in the data read or not (default
  is not to include).
  */
QString QthmThemeLoader::readTillToken(QTextStream &stream, const QRegExp &tokens, const QRegExp &bypassTokens, bool excludeToken)
{
    QString ret;
    while (!stream.atEnd() && !ret.contains(tokens)) {
        ret += readChar(stream, bypassTokens);
    }
    if (excludeToken)
        // remove the last character as that is the token
        ret.remove(ret.length() - 1, 1);
    return ret;
}

/*!
  \internal
  Parses the declarator of each selector. Resolves the "inheritance" between atomic
  selector items (the last items in a CSS selector component).
  */
bool QthmThemeLoader::handleSelector(const Selector &selector, const QString &declarator)
{
    QStringList propertyList = declarator.split(';');

    if (propertyList.isEmpty()) {
        ThemeEnginePrivate::setError(QString("Selector %1 has empty declarator!").
                                     arg(ThemeEnginePrivate::selectorToString(selector)));
        return false;
    }

    // the properties from base classes that are not overridden will be copied
    // into the other selectors before we generate the QML components, in
    // normalizeStyles; yet we simply store them in the table
    QHash<QString, QString> properties;
    if (selectorTable.contains(selector))
        // update the current hash
        properties = selectorTable.value(selector);

    Q_FOREACH (const QString &property, propertyList) {
        if (property.isEmpty())
            continue;

        // properties might have other columns, so we cannot do separation simply
        // by splitting the string using ':' as separator, we need to do separation
        // based on the first ':' reached.
        int columnIndex = property.indexOf(':');
        QString prop = property.left(columnIndex);
        QString value = property.right(property.length() - columnIndex - 1);
        properties.insert(prop, value);
    }
    selectorTable.insert(selector, properties);

    return true;
}

/*!
  \internal
  Normalizes the style table - copies the properties that were not overridden from the base
  styles into the derivates.
  */
void QthmThemeLoader::normalizeStyles()
{
    QHashIterator<Selector, QHash<QString, QString> > i(selectorTable);
    while (i.hasNext()) {
        i.next();
        Selector selector = i.key();
        QHash<QString, QString> propertyMap = i.value();
        bool propertyMapUpdated = false;

        int count = selector.count();

        while (count-- > 1) {
            Selector subset = selectorSubset(selector, count);

            // check if we have a style that is a subset of the curent one and if yes
            // copy the base propertyes that are not overloaded by the current one
            if (selectorTable.contains(subset)) {
                // get the properties and copy the base ones into the current selector
                QHashIterator<QString, QString> baseProperty(selectorTable.value(subset));
                while (baseProperty.hasNext()) {
                    baseProperty.next();
                    if (!propertyMap.contains(baseProperty.key())) {
                        propertyMap.insert(baseProperty.key(), baseProperty.value());
                        propertyMapUpdated = true;
                    }
                }
            }
        }
        if (propertyMapUpdated) {
            selectorTable.insert(selector, propertyMap);
            // restart?
            //i.toFront();
        }
    }
}

/*!
  \internal
  Parses a QTHM theme. Reads the stream char-by-char to avoid seeking in the stream.
  Seeking would be needed when reading entire line and parsing tags out of it.
*/
bool QthmThemeLoader::parseTheme(const QUrl &url)
{
    bool ret = true;
    // open the file
    QFile file(url.path());
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);

        QString data;

        // read stream character by character so we don't need to seek
        while (!stream.atEnd()) {

            data += readChar(stream);

            if (data.isEmpty())
                break;

            if (data[0] == '@') {
                // rule!! read till the first token
                data = readTillToken(stream, QRegExp("[({]"), QRegExp("[\t]")).simplified();
                //lookup for the rule handler and continue parsing with it
                ret = rules.contains(data);
                if (ret)
                    ret = rules.value(data)(this, stream);
                else
                    ThemeEnginePrivate::setError(QString("Unhandled rule: %1").arg(data));
                if (!ret)
                    return ret;

                // reset data and continue parsing
                data.clear();
                continue;
            }

            // seems we have a theme rule definition, so read till we hit a '{' token
            data += readTillToken(stream, QRegExp("[{]"), QRegExp("[\t\r\n]")).simplified();
            if (data.isEmpty())
                continue;

            QList<Selector> selectors = ThemeEnginePrivate::parseSelector(data, SELECTOR_IGNORE_RELATIONSHIP);
            if (selectors.isEmpty()) {
                ThemeEnginePrivate::setError(QString("Syntax error!\n%1").arg(data));
                return false;
            } else {
                // load declarator and apply on each selector
                data = readTillToken(stream, QRegExp("[}]"), QRegExp("[ \t\r\n]"));
                Q_FOREACH (const Selector &selector, selectors) {
                    ret = handleSelector(selector, data);
                    if (!ret) {
                        ThemeEnginePrivate::setError(
                                    QString("Error parsing declarator for selector %1").
                                    arg(ThemeEnginePrivate::selectorToString(selector)));
                        return ret;
                    }
                }
                // clear data and continue parsing
                data.clear();
            }
        }
    } else {
        ret = false;
        ThemeEnginePrivate::setError(QString("%1: %2")
                                     .arg(file.errorString())
                                     .arg(file.fileName()));
    }
    return ret;
}

bool QthmThemeLoader::buildStyleTree()
{
    Selector selector;
    QString style;
    QString delegate;
    QString qmap;

    // go through the selector map and build the styles to each
    QHashIterator<Selector, QHash<QString, QString> > i(selectorTable);
    while (i.hasNext()) {
        i.next();
        selector = i.key();
        QHashIterator<QString, QString> properties(i.value());
        QPair<QString, QString> qmlTypes;

        qmap = '.' + selector.last().styleClass;

        if (qmlMap.contains(qmap))
            qmlTypes = qmlMap.value(qmap);

        // get the type for style and delegate
        if (i.value().count() > 0) {
            QString propertyPrefix;

            if (!qmlTypes.first.isEmpty()) {
                // we have the mapping!!
                style = qmlTypes.first;
                style = style + " {\n";
            } else {
                style = "QtObject {\n";
                propertyPrefix = "property variant";
            }

            // add properties
            while (properties.hasNext()) {
                properties.next();
                style += QString("   %1 %2: %3\n").arg(propertyPrefix).arg(properties.key()).arg(properties.value());
            }
            // append the closing brace
            style += '}';
        }

        // delegate
        if (!qmlTypes.second.isEmpty())
            delegate += QString("%1{}").arg(qmlTypes.second);

        // normalize selector so we build the Rule with the proper one
        normalizeSelector(selector);

        // creating components from internal QML source is synchronous, unless
        // one of the imported elements require threaded loading. Therefore we use
        // StyleRule to create style and delegate components so StyleRule can handle
        // asynchronous completion of those.

        if (!style.isEmpty())
            style = QString(styleRuleComponent).arg(imports).arg(style);
        if (!delegate.isEmpty())
            delegate = QString(styleRuleComponent).arg(imports).arg(delegate);
        StyleRule *rule = new StyleRule(m_engine,
                                        ThemeEnginePrivate::selectorToString(selector),
                                        style,
                                        delegate);
        // the error is reported in case the creation is synchronous, so capture it here
        if (!ThemeEngine::instance()->error().isEmpty()) {
            delete rule;
            return false;
        } else {
            // we either have a success or async component creation
            styleTree->addStyleRule(selector, rule);
            style.clear();
            delegate.clear();
        }
    }

    return true;
}


/*============================================================================*/

/*!
  \internal
  Extracts the theme specified to be imported, and loads it.
  */
bool QthmThemeLoader::handleImport(QthmThemeLoader *loader, QTextStream &stream)
{
    QString themeFile = QthmThemeLoader::readTillToken(stream, QRegExp("[;]"), QRegExp("[ )\t\r\n\"]"));
    // check if the imported theme file is an absolute path or not;
    // if not, build the path relative to the current parsed file
    // Note: resource stored theme files must use absolute paths, or should have
    // qrc: scheme specified
    if (!themeFile.startsWith('/')) {
        QFile *file = qobject_cast<QFile*>(stream.device());
        QFileInfo fi(*file);
        themeFile.prepend('/').prepend(fi.dir().absolutePath());
    }
    if (themeFile.startsWith("qrc:"))
        return loader->parseTheme(themeFile);
    else
        return loader->parseTheme(QUrl::fromLocalFile(themeFile));
}

/*!
  \internal
  Parses qml-mapping rule needed when defing the QML Rule pattern. The qml-mapping rule
  syntax is as follows:
  qml-mapping(css-tag, style-qml-type, delegate-qml-type)
  where
     - css-tag is the tag used in CSS theme
     - style-qml-type is the name of the QML document defining the style properties
     - delegate-qml-type is the name of the QML document defining the widget delegate.
  If no style is given, theme engine will use QtObject for style and will declare all
  properties typed as variant.

  Example:
  css.

  @qml-mapping(button, ButonStyle, ButtonDelegate)

  .button {
     color: "blue"
  }

  .frame {
     border: 2
  }

  will be translated as

  Rule {
    selector: ".button"
    style: ButtonStyle {
        color: "blue"
    }
    delegate: ButtonDelegate{}
  }

  Rule {
    selector: ".frame"
    style: QtObject {
        property variant border: 2
    }
  }
  */
bool QthmThemeLoader::handleQmlMapping(QthmThemeLoader *loader, QTextStream &stream)
{
    QString params = QthmThemeLoader::readTillToken(stream, QRegExp("[;]"), QRegExp("[ )\t\r\n\"]"));
    QStringList mapping = params.split(',');

    // we should have 3 elements in the list! if we don't we have an error!
    bool ret = (mapping.count() == 3);
    if (!ret)
        ThemeEnginePrivate::setError(QString("Mapping has %1 parameter(s), should have 3!").
                                     arg(mapping.count()));
    else
        loader->qmlMap.insert(mapping[0], qMakePair(mapping[1], mapping[2]));

    return ret;
}

/*!
  \internal
  Callback handling qml-import tags. Adds the import sentence to the import list
  that will be added to the template creating the style rule, and the import path
  to engine. The import path may be relative or absolute, and can contain "app"
  and "sys" keywords, which result in application's current folder and global
  theme's folder.
  */
bool QthmThemeLoader::handleQmlImport(QthmThemeLoader *loader, QTextStream &stream)
{
    QString param = QthmThemeLoader::readTillToken(stream, QRegExp("[;]"), QRegExp("[)\t\r\n]")).simplified();

    QStringList import = param.split(',');
    QString importPath = (import.count() <= 1) ? QString() : import[0];
    QString importUrl = (import.count() < 2) ? import[0] : import[1];

    // check whether we have the import set
    if (!loader->imports.contains(importUrl)) {
        loader->imports += QString("import %1\n").arg(importUrl.simplified());

        if (!importPath.isEmpty()) {
            if (importPath.startsWith("app:")) {
                importPath.remove("app:");
                if (!importPath.startsWith('/'))
                    importPath.prepend('/');
                importPath.prepend(QDir::currentPath());
            } else if (importPath.startsWith("sys:")) {
                importPath.remove("sys:");
                if (!importPath.startsWith('/'))
                    importPath.prepend('/');
                importPath.prepend(systemThemePath);
            }
            loader->m_engine->addImportPath(importPath);
        }

    } else {
        ThemeEnginePrivate::setError(QString("QML import %1 allready added!").
                                     arg(import[1]));
        return false;
    }

    return true;
}


/*=============================================================================
  CSS-LIKE THEME LOADER
=============================================================================*/

QthmThemeLoader::QthmThemeLoader(QQmlEngine *engine, QObject *parent):
    ThemeLoader(engine, parent),
     styleTree(0)
{
    // fill the callback maps
    rules["import url"] = handleImport;
    rules["qml-mapping"] = handleQmlMapping;
    rules["qml-import"] = handleQmlImport;
}

StyleTreeNode * QthmThemeLoader::loadTheme(const QUrl &url)
{
    styleTree = 0;
    // parses the theme
    if (parseTheme(url)) {

        normalizeStyles();
        // build up the QML style tree
        styleTree = new StyleTreeNode(0);
        if (!buildStyleTree()) {
            delete styleTree;
            styleTree = 0;
        }

        // cleanup
        imports.clear();
        qmlMap.clear();
        selectorTable.clear();
    }

    return styleTree;
}
