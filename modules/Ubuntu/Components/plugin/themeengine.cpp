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
#include "itemstyleattached.h"
#include "itemstyleattached_p.h"
#include "themeloader_p.h"
#include "qmlthemeloader_p.h"
#include <QtCore/QString>
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

/*!
  \qmltype Theme
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup theming
  \brief The Theme element provides functionality to change the current theme.

  Informs the application about the current theme file used through \a currentTheme
  property and notifies the items and application about successful theme change
  through \a themeChanged() signal. When error occurs during theme loading, it is
  reported through \a error property.
*/

ThemeEngine *ThemeEnginePrivate::themeEngine = 0;

/*=============================================================================
  THEMING ENGINE PRIVATES
=============================================================================*/

ThemeEnginePrivate::ThemeEnginePrivate(ThemeEngine *qq) :
    q_ptr(qq),
    m_engine(qobject_cast<QQmlEngine*>(qq->parent())),
    firstThemeLoaded(false)
{
    themeEngine = q_ptr;

    // register theme loaders
    // so far we have one single loader, however keep the design so we can add
    // more of them later if needed
    themeLoaders[".qmltheme"] = new QmlThemeLoader(m_engine);

    // connect theme settings to capture theme updates
    QObject::connect(&themeSettings, SIGNAL(themeSettingsChanged()), q_ptr, SLOT(_q_updateTheme()));

    // connect theme watcher to the same slot so we update the theme whenever we sense changes
    QObject::connect(&themeWatcher, SIGNAL(fileChanged(QString)), q_ptr, SLOT(_q_reloadTheme()));
}

ThemeEnginePrivate::~ThemeEnginePrivate()
{
    // FIXME: no need anymore for multiple theme file format loader support
    QMapIterator<QString, ThemeLoader*> i(themeLoaders);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
}

void ThemeEnginePrivate::_q_reloadTheme()
{
    currentTheme = QString();
    _q_updateTheme();
}

/*!
  \internal
  Reads the theme descriptor file and changes the theme accordingly
*/
void ThemeEnginePrivate::_q_updateTheme()
{
    const QUrl newTheme = themeSettings.themeFile();
    if (newTheme.isValid() && (currentTheme != newTheme)) {
        // remove previous import paths and add the ones defined for the new theme
        QStringList importList = m_engine->importPathList();
        if (!importPaths.isEmpty()) {
            Q_FOREACH (const QString &import, importPaths)
                importList.removeAll(import);
        }
        importPaths = themeSettings.imports();
        if (!importPaths.isEmpty()) {
            importList << importPaths;
            m_engine->setImportPathList(importList);
        }
        // load the theme
        loadTheme(newTheme);
    }
}

void ThemeEnginePrivate::removeWatchedFiles()
{
    QStringList watchedThemeFiles = themeWatcher.files();
    Q_FOREACH(const QString &file, watchedThemeFiles) {
        themeWatcher.removePath(file);
    }
}

void ThemeEnginePrivate::addWatchedFiles(const QStringList &watchedThemeFiles)
{
    Q_FOREACH(const QString &file, watchedThemeFiles) {
        themeWatcher.addPath(file);
    }
}

/*!
  \internal
  Loads the theme using the registered theme file format loaders. Aliased theme
  files stored in the resources should contain the file format, otherwise the
  engine cannot detect the file format.
  */
void ThemeEnginePrivate::loadTheme(const QUrl &themeFile)
{
    QUrl url(themeFile);
    if (!url.isValid())
        url = themeSettings.themeFile();

    QString fileType = url.path();
    fileType = fileType.right(fileType.length() - fileType.lastIndexOf('.'));

    if (themeLoaders.contains(fileType)) {
        removeWatchedFiles();
        QStringList watchedThemeFiles;
        m_styleCache.clear();
        bool ok = themeLoaders.value(fileType)->loadTheme(url, watchedThemeFiles, m_styleCache);
        if (ok && errorString.isEmpty()) {
            addWatchedFiles(watchedThemeFiles);
            // emit themeChanged() to update style
            currentTheme = url;
            Q_Q(ThemeEngine);
            Q_EMIT q->themeChanged();
        }
    } else {
        setError("Unknown theme URL" + url.toString());
    }
}

/*!
  \internal
  This method searches for styling components matching the given selector (style path).
*/
StyleCache::StyleData *ThemeEnginePrivate::styleRuleForPath(const Selector &path)
{
    if (!themeEngine)
        return 0;
    return themeEngine->d_ptr->m_styleCache.match(path);
}


/*=============================================================================
  Utility functions
=============================================================================*/

