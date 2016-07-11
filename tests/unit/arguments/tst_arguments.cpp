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
#include <UbuntuToolkit/private/ucarguments_p.h>
#undef protected

#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include "uctestcase.h"
#include <QtQml/QQmlEngine>
#include <UbuntuToolkit/private/i18n_p.h>

UT_USE_NAMESPACE

class tst_UCArguments : public QObject
{
    Q_OBJECT

public:
    tst_UCArguments() : m_application(NULL) {}

private:
    QVector<char*> m_arguments;
    int m_argumentsSize;
    QCoreApplication* m_application;
    QQmlEngine *engine = Q_NULLPTR;

    void clearCommandLine() {
        m_arguments.clear();
        m_argumentsSize = 0;

        delete engine;
        engine = Q_NULLPTR;
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

        char* binaryName = (char*)"binary";
        m_arguments.prepend(binaryName);

        m_argumentsSize = m_arguments.size();
        m_application = new QCoreApplication(m_argumentsSize, m_arguments.data());
        engine = new QQmlEngine;
        // initialize i18 only
        UbuntuI18n::instance(engine);
    }

    void testCommandLine(QString commandLine, bool expectedError, QString dataTag=QString()) {
        if (dataTag.isEmpty()) {
            dataTag = commandLine;
        }
        QTest::newRow(dataTag.toLocal8Bit().constData()) << commandLine << expectedError;
    }

    void testCommandLineUsage(QString commandLine, bool expectedError, bool usage=false, QString dataTag=QString()) {
        QTest::newRow(dataTag.toLocal8Bit().constData()) << commandLine << expectedError << usage;
    }

    void testCommandLineForUnicode(QString argumentValue) {
        QTest::newRow(argumentValue.toLocal8Bit().constData()) << QString("--unicodeArgument ").append(argumentValue) << argumentValue;
    }

private Q_SLOTS:
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

