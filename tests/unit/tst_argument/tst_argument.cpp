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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#define protected public
#include "ucargument.h"
#undef protected

#include <QtCore/QString>
#include <QtTest/QTest>

class tst_UCArgument : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAt() {
        UCArgument argument;

        QCOMPARE(argument.at(-2).type(), QVariant::Invalid);
        QCOMPARE(argument.at(0).type(), QVariant::Invalid);
        QCOMPARE(argument.at(1).type(), QVariant::Invalid);
        QCOMPARE(argument.at(2).type(), QVariant::Invalid);

        QStringList values;
        values << "value1" << "value2";
        argument.setValues(values);

        QCOMPARE(argument.at(-2).type(), QVariant::Invalid);
        QCOMPARE(argument.at(0).type(), QVariant::String);
        QCOMPARE(argument.at(1).type(), QVariant::String);
        QCOMPARE(argument.at(2).type(), QVariant::Invalid);

        QCOMPARE(argument.at(0).toString(), QString("value1"));
        QCOMPARE(argument.at(1).toString(), QString("value2"));
    }

    void testSyntax() {
        QFETCH(QString, name);
        QFETCH(QStringList, valueNames);
        QFETCH(bool, required);
        QFETCH(QString, expectedSyntax);

        UCArgument argument;
        argument.setName(name);
        argument.setRequired(required);
        argument.setValueNames(valueNames);

        QCOMPARE(argument.syntax(), expectedSyntax);
    }

    void testSyntax_data() {
        QTest::addColumn<QString>("name");
        QTest::addColumn<QStringList>("valueNames");
        QTest::addColumn<bool>("required");
        QTest::addColumn<QString>("expectedSyntax");

        QStringList valueNames;
        valueNames << "VALUE1" << "VALUE2";

        QTest::newRow("no name, no valueNames, not required") << "" << QStringList() << false << "";
        QTest::newRow("name, no valueNames, not required") << "argument" << QStringList() << false << "--argument";
        QTest::newRow("no name, 2 valueNames, not required") << "" << valueNames << false << "[VALUE1] [VALUE2]";
        QTest::newRow("name, 2 valueNames, not required") << "argument" << valueNames << false << "--argument=VALUE1 VALUE2";
        QTest::newRow("no name, no valueNames, required") << "" << QStringList() << true << "";
        QTest::newRow("name, no valueNames, required") << "argument" << QStringList() << true << "--argument";
        QTest::newRow("no name, 2 valueNames, required") << "" << valueNames << true << "VALUE1 VALUE2";
        QTest::newRow("name, 2 valueNames, required") << "argument" << valueNames << true << "--argument=VALUE1 VALUE2";
    }
};

QTEST_MAIN(tst_UCArgument)

#include "tst_argument.moc"
