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

#include "ucapplication.h"

#include <QtCore/QCoreApplication>
#include <QDebug>

/*!
 * \qmltype UbuntuApplication
 * \instantiates UbuntuApplication
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief UbuntuApplication is a QML binding for a subset of QCoreApplication.
 *
 * UbuntuApplication is a context property in QML.
 * \endqml
 */
UCApplication::UCApplication(QObject* parent) : QObject(parent)
{
}

/*!
 * \qmlproperty string Application::applicationName
 * \internal
 * The name of the application, see QCoreApplication::applicationName
 */
QString UCApplication::applicationName() {
    return QCoreApplication::applicationName();
}

void UCApplication::setApplicationName(QString applicationName) {
    /* QStandardPaths uses the name to build folder names.
       This works across platforms. For confinement we rely on the fact
       that the folders are whitelisted based on the app name. Similar
       to how Unity uses it to distinguish running applications.
     */
    QCoreApplication::setApplicationName(applicationName);
    // Unset organization to skip an extra folder component
    QCoreApplication::setOrganizationName("");
    Q_EMIT applicationNameChanged();
}

