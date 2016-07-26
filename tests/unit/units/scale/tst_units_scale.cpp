/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtTest/QtTest>
#include <UbuntuToolkit/private/ucunits_p.h>

#include <qpa/qplatformnativeinterface.h>

UT_USE_NAMESPACE

class tst_UCUnitsScale : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void dpGridUnitDefaultWithScale() {
        UCUnits units;

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.0f);
        QCOMPARE(units.dp(1.72), 2.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1000.0f);

        QMetaObject::invokeMethod(qGuiApp->platformNativeInterface(), "changeScale", Q_ARG(float, 2.5));
        QCOMPARE(units.dp(1.0), 2.0f);
        QCOMPARE(units.dp(1.32), 3.0f);
        QCOMPARE(units.dp(1.72), 3.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 2.0f);
        QCOMPARE(units.dp(1000.01), 2500.0f);
    }

    void guGridUnitDefaultWithScale() {
        UCUnits units;

        QCOMPARE(units.gu(0.5), 4.0f);
        QCOMPARE(units.gu(1), 8.0f);
        QCOMPARE(units.gu(1.5), 12.0f);
        QCOMPARE(units.gu(2), 16.0f);
        QCOMPARE(units.gu(4), 32.0f);
        QCOMPARE(units.gu(100000), 800000.0f);
        QCOMPARE(units.gu(150.51983), 1204.0f);

        QMetaObject::invokeMethod(qGuiApp->platformNativeInterface(), "changeScale", Q_ARG(float, 2.5));
        QCOMPARE(units.gu(0.5), 10.0f);
        QCOMPARE(units.gu(1), 20.0f);
        QCOMPARE(units.gu(1.5), 30.0f);
        QCOMPARE(units.gu(2), 40.0f);
        QCOMPARE(units.gu(4), 80.0f);
        QCOMPARE(units.gu(100000), 2000000.0f);
        QCOMPARE(units.gu(150.51983), 3010.0f);
    }
};

QTEST_MAIN(tst_UCUnitsScale)

#include "tst_units_scale.moc"