/*!
  \internal
  Checks whether the instance can be registered to the given name, and registers it.
  Removes any previous registration.
*/
bool ThemeEnginePrivate::registerName(QQuickItem *item, const QString &newName)
{
    bool ret = true;

    // check first whether the next ID is valid and can be registered
    QString prevName(item->property("name").toString());
    if (newName.isEmpty()) {
        // remove the previous occurence
        if (!prevName.isEmpty())
            themeEngine->d_ptr->m_instanceCache.remove(prevName);
    } else {
        if (themeEngine->d_ptr->m_instanceCache.contains(newName))
            ret = false;
        else {
            // remove the previous occurence
            if (!prevName.isEmpty())
                themeEngine->d_ptr->m_instanceCache.remove(prevName);
            // register instance
            themeEngine->d_ptr->m_instanceCache.insert(newName, item);
        }
    }

    return ret;
}

/*!
  \internal
  Sets the error string and emits the signal associated to the public property,
  so error handling is propagated to the UI.
  */
void ThemeEnginePrivate::setError(const QString &error)
{
    themeEngine->d_ptr->errorString = error;
    qDebug() << error;
    Q_EMIT themeEngine->errorChanged();
}

/*!
  \internal
  Returns the style propertyies object attached to an item.
  */
ItemStyleAttached *ThemeEnginePrivate::attachedStyle(QObject *obj)
{
    if (!obj)
        return 0;
    QObject *attached = qmlAttachedPropertiesObject<ItemStyleAttached>(obj, false);
    return qobject_cast<ItemStyleAttached*>(attached);
}

/*!
  \internal
  Parses and returns the path described by \a selector as a list of
  class and name pairs. Supports selector grouping (separated with commas).
  */
QList<Selector> ThemeEnginePrivate::parseSelector(const QString &selectorString)
{
    QList<Selector> pathList;
    QStringList groupList = selectorString.split(",");

    Q_FOREACH (const QString &group, groupList) {
        pathList.append(Selector(group));
    }
    return pathList;
}


/*=============================================================================
  THEMING ENGINE PUBLIC API
=============================================================================*/

/*!
  \preliminary
  \qmlsignal Theme::themeChanged()
  Emitted when the theme file gets loaded with success.
*/

ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent),
    d_ptr(new ThemeEnginePrivate(this))
{
}

/*!
  \internal
  The method is used by the QML framework to register theme engine instance.

  When called it configures the engine with the given declarative engine and loads
  the last theme configured in the settings. Returns the theme engine's instance
  on successful initialization. Theme loading failure does not affect the success
  of the initialization, however it is reflected in the \a error property.
  Further calls of the function do not re-initialize the engine nor re-load the
  configured theme.
*/
QObject *ThemeEngine::initializeEngine(QQmlEngine *engine)
{
    if (!ThemeEnginePrivate::themeEngine && engine) {
        ThemeEnginePrivate::themeEngine = new ThemeEngine(engine);
    }
    return ThemeEnginePrivate::themeEngine;
}

/*!
  \internal
  The method returns the singleton instance of the theme engine.
  */
ThemeEngine *ThemeEngine::instance()
{
    ThemeEngine *ret = ThemeEnginePrivate::themeEngine;
    if (ret && !ret->d_ptr->firstThemeLoaded) {
        ret->d_ptr->firstThemeLoaded = true;
        ret->d_ptr->_q_updateTheme();
    }
    return ret;
}

/*!
  \preliminary
  \qmlmethod bool Theme::loadTheme(const QUrl &themeFile)
  Loads a theme file from any location, and updates the \a currentTheme property
  on success. Any error messages are reported in the \a error property.
  */
bool ThemeEngine::loadTheme(const QUrl &themeFile)
{
    Q_D(ThemeEngine);
    d->errorString = QString();
    d->loadTheme(themeFile);
    return d->errorString.isEmpty();
}

/*!
  \qmlproperty string Theme::error
  The property contains the error, which occurred upon loading. The error is not cleared
  automatically and must be acknowledged after each operation.
  */
QString ThemeEngine::error() const
{
    Q_D(const ThemeEngine);
    return d->errorString;
}

/*!
  Property reset method for the \a error property.
  */
void ThemeEngine::resetError()
{
    Q_D(ThemeEngine);
    d->errorString = QString();
}


/*!
  \qmlproperty string Theme::currentTheme
  This property holds the current theme file loaded.
  */
QString ThemeEngine::theme() const
{
    Q_D(const ThemeEngine);
    return d->currentTheme.toString();
}

#include "moc_themeengine.cpp"
