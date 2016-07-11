/*
 * Copyright 2016 Canonical Ltd.
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

#ifndef UBUNTUTOOLKITMODULE_H
#define UBUNTUTOOLKITMODULE_H

#include <QtCore/QObject>
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>
#include <qqml.h>
#include <ubuntutoolkitglobal.h>

class QQmlEngine;
class QWindow;
class QQmlExtensionPlugin;
/*
 * Type registration functions.
 */

template<typename T>
QObject *qmlRegisterSimpleSingletonTypeCallback(QQmlEngine *, QJSEngine *)
{
    return (new T);
}

template<typename T>
int qmlRegisterSimpleSingletonType(const char *uri, int major, int minor, const char *typeName)
{
    return qmlRegisterSingletonType<T>(uri, major, minor, typeName, qmlRegisterSimpleSingletonTypeCallback<T>);
}

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UbuntuToolkitModule : public QObject
{
    Q_OBJECT
public:

    static QUrl baseUrl(QQmlEngine *engine);

    static void initializeModule(QQmlEngine *engine, const QUrl &pluginBaseUrl);
    static void defineModule();
    static void undefineModule();

    // use this API only in tests!
    static void initializeContextProperties(QQmlEngine*);
private:
    explicit UbuntuToolkitModule(QObject *parent = 0);
    static UbuntuToolkitModule* create(QQmlEngine *engine, const QUrl &baseUrl);
    void registerWindowContextProperty();
    Q_SLOT void setWindowContextProperty(QWindow* focusWindow);
    static void registerTypesToVersion(const char *uri, int major, int minor);

    QUrl m_baseUrl;
};

class UBUNTUTOOLKIT_EXPORT UbuntuStylesModule
{
public:
    static void defineModule(const char *uri);
    static void undefineModule();
};

class UBUNTUTOOLKIT_EXPORT UbuntuLabsModule
{
public:
    static void initializeModule(QQmlEngine *engine, QQmlExtensionPlugin *plugin);
    static void defineModule(const char *uri);
    static void undefineModule();
};

UT_NAMESPACE_END

#endif // UBUNTUTOOLKITMODULE_H
