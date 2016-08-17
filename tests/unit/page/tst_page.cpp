/*
 * Copyright 2014 Canonical Ltd.
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

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QDir>

#include "uctestcase.h"

class tst_Page : public QObject
{
    Q_OBJECT

public:
    tst_Page()
    {
    }

private Q_SLOTS:

    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void testAnchorToPage_bug1249386() {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("AnchorToPage.qml"));
        QCOMPARE(testCase->warnings(), 0);
    }
};

QTEST_MAIN(tst_Page)

#include "tst_page.moc"