    void testUsage() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);

        setCommandLine(commandLine);

        UCArguments arguments;
        if (error) {
            QString usage(arguments.usage());
            QTest::ignoreMessage(QtWarningMsg, usage.toUtf8());
        }
        arguments.componentComplete();

        QCOMPARE(arguments.error(), error);
    }

    void testUsage_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<bool>("error");
        testCommandLine("", false, "NO ARGUMENTS");
        testCommandLine("--usage", true);
        testCommandLine("--help", true);
        testCommandLine("-h", true);
        testCommandLine("--boolArgument --usage", true);
        testCommandLine("--boolArgument --otherArgument --usage", true);
        testCommandLine("--usage --boolArgument --otherArgument", true);
    }

    void testUnicodeArgument() {
        /* Pretend that the system is UTF-8 so that the inputted command line
         * that is itself UTF-8 is correctly decoded by Qt.
         */
        QTextCodec* previousCodec = QTextCodec::codecForLocale();
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

        QFETCH(QString, commandLine);
        QFETCH(QString, expectedValue);

        setCommandLine(commandLine);

        UCArguments arguments;
        QStringList valueNames;
        valueNames << "VALUE1";

        UCArgument unicodeArgument;
        unicodeArgument.setName("unicodeArgument");
        unicodeArgument.setValueNames(valueNames);
        arguments.appendArguments(&unicodeArgument);
        arguments.componentComplete();

        QCOMPARE(arguments.values()->property("unicodeArgument").type(), QVariant::String);
        QCOMPARE(arguments.values()->property("unicodeArgument").toString(), expectedValue);

        QTextCodec::setCodecForLocale(previousCodec);
    }

    void testUnicodeArgument_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<QString>("expectedValue");

        testCommandLineForUnicode(QString("DIRECCIÓN"));
        testCommandLineForUnicode(QString("ファイル名を"));
        testCommandLineForUnicode(QString("☭☢€→☎❄♫✂▷✇♎⇧☮♻⌘⌛☘"));
        testCommandLineForUnicode(QString("file://Vidéos/Le.goût.des.autres.-.DIVX[www.makingoff.org]"));
    }

    void testOneRequiredNamedBoolArgument() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);
        QFETCH(bool, usage);

        setCommandLine(commandLine);

        UCArguments arguments;
        UCArgument boolArgument;
        boolArgument.setName("boolArgument");
        arguments.appendArguments(&boolArgument);
        if (error)
            QTest::ignoreMessage(QtWarningMsg, "binary is expecting an additional argument: --boolArgument");
        if (error && usage)
            QTest::ignoreMessage(QtWarningMsg, arguments.usage().toUtf8());
        arguments.componentComplete();

        QCOMPARE(arguments.values()->property("boolArgument").type(), QVariant::Bool);
        QCOMPARE(arguments.values()->property("boolArgument"), QVariant(!error));
        QCOMPARE(arguments.error(), error);
    }

    void testOneRequiredNamedBoolArgument_data() {
        QTest::addColumn<QString>("commandLine");
        QTest::addColumn<bool>("error");
        QTest::addColumn<bool>("usage");
        testCommandLineUsage("", true, true, "NO ARGUMENTS");
        testCommandLineUsage("--boolArgument", false);
        testCommandLineUsage("--boolArgument --boolArgument", false);
        testCommandLineUsage("--boolArgument --otherBool", false);
        testCommandLineUsage("--boolArgument --otherArg1=value --otherArg2 value", false);
        testCommandLineUsage("--boolArgument --otherArg=value defaultValue1 defaultValue2", false);
        testCommandLineUsage("--otherArg=value --boolArgument defaultValue1 defaultValue2", false);
        testCommandLineUsage("--otherArg=value defaultValue1 defaultValue2 --boolArgument", false);
        testCommandLineUsage("--otherArg=value defaultValue1 defaultValue2", true, true);
    }

    void testOneRequiredNamedArgument() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);
        QFETCH(bool, usage);

        setCommandLine(commandLine);

        UCArguments arguments;
        QStringList valueNames;
        valueNames << "VALUE1";

        UCArgument argument1;
        argument1.setName("argument1");
        argument1.setValueNames(valueNames);
        arguments.appendArguments(&argument1);

        if (error)
            QTest::ignoreMessage(QtWarningMsg, "binary is expecting an additional argument: --argument1=VALUE1");
        if (error && usage)
            QTest::ignoreMessage(QtWarningMsg, arguments.usage().toUtf8());
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
        QTest::addColumn<bool>("usage");
        testCommandLineUsage("", true, true, "NO ARGUMENTS");
        testCommandLineUsage("--argument1=value1", false);
        testCommandLineUsage("--argument1 value1", false);
        testCommandLineUsage("--otherArgument --argument1 value1", false);
        testCommandLineUsage("--argument1 value1 --otherArgument", false);
        testCommandLineUsage("--argument1 value1 defaultValue --otherArgument", false);
        testCommandLineUsage("--argument1=value1 defaultValue --otherArgument=value", false);
        testCommandLineUsage("--argument1 value1 defaultValue", false);
    }

    void testTwoRequiredNamedArguments() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);
        QFETCH(bool, usage);

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

        if (error && commandLine == "--argument1=value1 value2 --argument2=value1")
            QTest::ignoreMessage(QtWarningMsg, "binary is expecting a value for argument: --argument2=VALUE1 VALUE2");
        else if (error)
            QTest::ignoreMessage(QtWarningMsg, "binary is expecting an additional argument: --argument1=VALUE1 VALUE2");
        if (error && usage)
            QTest::ignoreMessage(QtWarningMsg, arguments.usage().toUtf8());
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
        QTest::addColumn<bool>("usage");
        testCommandLineUsage("", true, "NO ARGUMENTS");
        testCommandLineUsage("--argument1=value1 value2 --argument2=value1 value2", false);
        testCommandLineUsage("--argument1 value1 value2 --argument2 value1 value2", false);
        testCommandLineUsage("--otherArgument --argument1 value1 value2 --argument2 value1 value2", false);
        testCommandLineUsage("--argument1 value1 value2 --argument2 value1 value2 --otherArgument", false);
        testCommandLineUsage("--argument1 value1 value2 --argument2 value1 value2 defaultValue --otherArgument", false);
        testCommandLineUsage("--argument1 value1 value2 --argument2 value1 value2 defaultValue", false);
        testCommandLineUsage("--argument1=value1 value2 --argument2=value1", true, true);
    }

    void testRequiredDefaultArgument() {
        QFETCH(QString, commandLine);
        QFETCH(bool, error);
        QFETCH(bool, usage);

        setCommandLine(commandLine);

        UCArguments arguments;
        QStringList valueNames;
        valueNames << "DEFAULTVALUE1" << "DEFAULTVALUE2";

        UCArgument defaultArgument;
        defaultArgument.setValueNames(valueNames);
        arguments.setDefaultArgument(&defaultArgument);
        if (error)
            QTest::ignoreMessage(QtWarningMsg, "binary is expecting additional arguments: DEFAULTVALUE1 DEFAULTVALUE2");
        if (error && usage)
            QTest::ignoreMessage(QtWarningMsg, arguments.usage().toUtf8());
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
        QTest::addColumn<bool>("usage");
        testCommandLineUsage("", true, true, "NO ARGUMENTS");
        testCommandLineUsage("--boolArgument", true, true);
        testCommandLineUsage("--boolArgument --otherArg1=value --otherArg2 value", true, true);
        testCommandLineUsage("--boolArgument --otherArg=value defaultValue1 defaultValue2", false, false);
        testCommandLineUsage("--otherArg=value --boolArgument defaultValue1 defaultValue2", false, false);
        testCommandLineUsage("--otherArg=value defaultValue1 defaultValue2 --boolArgument", false, false);
        testCommandLineUsage("--otherArg=value defaultValue1 defaultValue2", false, false);
    }

    void testAPIdefaultArgument() {
        UCArguments arguments;
        arguments.componentComplete();

        // getter
        UCArgument* defaultArgument = arguments.defaultArgument();
        QCOMPARE(defaultArgument->name(), QString());
        QCOMPARE(defaultArgument->help(), QString());
        QCOMPARE(defaultArgument->required(), true);
        QCOMPARE(defaultArgument->valueNames().size(), 0);
        QCOMPARE(defaultArgument->at(0).type(), QVariant::Invalid);

        // setter: custom UCArgument
        UCArgument customDefaultArgument;
        QSignalSpy spy(&arguments, SIGNAL(defaultArgumentChanged()));
        arguments.setDefaultArgument(&customDefaultArgument);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(arguments.defaultArgument(), &customDefaultArgument);

        // setter: NULL
        arguments.setDefaultArgument(NULL);
        QCOMPARE(arguments.defaultArgument(), (UCArgument*)NULL);
    }

    void testAPIprintUsage() {
        UCArguments arguments;
        QString usage(arguments.usage());
        QTest::ignoreMessage(QtWarningMsg, usage.toUtf8());
        arguments.printUsage();
    }

    void testAPIquitWithError() {
        UCArguments arguments;
        QString errorMessage("error message");

        QCOMPARE(arguments.error(), false);
        QCOMPARE(arguments.errorMessage(), QString());

        QTest::ignoreMessage(QtWarningMsg, errorMessage.toUtf8());
        arguments.quitWithError(errorMessage);

        QCOMPARE(arguments.error(), true);
        QCOMPARE(arguments.errorMessage(), errorMessage);
    }

    // unit tests for private API: only done to simplify error tracking
    // do not hesitate to rip off when refactoring UCArgument's implementation
    void testAPIbuildExpectedArguments() {
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

        QList<UCArgument*> declaredArguments;
        QHash<QString, QStringList> result;
        QHash<QString, QStringList> expectedResult;

        result = arguments.buildExpectedArguments(declaredArguments);
        QCOMPARE(result, expectedResult);

        declaredArguments.append(&argument1);
        declaredArguments.append(&argument2);
        result = arguments.buildExpectedArguments(declaredArguments);
        expectedResult.insert("argument1", valueNames);
        expectedResult.insert("argument2", valueNames);
        QCOMPARE(result, expectedResult);
    }

    void testAPIparseRawArguments() {
        UCArguments arguments;

        QStringList values;
        values << "value1" << "value2";

        QStringList rawArguments;
        QHash<QString, QStringList> expectedArguments;
        QHash<QString, QStringList> result;
        QHash<QString, QStringList> expectedResult;

        result = arguments.parseRawArguments(rawArguments, expectedArguments);
        QCOMPARE(result, expectedResult);

        expectedArguments.insert("argument1", values);
        expectedArguments.insert("argument2", values);
        expectedResult.insert("argument1", values);
        expectedResult.insert("argument2", values);

        rawArguments << "binary" << "--argument1" << "value1" << "value2" << "--argument2" << "value1" << "value2";
        result = arguments.parseRawArguments(rawArguments, expectedArguments);
        QCOMPARE(result, expectedResult);

        rawArguments.clear();
        rawArguments << "binary" << "--argument1=value1" << "value2" << "--argument2" << "value1" << "value2";
        result = arguments.parseRawArguments(rawArguments, expectedArguments);
        QCOMPARE(result, expectedResult);
    }

    void testAPIcollectArgumentValues() {
        UCArguments arguments;

        QStringList::const_iterator i;
        QStringList::const_iterator end;
        int nValues = 0;
        QStringList result;
        QStringList expectedResult;

        result = arguments.collectArgumentValues(i, end, nValues);
        QCOMPARE(result, expectedResult);

        QStringList rawArguments;
        rawArguments << "binary" << "--argument1" << "value1" << "value2" << "--argument2" << "value1" << "value2";
        i = rawArguments.constBegin();
        i++; // i points to "--argument1"
        end = rawArguments.constEnd();
        nValues = 2;
        expectedResult << "value1" << "value2";

        result = arguments.collectArgumentValues(i, end, nValues);
        QCOMPARE(result, expectedResult);

        QStringList::const_iterator iExpected = rawArguments.constBegin();
        iExpected++; iExpected++; iExpected++; // iExpected points to "value2"

        QCOMPARE(i, iExpected);
    }

    void testAPIusageRequested() {
        UCArguments arguments;
        QStringList argumentNames;

        QCOMPARE(arguments.usageRequested(argumentNames), false);

        argumentNames.clear();
        argumentNames.append("usage");
        QCOMPARE(arguments.usageRequested(argumentNames), true);

        argumentNames.clear();
        argumentNames.append("h");
        QCOMPARE(arguments.usageRequested(argumentNames), true);

        argumentNames.clear();
        argumentNames.append("help");
        QCOMPARE(arguments.usageRequested(argumentNames), true);

        argumentNames.clear();
        argumentNames.append("anything");
        argumentNames.append("usage");
        QCOMPARE(arguments.usageRequested(argumentNames), true);

        argumentNames.clear();
        argumentNames.append("anything");
        QCOMPARE(arguments.usageRequested(argumentNames), false);
    }
};

// Do not use QTEST_MAIN. QTEST_MAIN is 99.99% of the times what you
// want when running a Qt testcase but it creates a Q*Application and in
// this case that is not wanted since we are creating and destroying them as
// part of the test and Qt doesn't like when there's more than one Q*Application
// at the same time, so do without creating a Q*Application in main.
// This can of course have some implications but it seems that
// for this testcase all is fine.
int main(int argc, char *argv[]) 
{
    int myArgc = argc;
    char **myArgv = new char*[argc + 1];
    // Eat -platform xyz, we're just using QCoreApplications here so
    // no need to say the gui platform to use
    // We are techically leaking the myArgv char** and it's strdup'ed contents
    // but it happens the binary just finishes after, so not a problem
    int j = 0;
    for (int i = 0; i < argc; i++) {
        if (argv[i] == QString("-platform")) {
            if (i < argc) {
                i++; // skip also the platform value
                myArgc -= 2;
            } else {
                myArgc -= 1;
            }
        } else {
            myArgv[j] = strdup(argv[i]);
            j++;
        }
    }
    myArgv[myArgc] = 0;
    tst_UCArguments tc;
    return QTest::qExec(&tc, myArgc, myArgv);
}

#include "tst_arguments.moc"
