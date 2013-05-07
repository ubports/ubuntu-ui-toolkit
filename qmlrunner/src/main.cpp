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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include <QtCore/QLatin1String>
//#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
#include <QtQml/QtQml>
#include <QtQuick/QQuickView>

int main(int argc, char** argv)
{
    // WebKit chokes on threads
    qputenv("QML_NO_THREADED_RENDERER", QByteArray("1"));
    // Ensure that QWebProcess will be found
    qputenv("PATH", qgetenv("PATH") + ":/opt/qt5/bin");

    QGuiApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);

    if (app.arguments().length() != 2)
    {
        printf("Usage:\n  %s - /path/to/filename.qml\n", argv[0]);
        exit(1);
    }

    QQuickView window;
    window.setFlags(Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    QObject::connect(window.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
    window.setResizeMode(QQuickView::SizeRootObjectToView);
    window.setSource(QUrl(app.arguments().value(1)));
    if (!window.errors().empty())
        exit(1);
    window.show();

    return app.exec();
}
