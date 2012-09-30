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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#ifndef UBUNTU_COMPONENTS_I18N_H
#define UBUNTU_COMPONENTS_I18N_H

//#include <QtDeclarative/QDeclarativeEngine>
//#include <QtDeclarative/QDeclarativeExtensionPlugin>
#include <QtDeclarative> // TODO: be more specific
#include <QVariant> // TODO: replace by string
#include <QObject>

//class UbuntuComponentsPlugin : public QDeclarativeExtensionPlugin
class UbuntuI18n : public QObject
{
    Q_OBJECT

public:
    void registerTypes(const char *uri);
    //void initializeEngine(QDeclarativeEngine *engine, const char *uri);
    UbuntuI18n(QObject* parent = NULL);
//    static void registerTypes();
    Q_INVOKABLE  QVariant translate(QVariant var); // TODO: use strings?
};

QML_DECLARE_TYPE(UbuntuI18n)

#endif // UBUNTU_COMPONENTS_PLUGIN_H

