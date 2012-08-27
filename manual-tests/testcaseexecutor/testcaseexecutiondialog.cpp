#include "testcaseexecutiondialog.h"
#include "ui_testcaseexecutiondialog.h"

TestCaseExecutionDialog::TestCaseExecutionDialog(QString testcase, QString testdata, QUrl qmlFile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestCaseExecutionDialog),
    m_declarativeView(new QDeclarativeView)
{
    ui->setupUi(this);
    ui->plainTextEdit->setPlainText(testdata);
    this->setWindowTitle(testcase);
    ui->horizontalLayout->addWidget(m_declarativeView);

    QFile qmldata(qmlFile.toLocalFile());
    QString data;
    if (qmldata.open(QIODevice::ReadOnly)) {
        data = qmldata.readAll();
        qmldata.close();
    }

    ui->plainTextEdit_2->setPlainText(data);
    tmpQmlFile.open();
    tmpQmlFile.write(data.toAscii());
    tmpQmlFile.close();

    m_declarativeView->engine()->addImportPath("..");
    m_declarativeView->setSource(QUrl::fromLocalFile(tmpQmlFile.fileName()));
    m_declarativeView->show();
    m_declarativeView->viewport()->setFocusPolicy(Qt::StrongFocus);
}

void TestCaseExecutionDialog::on_pushButton_clicked() {
    tmpQmlFile.open();
    tmpQmlFile.write(ui->plainTextEdit_2->toPlainText().toAscii());
    tmpQmlFile.close();
    m_declarativeView->engine()->clearComponentCache();
    m_declarativeView->setSource(QUrl::fromLocalFile(tmpQmlFile.fileName()));
}

TestCaseExecutionDialog::~TestCaseExecutionDialog()
{
    delete ui;
}
