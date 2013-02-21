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
#include "qmlthemeloader_p.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QDebug>

/*
  QmlTheme file parser

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

const char *stylePropertyFormat = \
        "%1 {\n"
        "%2"
        "    }";

/*!
  \internal
  Resets the node sensitivity flag for the selector nodes so the selector is
  interpreted as defined in CSS. Sensitivity flags are set by the parser to ease
  searching in the selector hash for the base selectors (subsets of the current
  selector).
*/
void normalizeSelector(Selector &selector)
{
    for (int i = 0; i < selector.count(); i++)
        selector[i].sensitivity = SelectorNode::Normal;
}

/*!
  \internal
    Returns a subset from the given selector and configures it to ignore relation
    and name.
  */
Selector selectorSubset(const Selector &path, int elements, SelectorNode::NodeSensitivity sensitivity = SelectorNode::IgnoreAll)
{
    Selector result;
    while (elements > 0) {
        result << path[path.length() - elements];
        result.last().sensitivity = sensitivity;
        elements--;
    }
    return result;
}

/*!
 * \internal
 * Create a QQmlComponent from a given QML string using the given engine
 */
QQmlComponent *createComponent(QQmlEngine *engine, const QString &qmlCode)
{
    if (qmlCode.isEmpty())
        return 0;

    QQmlComponent *ret = new QQmlComponent(engine);
    ret->setData(qmlCode.toLatin1(), QUrl());
    if (ret->isError() || !ret->isReady()) {
        QString errorString = ret->isError() ? ret->errorString() : "Component not ready";
        ThemeEnginePrivate::setError(QString("Error on creating style rule: \n%2\n%3")
                                     .arg(qmlCode)
                                     .arg(errorString));
        delete ret;
        ret = 0;
    }
    return ret;
}

/*!
 * \brief QmlThemeLoader::urlMacro resolves the QmlTheme url() macro.
 */
QString QmlThemeLoader::urlMacro(const QString &param, const QTextStream &stream)
{
    QString path(param);
    path.remove('\"');

    if (!path.startsWith('/') && !path.startsWith("qrc:/") &&
            !path.startsWith("image:/") && !path.startsWith(":/")) {
        // check if we have one of the location tags: current, system, application
        if (path.startsWith("current:")) {
            path.remove("current:");
            if (!path.startsWith('/'))
                path.prepend('/');
            path.prepend(QDir::currentPath());
        } else if (path.startsWith("application:")) {
            path.remove("application:");
            if (!path.startsWith('/'))
                path.prepend('/');
            path.prepend(QCoreApplication::applicationDirPath());
        } else if (path.startsWith("system:")) {
            path.remove("system:");
            if (!path.startsWith('/'))
                path.prepend('/');
            path.prepend(themeFolder());
        } else {
            QFile *file = qobject_cast<QFile*>(stream.device());
            QFileInfo fi(*file);
            //create the path so that returns the absolute path to the URL given
            path = QFileInfo(fi.absoluteDir().path() + '/' + path).absoluteFilePath();
        }
    }
    return path;
}

/*!
  \internal
  Reads the stream character by character skipping characters specified in \a bypassTokens
  parameter and comments. Returns a string which in the most cases contains a single character
  except when a '/' chracter was followed by a valid one, when returns two chars.
  */
QString QmlThemeLoader::readChar(QTextStream &stream, const QRegExp &bypassTokens)
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
  specified in \a bypassTokens will be skipped. \a excludeToken specifies whether
  the limiting token reached should be included in the data read or not (default
  is not to include).
  */
QString QmlThemeLoader::readTillToken(QTextStream &stream, const QRegExp &tokens, const QRegExp &bypassTokens, bool excludeToken)
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
  Special read function for reading declaration block where we may have objects
  defined and we need to count the opened and closed parenthesis.
  Example of such a declaration block:
  \code
  .button {
      pressedAnimation: NumberAnimation {duration: 300; easing.type: Easing.OutQuad};
  }
  \endcode
  */
QString QmlThemeLoader::readDeclarationBlock(QTextStream &stream)
{
    // read till the close brace count is zero
    // the first opened brace is consumed, therefore start from 1
    int braceCount = 1;
    QString chunk, data;

    while (!stream.atEnd() && (braceCount > 0)) {
        chunk = readChar(stream, QRegExp("[\r\n]"));
        if (chunk[0] == '{')
            braceCount++;
        if (chunk[0] == '}')
            braceCount--;
        if (braceCount)
            data += chunk;
    }

    return data;
}

/*!
 * \internal
 * Parses the declaration block and fills the property map from it. Also resolves
 * macros and any other special tokens/tags in property value.
 */
