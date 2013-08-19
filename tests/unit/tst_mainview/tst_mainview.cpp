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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include "i18n.h"
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QStandardPaths>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

class tst_MainView : public QObject
{
    Q_OBJECT

public:
    tst_MainView() {}

private Q_SLOTS:
    void testSetApplicationName() {
        QString appName("com.ubuntu.foo");
        UbuntuI18n::instance().setApplicationName(appName);
        QCOMPARE(UbuntuI18n::instance().applicationName(), appName);
        QCOMPARE(QCoreApplication::applicationName(), appName);
    }

    void testExpectedDataFolder() {
        QString appName("net.weight.gain");
        UbuntuI18n::instance().setApplicationName(appName);
        QString dataFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
        QString xdgDataHome(QProcessEnvironment::systemEnvironment().value("XDG_DATA_HOME",
            QProcessEnvironment::systemEnvironment().value("HOME") + "/.local/share"));
        QString expectedDataFolder(xdgDataHome + "/" + appName);
        QCOMPARE(dataFolder, expectedDataFolder);
    }

    void testExpectedCacheFolder() {
        QString appName("cat.long.very");
        UbuntuI18n::instance().setApplicationName(appName);
        QString cacheFolder(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        QString xdgCacheHome(QProcessEnvironment::systemEnvironment().value("XDG_CACHE_HOME",
            QProcessEnvironment::systemEnvironment().value("HOME") + "/.cache"));
        QString expectedCacheFolder(xdgCacheHome + "/" + appName);
        QCOMPARE(cacheFolder, expectedCacheFolder);
    }
};

QTEST_MAIN(tst_MainView)

#include "tst_mainview.moc"
