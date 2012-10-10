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
#include "units.h"

class tst_Units: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void singletonInstance() {
        Units& units1 = Units::instance();
        Units& units2 = Units::instance();
        QCOMPARE(&units1, &units2);
    }

    void defaultScaleFactor() {
        Units units;
        QCOMPARE(units.scaleFactor(), 1.0f);
    }

    void setScaleFactor() {
        Units units;
        units.setScaleFactor(0.0);
        QCOMPARE(units.scaleFactor(), 0.0f);
        units.setScaleFactor(42.0);
        QCOMPARE(units.scaleFactor(), 42.0f);
    }

    void scaleFactorEnvironmentVariable() {
        QByteArray scaleFactor = QString::number(1.42).toLocal8Bit();
        qputenv("SCALE_FACTOR", scaleFactor);
        Units units;
        QCOMPARE(units.scaleFactor(), 1.42f);
        qputenv("SCALE_FACTOR", "");
    }

    void dpScaleFactorOne() {
        Units units;

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.0f);
        QCOMPARE(units.dp(1.72), 1.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 0.0f);
        QCOMPARE(units.dp(0.9999), 0.0f);
        QCOMPARE(units.dp(1000.01), 1000.0f);
    }

    void dpScaleFactorOnePointFive() {
        Units units;
        units.setScaleFactor(1.5);

        QCOMPARE(units.dp(1.0), 1.0f);
        QCOMPARE(units.dp(1.32), 1.0f);
        QCOMPARE(units.dp(1.72), 2.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 0.0f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 1500.0f);
    }

    void dpScaleFactorTwo() {
        Units units;
        units.setScaleFactor(2.0);

        QCOMPARE(units.dp(1.0), 2.0f);
        QCOMPARE(units.dp(1.32), 2.0f);
        QCOMPARE(units.dp(1.72), 3.0f);
        QCOMPARE(units.dp(0.23), 0.0f);
        QCOMPARE(units.dp(0.51), 1.0f);
        QCOMPARE(units.dp(0.9999), 1.0f);
        QCOMPARE(units.dp(1000.01), 2000.0f);
    }

    void resolveEmpty() {
        Units units;
        QString resolved;
        QString expected;

        resolved = units.resolveResource(QUrl(""));
        expected = QString("");
        QCOMPARE(resolved, expected);

        units.setScaleFactor(2.0);

        resolved = units.resolveResource(QUrl(""));
        expected = QString("");
        QCOMPARE(resolved, expected);
    }

    void resolveNonExisting() {
        Units units;
        QString resolved;
        QString expected;

        resolved = units.resolveResource(QUrl::fromLocalFile("non_existing.png"));
        expected = QString("");
        QCOMPARE(resolved, expected);
    }

    void resolveDirectory() {
        Units units;
        QString resolved;
        QString expected;

        resolved = units.resolveResource(QUrl::fromLocalFile(""));
        expected = QString("");
        QCOMPARE(resolved, expected);
    }

    void resolveExactMatch() {
        Units units;
        QString resolved;
        QString expected;

        units.setScaleFactor(1.0);
        resolved = units.resolveResource(QUrl::fromLocalFile("exact_match.png"));
        expected = QString("1/" + QDir::currentPath() + QDir::separator() + "exact_match.png");
        QCOMPARE(resolved, expected);
    }

    void resolveExactMatchQrc() {
        Units units;
        QString resolved;
        QString expected;

        units.setScaleFactor(1.0);
        resolved = units.resolveResource(QUrl("qrc:/test/prefix/exact_match.png"));
        expected = QString("1/:/test/prefix/exact_match.png");
        QCOMPARE(resolved, expected);
    }

    void resolveLowerScaleFactor() {
        Units units;
        QString resolved;
        QString expected;

        units.setScaleFactor(2.0);
        resolved = units.resolveResource(QUrl::fromLocalFile("lower_scale.png"));
        expected = QString("1.33333/" + QDir::currentPath() + QDir::separator() + "lower_scale@1.5x.png");
        QCOMPARE(resolved, expected);
    }

    void resolveLowerScaleFactorQrc() {
        Units units;
        QString resolved;
        QString expected;

        units.setScaleFactor(2.0);
        resolved = units.resolveResource(QUrl("qrc:/test/prefix/lower_scale.png"));
        expected = QString("1.33333/:/test/prefix/lower_scale@1.5x.png");
        QCOMPARE(resolved, expected);
    }

    void resolveHigherScaleFactor() {
        Units units;
        QString resolved;
        QString expected;

        units.setScaleFactor(1.5);
        resolved = units.resolveResource(QUrl::fromLocalFile("higher_scale.png"));
        expected = QString("0.75/" + QDir::currentPath() + QDir::separator() + "higher_scale@2x.png");
        QCOMPARE(resolved, expected);
    }

    void resolveHigherScaleFactorQrc() {
        Units units;
        QString resolved;
        QString expected;

        units.setScaleFactor(1.5);
        resolved = units.resolveResource(QUrl("qrc:/test/prefix/higher_scale.png"));
        expected = QString("0.75/:/test/prefix/higher_scale@2x.png");
        QCOMPARE(resolved, expected);
    }
};

QTEST_MAIN(tst_Units)

#include "tst_units.moc"
