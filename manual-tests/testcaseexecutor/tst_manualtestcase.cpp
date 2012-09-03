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

#include "tst_manualtestcase.h"
#include "testcaseexecutiondialog.h"

#include <QDebug>
#include <QtTest>
#include <QDialogButtonBox>

tst_manualtestcase::tst_manualtestcase(QObject *parent) : QObject(parent) {

}

void tst_manualtestcase::run() {
    foreach (QString testcase, m_testCases) {
        qDebug() << "Starting" << testcase;
        qDebug() << "Loading data to QDeclarativeView";
        QString testfile = testcase.split("_").takeAt(1) + ".qml";
        QFile test(testcase);
        if (test.open(QIODevice::ReadOnly)) {
            QString testdata = test.readAll();
            test.close();
            runTest(testcase,testdata,testfile);
        }
    }
}

void tst_manualtestcase::list() {
    QDir currentDirectory = QDir::currentPath();
    m_testCases = currentDirectory.entryList(QStringList() << "tst*.txt");
    qDebug() << m_testCases;
}

void tst_manualtestcase::runTest(QString testcase, QString testdata, QString qmlFile) {
    TestCaseExecutionDialog testcaseExecutor(testcase,testdata,QUrl::fromLocalFile(qmlFile));

    int result = testcaseExecutor.exec();
    QVERIFY((result==1));
}

QTEST_MAIN(tst_manualtestcase)

