#ifndef TST_MANUALTESTCASE_H
#define TST_MANUALTESTCASE_H

#include <QObject>
#include <QStringList>
#include <QDebug>

class tst_manualtestcase : public QObject
{
    Q_OBJECT

public:
    explicit tst_manualtestcase(QObject *parent = 0);

private slots:
    void list();
    void run();

protected:
    void runTest(QString testcase, QString testdata, QString qmlFile);
    QStringList m_testCases;
};

#endif // TST_MANUALTESTCASE_H
