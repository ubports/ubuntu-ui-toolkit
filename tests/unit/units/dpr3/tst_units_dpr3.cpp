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

class tst_UCUnitsDPR3: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void dpGridUnitDefaultWithDPR3() {
        UCUnits units;

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.33333f);
        QCOMPARE(units.dp(1.72), 1.66666f);
        QCOMPARE(units.dp(0.23), 0.333333f);
        QCOMPARE(units.dp(0.51), 0.666666f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1000.0f);
    }

    void guGridUnitDefaultWithDPR3() {
        UCUnits units;

        QCOMPARE(units.gu(0.5), 4.0f);
        QCOMPARE(units.gu(1), 8.0f);
        QCOMPARE(units.gu(1.5), 12.0f);
        QCOMPARE(units.gu(2), 16.0f);
        QCOMPARE(units.gu(4), 32.0f);
        QCOMPARE(units.gu(100000), 800000.0f);
        QCOMPARE(units.gu(150.51983), 1204.0f);
    }

    void dpGridUnitSixteenWithDPR3() {
        UCUnits units;
        units.setGridUnit(16);

        QCOMPARE(units.dp(1.0), 0.666666f);
        QCOMPARE(units.dp(1.32), 1.0f);
        QCOMPARE(units.dp(1.72), 1.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 0.333333f);
        QCOMPARE(units.dp(0.9999), 0.666666f);
        QCOMPARE(units.dp(1000.01), 666.666666f);
    }

    void guGridUnitSixteenWithDPR3() {
        UCUnits units;
        units.setGridUnit(16);

        QCOMPARE(units.gu(0.5), 2.666666f);
        QCOMPARE(units.gu(1), 5.333333f);
        QCOMPARE(units.gu(1.5), 8.0f);
        QCOMPARE(units.gu(2), 10.666666f);
        QCOMPARE(units.gu(4), 21.333333f);
        QCOMPARE(units.gu(100000), 533333.333333f);
        QCOMPARE(units.gu(150.51983), 802.666666f);
    }
};

//QTEST_MAIN(tst_UCUnitsDPR3) - want to set custom env var, so need to use actual code:
int main(int argc, char *argv[])
{
    qputenv("QT_DEVICE_PIXEL_RATIO", "3");

    QGuiApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    tst_UCUnitsDPR3 tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_units_dpr3.moc"
