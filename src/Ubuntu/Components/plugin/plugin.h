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

#ifndef UBUNTU_COMPONENTS_PLUGIN_H
#define UBUNTU_COMPONENTS_PLUGIN_H

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>

class QWindow;

class UbuntuComponentsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
    static const QUrl &pluginUrl()
    {
        return m_baseUrl;
    }
    static void initializeContextProperties(QQmlEngine*);

private Q_SLOTS:
    void registerWindowContextProperty();
    void setWindowContextProperty(QWindow* focusWindow);

private:
    static QUrl m_baseUrl;
    void initializeBaseUrl();
    void registerTypesToVersion(const char *uri, int major, int minor);
};
#endif // UBUNTU_COMPONENTS_PLUGIN_H

