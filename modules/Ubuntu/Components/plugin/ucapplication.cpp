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

/*!
 * \qmltype Application
 * \instantiates Application
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief Application is a QML binding for a subset of QCoreApplication.
 *
 * Application is a context property in QML.
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
    QCoreApplication::setApplicationName(applicationName);
    // Unsetting the organization name to skip an unneeded folder component
    QCoreApplication::setOrganizationName("");
}

/*!
 * \qmlproperty string Application::organizationName
 * \internal
 * The name of the organization, see QCoreApplication::organizationName
 */
QString UCApplication::organizationName() {
    return QCoreApplication::organizationName();
}

void UCApplication::setOrganizationName(QString organizationName) {
    QCoreApplication::setOrganizationName(organizationName);
}


