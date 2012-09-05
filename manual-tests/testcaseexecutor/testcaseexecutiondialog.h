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
    void on_pushButtonQuit_clicked();
    
private:
    QDeclarativeView* m_declarativeView;
    Ui::TestCaseExecutionDialog *m_ui;
};

#endif // TESTCASEEXECUTIONDIALOG_H
