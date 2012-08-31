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