void QmlThemeLoader::parseDeclarationBlock(const QString &blockData, PropertyMap &propertyMap, const QTextStream &stream)
{
    // parse data
    QString propertyName, propertyValue;
    const QChar *data = blockData.constData();
    while (!data->isNull()) {
        if (*data != ':')
            propertyName += *data;
        else {
            // and continue to parse string till we get the declaration end token ';'
            // note that we can have several of those as we can have object blocks inside
            // e.g. animation: NumberAnimation {duration: 100; easing.type: Easing.OutQuad};
            // in which case we need to pay attention on braces, and so on.
            propertyValue.clear();
            int braceCount = 0;
            data++;
            while (!data->isNull()) {
                if ((*data == ';') && !braceCount)
                    break;
                if (*data == '{')
                    braceCount++;
                if (*data == '}')
                    braceCount--;
                propertyValue += *data;
                data++;
            }
            propertyValue = propertyValue.trimmed();
            if (!propertyValue.isEmpty()) {
                // resolve all macros and special tags/tokens
                patchDeclarationValue(propertyValue, stream);
                propertyMap.properties.insert(propertyName.trimmed(), propertyValue);
            }
            // check if we reached the end of the data
            if (data->isNull())
                return;
            propertyName.clear();
            propertyValue.clear();
        }
        data++;
    }
}

/*!
 * \internal
 * Resolves macros, tokens/tags in the property value data.
 */
void QmlThemeLoader::patchDeclarationValue(QString &value, const QTextStream &stream)
{
    // check if the value is declared using url() macro
    int atUrl;
    while ((atUrl = value.indexOf("url")) >= 0) {
        // check if it is the url() function, so the next valid character should be a "(" one
        int pathStart = value.indexOf('(', atUrl);
        if (pathStart >= 0) {
            int pathEnd = value.indexOf(')', pathStart);
            QString path = value.mid(pathStart + 1, pathEnd - pathStart - 1).trimmed();
            // replace url(path) with the resolved one
            value.replace(atUrl, pathEnd - atUrl + 1, urlMacro(path, stream).prepend('\"').append('\"'));
        }
    }
}


/*!
  \internal
  Resolves the "inheritance" between atomic selector items (the last items in a CSS selector component).
  */
void QmlThemeLoader::handleSelector(const Selector &selector, const PropertyMap &newProperties)
{
    PropertyMap propertyMap;
    if (selectorTable.contains(selector))
        propertyMap = selectorTable.value(selector);
    // merge tables; cannot use QHash::unite as that one uses insertMulti() for the existing keys.
    QHashIterator<QString, QString> i(newProperties.properties);
    while (i.hasNext()) {
        i.next();
        propertyMap.properties.insert(i.key(), i.value());
    }
    // save them (back) into the table
    selectorTable.insert(selector, propertyMap);
}

/*!
  \internal
  Normalizes the style table - copies the properties that were not overridden from the base
  styles into the derivates.
  */
void QmlThemeLoader::normalizeStyles()
{
    QHashIterator<Selector, PropertyMap > i(selectorTable);
    while (i.hasNext()) {
        i.next();
        Selector selector = i.key();
        normalizeSelector(i.key());
    }
}

/*!
  \internal
  Updates the properties of a rule based on the subset of the proeprty. May ignore relation
  and/or style ID when collecting properties
  */
bool QmlThemeLoader::updateRuleProperties(Selector &selector, PropertyMap &propertyMap)
{
    bool result = false;
    // check if we have a style that is a subset of the current one and if yes
    // copy the base propertyes that are not overloaded by the current one
    if (!selector[0].styleId.isEmpty()) {
        selector[0].sensitivity |= SelectorNode::IgnoreStyleId;
        selector[0].styleId = QString();
    }
    if (selectorTable.contains(selector)) {
        // make sure the selector is normalized
        normalizeSelector(selector);
        // get the properties and copy the base ones into the current selector
        QHashIterator<QString, QString> baseProperty = selectorTable.value(selector).properties;
        while (baseProperty.hasNext()) {
            baseProperty.next();
            if (!propertyMap.properties.contains(baseProperty.key())) {
                propertyMap.properties.insert(baseProperty.key(), baseProperty.value());
                result = true;
            }
        }
    }
    return result;
}

/*!
 * \internal
 * Normalizes a given selector.
 */
