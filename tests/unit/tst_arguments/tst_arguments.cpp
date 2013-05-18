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
#include "ucarguments.h"
#undef protected

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtTest/QTest>

class tst_UCArguments : public QObject
{
    Q_OBJECT

public:
    tst_UCArguments() : m_application(NULL) {}

private:
    QVector<char*> m_arguments;
    int m_argumentsSize;
    QCoreApplication* m_application;

    void clearCommandLine() {
        m_arguments.clear();
        m_argumentsSize = 0;

        if (m_application != NULL) {
            delete m_application;
            m_application = NULL;
        }
    }

    void setCommandLine(QString commandLine) {
        clearCommandLine();
        QStringList arguments = QString(commandLine).split(' ', QString::SkipEmptyParts);

        Q_FOREACH (QString argument, arguments) {
            QByteArray text = argument.toLocal8Bit();
            char *data = new char[text.size() + 1];
            strcpy(data, text.data());
            m_arguments.append(data);
        }

        // FIXME: real binary name
        char* binaryName = (char*)"binary";
        m_arguments.prepend(binaryName);

        m_argumentsSize = m_arguments.size();
        m_application = new QCoreApplication(m_argumentsSize, m_arguments.data());
    }

    void testCommandLine(QString commandLine, bool expectedError, QString dataTag=QString()) {
        if (dataTag.isEmpty()) {
            dataTag = commandLine;
        }
        QTest::newRow(dataTag.toLocal8Bit().constData()) << commandLine << expectedError;
    }

private Q_SLOTS:
    void init() {
    }

    void cleanup() {
        clearCommandLine();
    }

