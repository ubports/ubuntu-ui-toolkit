/*
 * Copyright 2015 Canonical Ltd.
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
#include "ucbottomedge.h"
#include "uctestcase.h"
#include "uctestextras.h"

class BottomEdgeTestCase : public UbuntuTestCase
{
    Q_OBJECT
public:
    BottomEdgeTestCase(const QString& file, ResizeMode resize = SizeViewToRootObject, bool assertOnFailure = true, QWindow* parent = 0)
        : UbuntuTestCase(file, resize, assertOnFailure, parent)
    {}
    UCBottomEdge *testItem()
    {
        return findItem<UCBottomEdge*>("testItem");
    }
};


class tst_BottomEdge : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void initTestCase()
    {
        UCTestExtras::registerTouchDevice();
    }

    void cleanup()
    {

    }

    void test_defaults()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("Defaults.qml"));
        QCOMPARE(test->testItem()->height(), test->rootObject()->height());
    }
};

QTEST_MAIN(tst_BottomEdge)

#include "tst_bottomedge.moc"