void QmlThemeLoader::normalizeSelector(const Selector &selector)
{
    PropertyMap propertyMap = selectorTable.value(selector);
    if (propertyMap.normalized)
        return;
    // not normalized yet
    if (selector.count() == 1) {
        // only one node, so it's normalized
        propertyMap.normalized = true;
        selectorTable.insert(selector, propertyMap);
        return;
    }
    // need to check only the last node from the selector path
    Selector subset = selectorSubset(selector, 1);
    if (updateRuleProperties(subset, propertyMap))
        selectorTable.insert(selector, propertyMap);
}


/*!
  \internal
  Parses a QmlTheme theme. Reads the stream char-by-char to avoid seeking in the stream.
  Seeking would be needed when reading entire line and parsing tags out of it.
*/
bool QmlThemeLoader::parseTheme(const QUrl &url)
{
    // open the file
    QString fname = (url.scheme() == "qrc") ? url.toString().remove("qrc") : url.path();
    QFile file(fname);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);

        QString data;

        // read stream character by character so we don't need to seek
        while (!stream.atEnd()) {

            data += readChar(stream);

            if (data.isEmpty())
                break;

            if (data[0] == '@') {
                if (!parseAtRules(stream))
                    return false;
                data.clear();
                continue;
            }
            if (!parseDeclarations(data, stream))
                return false;
        }
        return true;
    } else {
        ThemeEnginePrivate::setError(QString("%1: %2")
                                     .arg(file.errorString())
                                     .arg(file.fileName()));
    }
    return false;
}

bool QmlThemeLoader::parseAtRules(QTextStream &stream)
{
    // rule!! read till the first token
    QString data = readTillToken(stream, QRegExp("[({]"), QRegExp("[\t]")).simplified();

    //lookup for the rule handler and continue parsing with it
    if (rules.contains(data))
        return rules.value(data)(this, stream);
    else
        ThemeEnginePrivate::setError(QString("Unhandled rule: %1").arg(data));
    return false;
}

bool QmlThemeLoader::parseDeclarations(QString &data, QTextStream &stream)
{
   // read till we hit a '{' token
   data += readTillToken(stream, QRegExp("[{]"), QRegExp("[\t\r\n]")).simplified();
    if (data.isEmpty())
        return false;

    QList<Selector> selectors = ThemeEnginePrivate::parseSelector(data, SelectorNode::IgnoreRelationship);
    if (selectors.isEmpty()) {
        ThemeEnginePrivate::setError(QString("Syntax error!\n%1").arg(data));
        return false;
    } else {
        // load declarator and apply on each selector
        data = readDeclarationBlock(stream);
        PropertyMap propertyMap;
        if (!data.isEmpty())
            parseDeclarationBlock(data, propertyMap, stream);

        Q_FOREACH (const Selector &selector, selectors) {
            handleSelector(selector, propertyMap);
        }
    }

    // clear data before continuing parsing
    data.clear();
    return true;
}

bool QmlThemeLoader::generateStyleQml()
{
    QString styleQml;
    QString delegateQml;

    // go through the selector map and build the styles to each
    QHashIterator<Selector, PropertyMap > i(selectorTable);
    while (i.hasNext()) {
        i.next();
        Selector selector = i.key();
        PropertyMap propertyMap = i.value();

        buildStyleAndDelegate(selector, propertyMap.properties, styleQml, delegateQml);

        // normalize selector so we build the Rule with the proper one
        normalizeSelector(selector);

        QQmlComponent *style = createComponent(m_engine, styleQml);
        QQmlComponent *delegate = createComponent(m_engine, delegateQml);
        if (!style && !delegate) {
            return false;
        }
        styleTree->addStyleRule(selector, style, delegate);
    }

    return true;
}

QPair<QString, QString> QmlThemeLoader::selectorMapping(const Selector &selector)
{
    Selector subset;
    QString qmap;
    for (int count = selector.count(); count > 0; count--) {
        subset = selectorSubset(selector, count, SelectorNode::Normal);
        qmap = ThemeEnginePrivate::selectorToString(subset);
        if (qmlMap.contains(qmap)) {
            return qmlMap.value(qmap);
        }
    }
    // if none found, check the last node's style class
    qmap = '.' + selector.last().styleClass;
    return (qmlMap.contains(qmap)) ? qmlMap.value(qmap) : QPair<QString, QString>();
}