    void testNoArguments() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);

        setCommandLine(commandLine);

        UCArguments arguments;
        arguments.componentComplete();

        QCOMPARE(arguments.error(), error);
    }

    void testNoArguments_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<bool>("error");
        testCommandLine("", false, "NO ARGUMENTS");
        testCommandLine("--boolArgument", false);
        testCommandLine("--boolArgument --boolArgument", false);
        testCommandLine("--boolArgument --otherBool", false);
        testCommandLine("--boolArgument --otherArg1=value --otherArg2 value", false);
        testCommandLine("--boolArgument --otherArg=value defaultValue1 defaultValue2", false);
        testCommandLine("--otherArg=value --boolArgument defaultValue1 defaultValue2", false);
        testCommandLine("--otherArg=value defaultValue1 defaultValue2 --boolArgument", false);
        testCommandLine("--otherArg=value defaultValue1 defaultValue2", false);
    }

    void testOneRequiredNamedBoolArgument() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);

        setCommandLine(commandLine);

        UCArguments arguments;
        UCArgument boolArgument;
        boolArgument.setName("boolArgument");
        arguments.appendArguments(&boolArgument);
        arguments.componentComplete();

        QCOMPARE(arguments.values()->property("boolArgument").type(), QVariant::Bool);
        QCOMPARE(arguments.values()->property("boolArgument"), QVariant(!error));
        QCOMPARE(arguments.error(), error);
    }

    void testOneRequiredNamedBoolArgument_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<bool>("error");
        testCommandLine("", true, "NO ARGUMENTS");
        testCommandLine("--boolArgument", false);
        testCommandLine("--boolArgument --boolArgument", false);
        testCommandLine("--boolArgument --otherBool", false);
        testCommandLine("--boolArgument --otherArg1=value --otherArg2 value", false);
        testCommandLine("--boolArgument --otherArg=value defaultValue1 defaultValue2", false);
        testCommandLine("--otherArg=value --boolArgument defaultValue1 defaultValue2", false);
        testCommandLine("--otherArg=value defaultValue1 defaultValue2 --boolArgument", false);
        testCommandLine("--otherArg=value defaultValue1 defaultValue2", true);
    }

    void testOneRequiredNamedArgument() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);

        setCommandLine(commandLine);

        UCArguments arguments;
        QStringList valueNames;
        valueNames << "VALUE1";

        UCArgument argument1;
        argument1.setName("argument1");
        argument1.setValueNames(valueNames);
        arguments.appendArguments(&argument1);

        arguments.componentComplete();

        QCOMPARE(arguments.error(), error);
        if (!error) {
            QCOMPARE(arguments.values()->property("argument1").type(), QVariant::String);
            QCOMPARE(arguments.values()->property("argument1").toString(), QString("value1"));
        } else {
            QCOMPARE(arguments.values()->property("argument1").type(), QVariant::Invalid);
        }
    }

    void testOneRequiredNamedArgument_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<bool>("error");
        testCommandLine("", true, "NO ARGUMENTS");
        testCommandLine("--argument1=value1", false);
        testCommandLine("--argument1 value1", false);
        testCommandLine("--otherArgument --argument1 value1", false);
        testCommandLine("--argument1 value1 --otherArgument", false);
        testCommandLine("--argument1 value1 defaultValue --otherArgument", false);
        testCommandLine("--argument1=value1 defaultValue --otherArgument=value", false);
        testCommandLine("--argument1 value1 defaultValue", false);
    }

    void testTwoRequiredNamedArguments() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);

        setCommandLine(commandLine);

        UCArguments arguments;
        QStringList valueNames;
        valueNames << "VALUE1" << "VALUE2";

        UCArgument argument1;
        argument1.setName("argument1");
        argument1.setValueNames(valueNames);
        arguments.appendArguments(&argument1);

        UCArgument argument2;
        argument2.setName("argument2");
        argument2.setValueNames(valueNames);
        arguments.appendArguments(&argument2);

        arguments.componentComplete();

        QCOMPARE(arguments.error(), error);
        if (!error) {
            QCOMPARE(arguments.values()->property("argument1").type(), QVariant::StringList);
            QCOMPARE(arguments.values()->property("argument1").toStringList().size(), 2);
            QCOMPARE(arguments.values()->property("argument2").type(), QVariant::StringList);
            QCOMPARE(arguments.values()->property("argument2").toStringList().size(), 2);
        }
    }

    void testTwoRequiredNamedArguments_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<bool>("error");
        testCommandLine("", true, "NO ARGUMENTS");
        testCommandLine("--argument1=value1 value2 --argument2=value1 value2", false);
        testCommandLine("--argument1 value1 value2 --argument2 value1 value2", false);
        testCommandLine("--otherArgument --argument1 value1 value2 --argument2 value1 value2", false);
        testCommandLine("--argument1 value1 value2 --argument2 value1 value2 --otherArgument", false);
        testCommandLine("--argument1 value1 value2 --argument2 value1 value2 defaultValue --otherArgument", false);
        testCommandLine("--argument1 value1 value2 --argument2 value1 value2 defaultValue", false);
        testCommandLine("--argument1=value1 value2 --argument2=value1", true);
    }

    void testRequiredDefaultArgument() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);

        setCommandLine(commandLine);

        UCArguments arguments;
        QStringList valueNames;
        valueNames << "DEFAULTVALUE1" << "DEFAULTVALUE2";

        UCArgument defaultArgument;
        defaultArgument.setValueNames(valueNames);
        arguments.setDefaultArgument(&defaultArgument);
        arguments.componentComplete();

        QCOMPARE(arguments.error(), error);
        if (!error) {
            QCOMPARE(arguments.defaultArgument(), &defaultArgument);
            QCOMPARE(arguments.defaultArgument()->at(0).type(), QVariant::String);
            QCOMPARE(arguments.defaultArgument()->at(0).toString().isEmpty(), false);
            QCOMPARE(arguments.defaultArgument()->at(0).toString(), QString("defaultValue1"));
            QCOMPARE(arguments.defaultArgument()->at(1).type(), QVariant::String);
            QCOMPARE(arguments.defaultArgument()->at(1).toString().isEmpty(), false);
            QCOMPARE(arguments.defaultArgument()->at(1).toString(), QString("defaultValue2"));
            QCOMPARE(arguments.defaultArgument()->at(2).type(), QVariant::Invalid);
        }
    }

    void testRequiredDefaultArgument_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<bool>("error");
        testCommandLine("", true, "NO ARGUMENTS");
        testCommandLine("--boolArgument", true);
        testCommandLine("--boolArgument --otherArg1=value --otherArg2 value", true);
        testCommandLine("--boolArgument --otherArg=value defaultValue1 defaultValue2", false);
        testCommandLine("--otherArg=value --boolArgument defaultValue1 defaultValue2", false);
        testCommandLine("--otherArg=value defaultValue1 defaultValue2 --boolArgument", false);
        testCommandLine("--otherArg=value defaultValue1 defaultValue2", false);
    }
};

QTEST_MAIN(tst_UCArguments)

#include "tst_arguments.moc"
