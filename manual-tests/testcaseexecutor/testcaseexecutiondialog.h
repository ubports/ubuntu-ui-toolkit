#ifndef TESTCASEEXECUTIONDIALOG_H
#define TESTCASEEXECUTIONDIALOG_H

#include <QDialog>
#include <QtDeclarative>

namespace Ui {
class TestCaseExecutionDialog;
}

class TestCaseExecutionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TestCaseExecutionDialog(QString testcase, QString testdata, QUrl qmlFile, QWidget *parent = 0);
    ~TestCaseExecutionDialog();

protected slots:
    void on_pushButton_clicked();
    
private:
    QDeclarativeView* m_declarativeView;
    Ui::TestCaseExecutionDialog *ui;
    QTemporaryFile tmpQmlFile;
};

#endif // TESTCASEEXECUTIONDIALOG_H
