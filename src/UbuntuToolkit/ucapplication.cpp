/*
 * Copyright 2013 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.om>
 */

#include "ucapplication_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

UT_NAMESPACE_BEGIN

/*!
 * \qmltype UbuntuApplication
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief UbuntuApplication is a QML binding for a subset of QCoreApplication.
 *
 * UbuntuApplication is a context property in QML.
 */
UCApplication *UCApplication::m_app = nullptr;
UCApplication::UCApplication(QObject* parent) : QObject(parent), m_context(0)
                                                               , m_inputMethod(QGuiApplication::inputMethod())
{
    // Make sure we receive application name changes from C++ modules
    connect(QCoreApplication::instance(), &QCoreApplication::applicationNameChanged,
            this, &UCApplication::applicationNameChanged);
    // Changes to the default layout direction (RTL, LTR)
    QGuiApplication* application = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
    connect(application, &QGuiApplication::layoutDirectionChanged,
            this, &UCApplication::layoutDirectionChanged);
}

UCApplication::~UCApplication()
{
    m_app = nullptr;
}

void UCApplication::setContext(QQmlContext* context) {
    m_context = context;
}

/*!
 * \internal
 * The name of the application, see QCoreApplication::applicationName
 */
QString UCApplication::applicationName() {
    return QCoreApplication::applicationName();
}

void UCApplication::setApplicationName(const QString& applicationName) {
    /* QStandardPaths uses the name to build folder names.
       This works across platforms. For confinement we rely on the fact
       that the folders are whitelisted based on the app name. Similar
       to how Unity uses it to distinguish running applications.
     */
    QCoreApplication::setApplicationName(applicationName);

    /* We want to override the organization name for apps using qmlscene
     * to be sure we get sane storage paths. Otherwise we end up with strange
     * paths like .local/share/QtProject/$APP_NAME which breaks confinement and
     * should actually be .local/share/$APP_NAME
     */
    if (QCoreApplication::organizationName() == QStringLiteral("QtProject")) {
        QCoreApplication::setOrganizationName(QStringLiteral(""));
    }

    /*
       Ensure that LocalStorage and WebKit use the same location
       Docs are ambiguous: in practise applicationName is ignored by default
     */
    QQmlEngine* engine(m_context->engine());
    QString dataFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    engine->setOfflineStoragePath(dataFolder);
    // Get Qtlabs.settings to use a sane path
    QCoreApplication::setOrganizationDomain(applicationName);

}

/*!
 * \internal
 * The global input method. Can be overridden for testing.
 */
QObject* UCApplication::inputMethod() {
    return m_inputMethod;
}

void UCApplication::setInputMethod(QObject* inputMethod) {
    m_inputMethod = inputMethod;
}

/*!
 * \internal
 * The (default) layout direction. Can be overridden for testing,
 * unlike Qt.application.layoutDirection.
 */
Qt::LayoutDirection UCApplication::layoutDirection() {
    return QGuiApplication::layoutDirection();
}

void UCApplication::setLayoutDirection(Qt::LayoutDirection layoutDirection) {
    return QGuiApplication::setLayoutDirection(layoutDirection);
}


UT_NAMESPACE_END
