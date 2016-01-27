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

#include <QtTest/QtTest>
#include "ucunits.h"

class tst_UCUnits: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void singletonInstance() {
        UCUnits& units1 = UCUnits::instance();
        UCUnits& units2 = UCUnits::instance();
        QCOMPARE(&units1, &units2);
    }

    void defaultGridUnit() {
        qputenv("GRID_UNIT_PX", "");
        UCUnits units;
        QCOMPARE(units.gridUnit(), 8.0f);
    }

    void setGridUnit() {
        UCUnits units;
        units.setGridUnit(0.0);
        QCOMPARE(units.gridUnit(), 0.0f);
        units.setGridUnit(42.0);
        QCOMPARE(units.gridUnit(), 42.0f);
    }

    void gridUnitEnvironmentVariable() {
        QByteArray gridUnit = QString::number(11).toLocal8Bit();
        qputenv("GRID_UNIT_PX", gridUnit);
        UCUnits units;
        QCOMPARE(units.gridUnit(), 11.0);
        qputenv("GRID_UNIT_PX", "");
    }

    void dpGridUnitDefault() {
        UCUnits units;

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.0f);
        QCOMPARE(units.dp(1.72), 2.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1000.0f);
    }

    void guGridUnitDefault() {
        UCUnits units;

        QCOMPARE(units.gu(1.0), 8.0f);
        QCOMPARE(units.gu(1.32), 11.0f);
        QCOMPARE(units.gu(1.72), 14.0f);
        QCOMPARE(units.gu(0.23), 2.0f);
        QCOMPARE(units.gu(0.51), 4.0f);
        QCOMPARE(units.gu(0.9999), 8.0f);
        QCOMPARE(units.gu(1000.01), 8000.0f);
    }
    void dpGridUnitTen() {
        UCUnits units;
        units.setGridUnit(10);

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.0f);
        QCOMPARE(units.dp(1.72), 2.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1250.0f);
    }

    void guGridUnitTen() {
        UCUnits units;
        units.setGridUnit(10);

        QCOMPARE(units.gu(0.5), 5.0f);
        QCOMPARE(units.gu(1), 10.0f);
        QCOMPARE(units.gu(1.5), 15.0f);
        QCOMPARE(units.gu(2), 20.0f);
        QCOMPARE(units.gu(4), 40.0f);
        QCOMPARE(units.gu(100000), 1000000.0f);
        QCOMPARE(units.gu(150.51983), 1505.0f);
    }

    void dpGridUnitSixteen() {
        UCUnits units;
        units.setGridUnit(16);

        QCOMPARE(units.dp(1.0), 2.0f);
        QCOMPARE(units.dp(1.32), 3.0f);
        QCOMPARE(units.dp(1.72), 3.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 2.0f);
        QCOMPARE(units.dp(1000.01), 2000.0f);
    }

    void guGridUnitSixteen() {
        UCUnits units;
        units.setGridUnit(16);

        QCOMPARE(units.gu(0.5), 8.0f);
        QCOMPARE(units.gu(1), 16.0f);
        QCOMPARE(units.gu(1.5), 24.0f);
        QCOMPARE(units.gu(2), 32.0f);
        QCOMPARE(units.gu(4), 64.0f);
        QCOMPARE(units.gu(100000), 1600000.0f);
        QCOMPARE(units.gu(150.51983), 2408.0f);
    }

    void dpGridUnitEighteen() {
        UCUnits units;
        units.setGridUnit(18);

        QCOMPARE(units.dp(1.0), 2.0f);
        QCOMPARE(units.dp(1.32), 3.0f);
        QCOMPARE(units.dp(1.72), 3.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 2.0f);
        QCOMPARE(units.dp(1000.01), 2250.0f);
    }

    void dpGridUnitTwenty() {
        UCUnits units;
        units.setGridUnit(20);

        QCOMPARE(units.dp(1.0), 2.0f);
        QCOMPARE(units.dp(1.32), 3.0f);
        QCOMPARE(units.dp(1.72), 3.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 2.0f);
        QCOMPARE(units.dp(1000.01), 2500.0f);
    }

    void resolveEmpty() {
        UCUnits units;
        QString resolved;
        QString expected;

        resolved = units.resolveResource(QUrl(""));
        expected = QString("");
        QCOMPARE(resolved, expected);

        units.setGridUnit(18);

        resolved = units.resolveResource(QUrl(""));
        expected = QString("");
        QCOMPARE(resolved, expected);
    }

    void resolveNonExisting() {
        UCUnits units;
        QString resolved;
        QString expected;

        resolved = units.resolveResource(QUrl::fromLocalFile("non_existing.png"));
        expected = QString("");
        QCOMPARE(resolved, expected);
    }

    void resolveDirectory() {
        UCUnits units;
        QString resolved;
        QString expected;

        resolved = units.resolveResource(QUrl::fromLocalFile(""));
        expected = QString("");
        QCOMPARE(resolved, expected);
    }

    void resolveExactMatch() {
        UCUnits units;
        QString resolved;
        QString expected;

        units.setGridUnit(8);
        resolved = units.resolveResource(QUrl::fromLocalFile("exact_match.png"));
        expected = QString("1/" + QDir::currentPath() + QDir::separator() + "exact_match@8.png");
        QCOMPARE(resolved, expected);
    }

    void resolveExistsOnlyHigherGridUnit() {
        UCUnits units;
        QString resolved;
        QString expected;

        units.setGridUnit(10);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource_only_higher.png"));
        expected = QString("0.769231/" + QDir::currentPath() + QDir::separator() + "resource_only_higher@13.png");
        QCOMPARE(resolved, expected);
    }

    void resolveExistsOnlyHigherGridUnitQrc() {
        UCUnits units;
        QString resolved;
        QString expected;

        units.setGridUnit(10);
        resolved = units.resolveResource(QUrl("qrc:/test/prefix/resource_only_higher.png"));

        expected = QString("0.769231/:/test/prefix/resource_only_higher@13.png");
        QCOMPARE(resolved, expected);
    }

    void resolveExistsOnlySmallerGridUnit() {
        UCUnits units;
        QString resolved;
        QString expected;

        units.setGridUnit(10);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource_only_smaller.png"));
        expected = QString("1.42857/" + QDir::currentPath() + QDir::separator() + "resource_only_smaller@7.png");
        QCOMPARE(resolved, expected);
    }

    void resolveExistsOnlySmallerGridUnitQrc() {
        UCUnits units;
        QString resolved;
        QString expected;

        units.setGridUnit(10);
        resolved = units.resolveResource(QUrl("qrc:/test/prefix/resource_only_smaller.png"));
        expected = QString("1.42857/:/test/prefix/resource_only_smaller@7.png");
        QCOMPARE(resolved, expected);
    }

    void resolveExistsMultipleGridUnit() {
        UCUnits units;
        QString resolved;
        QString expected;

        units.setGridUnit(20);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource.png"));
        expected = QString("1.05263/" + QDir::currentPath() + QDir::separator() + "resource@19.png");
        QCOMPARE(resolved, expected);

        units.setGridUnit(19);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource.png"));
        expected = QString("1/" + QDir::currentPath() + QDir::separator() + "resource@19.png");
        QCOMPARE(resolved, expected);

        units.setGridUnit(18);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource.png"));
        expected = QString("1/" + QDir::currentPath() + QDir::separator() + "resource@18.png");
        QCOMPARE(resolved, expected);

        units.setGridUnit(17);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource.png"));
        expected = QString("0.944444/" + QDir::currentPath() + QDir::separator() + "resource@18.png");
        QCOMPARE(resolved, expected);

        units.setGridUnit(9);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource.png"));
        expected = QString("0.9/" + QDir::currentPath() + QDir::separator() + "resource@10.png");
        QCOMPARE(resolved, expected);

        units.setGridUnit(8);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource.png"));
        expected = QString("1/" + QDir::currentPath() + QDir::separator() + "resource@8.png");
        QCOMPARE(resolved, expected);

        units.setGridUnit(7);
        resolved = units.resolveResource(QUrl::fromLocalFile("resource.png"));
        expected = QString("0.875/" + QDir::currentPath() + QDir::separator() + "resource@8.png");
        QCOMPARE(resolved, expected);
    }
};

QTEST_MAIN(tst_UCUnits)

#include "tst_units.moc"