void QmlThemeLoader::buildStyleAndDelegate(Selector &selector, PropertyHash &properties, QString &style, QString &delegate)
{
    QPair<QString, QString> qmlTypes = selectorMapping(selector);

    style.clear();
    delegate.clear();

    // get the type for style and delegate
    if (properties.count() > 0) {
        QString propertyPrefix("    ");

        if (!qmlTypes.first.isEmpty()) {
            // we have the mapping!!
            style = QString(stylePropertyFormat).arg(qmlTypes.first);
        } else {
            style = QString(stylePropertyFormat).arg("QtObject");
            propertyPrefix += "property var";
        }

        // add properties
        QHashIterator<QString, QString> i(properties);
        QString propertyArg;
        while (i.hasNext()) {
            i.next();
            propertyArg += QString("   %1 %2: %3\n")
                    .arg(propertyPrefix)
                    .arg(i.key())
                    .arg(i.value());
        }
        // append the closing brace
        style = style.arg(propertyArg);
        style = QString(styleRuleComponent).arg(imports).arg(style);
    }

    // delegate
    if (!qmlTypes.second.isEmpty()) {
        delegate = QString("%1{}").arg(qmlTypes.second);
        delegate = QString(styleRuleComponent).arg(imports).arg(delegate);
    }
}

/*============================================================================*/

/*!
  \internal
  Extracts the theme specified to be imported, and loads it.
  */
bool QmlThemeLoader::handleImport(QmlThemeLoader *loader, QTextStream &stream)
{
    QString themeFile = QmlThemeLoader::readTillToken(stream, QRegExp("[;]"), QRegExp("[ )\t\r\n\"]"));
    // check if the imported theme file is an absolute path or not;
    // if not, build the path relative to the current parsed file
    // Note: resource stored theme files must use absolute paths, or should have
    // qrc: scheme specified
    bool result = false;
    themeFile = urlMacro(themeFile, stream);
    if (themeFile.startsWith("qrc"))
        result = loader->parseTheme(QUrl(themeFile));
    else {
        result = loader->parseTheme(QUrl::fromLocalFile(themeFile));
        if (result)
            loader->themeFiles << themeFile;
    }
    return result;
}

/*!
  \internal
  Parses qml-mapping rule needed when defing the QML Rule pattern. The qml-mapping rule
  syntax is as follows:
  qml-mapping(css-tag, style-qml-type, delegate-qml-type)
  where
     - css-tag is the tag used in CSS theme
     - style-qml-type is the name of the QML document defining the style properties
     - delegate-qml-type is the name of the QML document defining the item delegate.
  If no style is given, the theme engine will use QtObject for style and will declare all
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
bool QmlThemeLoader::handleQmlMapping(QmlThemeLoader *loader, QTextStream &stream)
{
    QString params = QmlThemeLoader::readTillToken(stream, QRegExp("[;]"), QRegExp("[)\t\r\n\"]"));
    QStringList mapping = params.split(',');

    // we should have 3 elements in the list! if we don't we have an error!
    bool ret = (mapping.count() == 3);
    if (!ret)
        ThemeEnginePrivate::setError(QString("Mapping has %1 parameter(s), should have 3!").
                                     arg(mapping.count()));
    else
        loader->qmlMap.insert(mapping[0].trimmed().toLower(), qMakePair(mapping[1].trimmed(), mapping[2].trimmed()));

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
bool QmlThemeLoader::handleQmlImport(QmlThemeLoader *loader, QTextStream &stream)
{
    QString param = QmlThemeLoader::readTillToken(stream, QRegExp("[;]"), QRegExp("[)\t\r\n]")).simplified();

    if (param.isEmpty()) {
        ThemeEnginePrivate::setError("Empty QML import statement!");
        return false;
    }

    QStringList import = param.split(',');
    QString importUrl = (import.count() >= 1) ? import[0].simplified().prepend("import ") : QString();
    QString importPath = (import.count() < 2) ? QString() : import[1].simplified();

    // check whether we have the import set
    if (!loader->imports.contains(importUrl)) {
        loader->imports += importUrl + '\n';

        if (!importPath.isEmpty()) {
            importPath = urlMacro(importPath, stream);
            if (!loader->m_engine->importPathList().contains(importPath))
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

QmlThemeLoader::QmlThemeLoader(QQmlEngine *engine):
    styleTree(0)
{
    m_engine = engine;
    // fill the callback maps
    rules["import url"] = handleImport;
    rules["qml-mapping"] = handleQmlMapping;
    rules["qml-import"] = handleQmlImport;
}

StyleTreeNode * QmlThemeLoader::loadTheme(const QUrl &url, QStringList &themeFiles)
{
    styleTree = 0;
    // parses the theme
    if (parseTheme(url)) {

        normalizeStyles();
        // build up the QML style tree
        styleTree = new StyleTreeNode(0);
        if (!generateStyleQml()) {
            delete styleTree;
            styleTree = 0;
        } else
            themeFiles<< url.path() << this->themeFiles;

        // cleanup
        ruleString.clear();
        imports.clear();
        qmlMap.clear();
        selectorTable.clear();
    }

    return styleTree;
}
