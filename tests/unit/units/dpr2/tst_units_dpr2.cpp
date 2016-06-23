/*
 * Copyright (C) 2015 Canonical, Ltd.
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
#include "ucunits.h"

UT_USE_NAMESPACE

class tst_UCUnitsDPR2: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void dpGridUnitDefaultWithDPR2() {
        UCUnits units;

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.5f);
        QCOMPARE(units.dp(1.72), 1.5f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 0.5f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1000.0f);
    }

    void guGridUnitDefaultWithDPR2() {
        UCUnits units;

        QCOMPARE(units.gu(0.5), 4.0f);
        QCOMPARE(units.gu(1), 8.0f);
        QCOMPARE(units.gu(1.5), 12.0f);
        QCOMPARE(units.gu(2), 16.0f);
        QCOMPARE(units.gu(4), 32.0f);
        QCOMPARE(units.gu(100000), 800000.0f);
        QCOMPARE(units.gu(150.51983), 1204.0f);
    }

    void dpGridUnitEightWithDPR2() {
        UCUnits units;
        units.setGridUnit(8);

        QCOMPARE(units.dp(1.0), 0.5f);
        QCOMPARE(units.dp(1.32), 0.5f);
        QCOMPARE(units.dp(1.72), 1.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 0.5f);
        QCOMPARE(units.dp(0.9999), 0.5f);
        QCOMPARE(units.dp(1000.01), 500.0f);
    }

    void guGridUnitEightWithDPR2() {
        UCUnits units;
        units.setGridUnit(8);

        QCOMPARE(units.gu(0.5), 2.0f);
        QCOMPARE(units.gu(1), 4.0f);
        QCOMPARE(units.gu(1.5), 6.0f);
        QCOMPARE(units.gu(2), 8.0f);
        QCOMPARE(units.gu(4), 16.0f);
        QCOMPARE(units.gu(100000), 400000.0f);
        QCOMPARE(units.gu(150.51983), 602.0f);
    }

    void dpGridUnitSixteenWithDPR2() {
        UCUnits units;
        units.setGridUnit(16);
        /* This testcase covers unit calculations when the Qt device pixel ratio is 2.
         *
         */

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.5f);
        QCOMPARE(units.dp(1.72), 1.5f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 0.5f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1000.0f);
    }

    void guGridUnitSixteenWithDPR2() {
        UCUnits units;
        units.setGridUnit(16);

        QCOMPARE(units.gu(0.5), 4.0f);
        QCOMPARE(units.gu(1), 8.0f);
        QCOMPARE(units.gu(1.5), 12.0f);
        QCOMPARE(units.gu(2), 16.0f);
        QCOMPARE(units.gu(4), 32.0f);
        QCOMPARE(units.gu(100000), 800000.0f);
        QCOMPARE(units.gu(150.51983), 1204.0f);
    }

    void dpGridUnitTwentyWithDPR2() {
        UCUnits units;
        units.setGridUnit(20);

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.5f);
        QCOMPARE(units.dp(1.72), 1.5f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 0.5f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1250.0f);
    }

    void guGridUnitTenWithDPR2() {
        UCUnits units;
        units.setGridUnit(10);

        QCOMPARE(units.gu(0.5), 2.5f);
        QCOMPARE(units.gu(1), 5.0f);
        QCOMPARE(units.gu(1.5), 7.5f);
        QCOMPARE(units.gu(2), 10.0f);
        QCOMPARE(units.gu(4), 20.0f);
        QCOMPARE(units.gu(100000), 500000.0f);
        QCOMPARE(units.gu(150.51983), 752.5f);
    }
};

//QTEST_MAIN(tst_UCUnitsDPR2) - want to set custom env var, so need to use actual code:
int main(int argc, char *argv[])
{
    qputenv("QT_DEVICE_PIXEL_RATIO", "2");

    QGuiApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    tst_UCUnitsDPR2 tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_units_dpr2.moc"
