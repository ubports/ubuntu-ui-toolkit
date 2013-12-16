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
 */

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include "uctestcase.h"

class RecreateViewTest : public QObject
{
    Q_OBJECT

public:
    RecreateViewTest() {}

private Q_SLOTS:
    void initTestCase();
    void testCase_recreateView();

private:
};

void RecreateViewTest::initTestCase()
{
}

void RecreateViewTest::testCase_recreateView()
{
    UbuntuTestCase *testCase = new UbuntuTestCase("SimpleApp.qml");
    delete testCase;

    testCase = new UbuntuTestCase("SimpleApp.qml");
    delete testCase;
}

QTEST_MAIN(RecreateViewTest)

#include "tst_recreateview.moc"
