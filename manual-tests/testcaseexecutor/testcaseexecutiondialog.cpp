#include "testcaseexecutiondialog.h"
#include "ui_testcaseexecutiondialog.h"

#include <QGLWidget>


TestCaseExecutionDialog::TestCaseExecutionDialog(QString testcase,
                                                 QString testdata,
                                                 QUrl qmlFile,
                                                 QWidget *parent) : QDialog(parent),
                                                                    m_declarativeView(new QDeclarativeView),
                                                                    m_ui(new Ui::TestCaseExecutionDialog) {
    m_ui->setupUi(this);
    m_ui->plainTextEdit->setPlainText(testdata);
    this->setWindowTitle(testcase);
    m_ui->horizontalLayout->addWidget(m_declarativeView);

    QFile qmldata(qmlFile.toLocalFile());
    QString data;
    if (qmldata.open(QIODevice::ReadOnly)) {
        data = qmldata.readAll();
        qmldata.close();
    }

    m_ui->plainTextEdit_2->setPlainText(data);

    QTemporaryFile tmpQmlFile;
    tmpQmlFile.open();
    tmpQmlFile.write(data.toAscii());
    tmpQmlFile.close();
    qDebug() << "using temporary file" << tmpQmlFile.fileName();

    m_declarativeView->engine()->addImportPath("../modules/");
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);
    format.setSwapInterval(1);

    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAttribute(Qt::WA_TranslucentBackground, false);
    m_declarativeView->setAttribute(Qt::WA_TranslucentBackground, false);
    m_declarativeView->setAttribute(Qt::WA_NoSystemBackground, true);
    m_declarativeView->setAttribute(Qt::WA_OpaquePaintEvent, true);
    m_declarativeView->setViewport(glWidget);
    m_declarativeView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_declarativeView->setSource(QUrl::fromLocalFile(tmpQmlFile.fileName()));
    m_declarativeView->show();
    m_declarativeView->viewport()->setFocusPolicy(Qt::StrongFocus);
}

void TestCaseExecutionDialog::on_pushButton_clicked() {
    QTemporaryFile tmpQmlFile;
    tmpQmlFile.open();
    tmpQmlFile.write(m_ui->plainTextEdit_2->toPlainText().toAscii());
    tmpQmlFile.close();
    m_declarativeView->engine()->clearComponentCache();
    m_declarativeView->setSource(QUrl::fromLocalFile(tmpQmlFile.fileName()));
}

TestCaseExecutionDialog::~TestCaseExecutionDialog() {
    delete m_ui;
}


void TestCaseExecutionDialog::on_pushButtonQuit_clicked() {
    QApplication::quit();
}
