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

#include <QtQml/QQmlExtensionPlugin>
#include <ubuntutoolkitmodule.h>

class UbuntuComponentsLabsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(uri == QLatin1String("Ubuntu.Components.Labs"));
        UT_PREPEND_NAMESPACE(UbuntuLabsModule)::defineModule(uri);
    }

    void initializeEngine(QQmlEngine *engine, const char *uri) override
    {
        UT_PREPEND_NAMESPACE(UbuntuLabsModule)::initializeModule(engine, this);
        QQmlExtensionPlugin::initializeEngine(engine, uri);
    }

    ~UbuntuComponentsLabsPlugin()
    {
        UT_PREPEND_NAMESPACE(UbuntuLabsModule)::undefineModule();
    }
};

#include "plugin.moc"

