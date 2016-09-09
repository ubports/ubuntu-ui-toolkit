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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>
#include <QtCore/QProcess>
#include <QtXml/QDomDocument>

QStringList _testReports;
int _testcaseCounter;
int _testcasePassed;
int _testcaseFailed;
int _testcaseSkipped;
int _testcaseXPass;
int _testcaseXFail;


void printResults() {

    int failedTests = 0;
    int passTests = 0;

    QStringList failedTestNames;

    foreach (QString fileName, _testReports) {
        QString testXmlData = "";
        QString testName = QFileInfo(fileName).fileName();;
        QFile testXml(fileName);

        qDebug() << "\nResults @" << fileName.toLatin1().constData();
        qDebug();

        int testcaseCounter = 0;
        int testcaseSkipped = 0;
        int testcasePassed = 0;
        int testcaseFailed = 0;
        int testcaseXFail = 0;
        int testcaseXPass = 0;

        if (testXml.open(QIODevice::ReadOnly)) {
            testXmlData = testXml.readAll();
            testXml.close();
        } else {
            qWarning() << "Unable to open" << fileName << "for reading. Skipped.";
            continue;
        }

        if (testXmlData.trimmed().length()==0) {
            QFile::remove(fileName);
            qWarning() << "The file was empty, removing it.";
            continue;
        }


            QDomDocument xmlDocument;
            QString errorMsg; int errorLine;
            if (!xmlDocument.setContent(testXmlData,&errorMsg,&errorLine)) {
                qDebug() << "\n";
                qWarning() << "\t" << "\033[0;41m error \033[0;0m" << errorMsg.toLatin1().constData() << " @ line:" << errorLine;
                continue;
            }

            QDomNodeList xmlTestCases = xmlDocument.elementsByTagName("testcase");
            for (int idx = 0; idx < xmlTestCases.length(); idx++) {
                testcaseCounter++;

                QDomNode xmlTestCase = xmlTestCases.at(idx);
                QDomNamedNodeMap attributes = xmlTestCase.attributes();

                QDomNode nameNode = attributes.namedItem("name");
                if (nameNode.isNull()) { qWarning() << "Could not find name node"; continue; }

                QDomNode resultNode = attributes.namedItem("result");
                QString result;

                if (resultNode.isNull()) {
                    result = "SKIP";
                    testcaseSkipped++;
                } else {
                    result = resultNode.nodeValue().toLatin1().constData();
                    if (result=="pass") { testcasePassed++; }
                    else if (result=="fail") { testcaseFailed++; }
                    else if (result=="xpass") { testcaseXPass++; }
                    else if (result=="xfail") { testcaseXFail++; }
                }

                qDebug() << "\t" << (result=="pass" ? "\033[0;32m[PASS]\033[0;0m" : (result=="SKIP") ? "\033[0;33m[SKIP]\033[0;0m" : (result=="xpass") ? "\033[2;32m[XPASS]\033[0;0m" : (result=="xfail") ? "\033[2;31m[XFAIL]\033[0;0m" : "\033[0;31m[FAILED]\033[0;0m") << nameNode.nodeValue().toLatin1().constData();
            }


        _testcaseCounter += testcaseCounter;
        _testcasePassed += testcasePassed;
        _testcaseFailed += testcaseFailed;
        _testcaseSkipped += testcaseSkipped;
        _testcaseXFail += testcaseXFail;
        _testcaseXPass += testcaseXPass;

        qDebug();
        qDebug() << "\t" << "Total" << testcaseCounter << " - \033[0;32m[PASS]\033[0;0m" << testcasePassed << "\033[2;31m[XFAIL]\033[0;0m" << testcaseXFail << "\033[2;32m[XPASS]\033[0;0m" << testcaseXPass << "\033[0;31m[FAILED]\033[0;0m" << testcaseFailed << "\033[0;33m[SKIP]\033[0;0m" << testcaseSkipped << "\n";

        if (testcaseFailed>0) { // FAILED
            failedTests++;
            failedTestNames << testName.toLatin1().constData();
            qDebug() << "\033[0;41m FAILED \033[0;0m" << testName.toLatin1().constData();
        } else {
            passTests++;
            qDebug() << "\033[0;42m PASSED \033[0;0m" << testName.toLatin1().constData();
        }
        qDebug() << "-------------------------------------------------------";
    }

    qDebug() << "\n\n";
    qDebug() << "TEST SUITES\t" << QString("%0").arg(passTests+failedTests).toLatin1().constData();
    qDebug() << "\033[0;42m PASSED \033[0;0m" << "\t"  << passTests;
    qDebug() << "\033[0;41m FAILED \033[0;0m" << "\t" << failedTests;

    qDebug() << "\n";
    qDebug() << "TEST CASES\t" << QString("%0").arg(_testcaseCounter).toLatin1().constData();
    qDebug() << "\033[0;32m[PASS]\033[0;0m\t\t" << _testcasePassed;
    qDebug() << "\033[2;31m[XFAIL]\033[0;0m\t\t" << _testcaseXFail;
    qDebug() << "\033[2;32m[XPASS]\033[0;0m\t\t" << _testcaseXPass;
    qDebug() << "\033[0;31m[FAILED]\033[0;0m\t" <<  _testcaseFailed;
    qDebug() << "\033[0;33m[SKIP]\033[0;0m\t\t" << _testcaseSkipped;
    qDebug() << "\n";
    if (failedTests>0) {
        qDebug() << "\tFailed tests:";
        foreach (QString failedTest, failedTestNames) {
            qDebug() << "\t" << failedTest.toLatin1().constData();
        }
        qDebug() << "\n";
    }
}

void findTestResults(QString path) {
    QDir dir(path);
    QFileInfoList files = dir.entryInfoList();
    foreach (const QFileInfo &file, files) {
        if (file.fileName()=="." || file.fileName()=="..") continue;
        if (file.isDir()) continue;

        if (file.completeSuffix()=="xml") {
            _testReports << file.absoluteFilePath();
        }

    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString searchPath = ".";
    int returnValue = 0;

    if (a.arguments().count()==2) {
        QString arg = a.arguments().takeAt(1).trimmed();
        searchPath = arg;
    }

    if (!QFileInfo(searchPath).exists()) {
        qWarning() << "\n\nInvalid argument, file or folder does not exist:\n" << searchPath << "\n\n";
        return -1;
    }

    if (QFileInfo(searchPath).isDir()) {
        findTestResults(searchPath);
    } else {
        _testReports << searchPath;
    }

    printResults();

    return returnValue;
}
