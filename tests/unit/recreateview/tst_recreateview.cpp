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
 */

#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QtTest/QtTest>

class RecreateViewTest : public QObject
{
    Q_OBJECT

public:
    RecreateViewTest() {}

private Q_SLOTS:
    void initTestCase();
    void testCase_recreateView();

private:
    QString m_modulePath;
};

void RecreateViewTest::initTestCase()
{
    QString modules(UBUNTU_QML_IMPORT_PATH);
    QVERIFY(QDir(modules).exists());

    m_modulePath = QDir(modules).absolutePath();
}

void RecreateViewTest::testCase_recreateView()
{
    QQuickView *view = new QQuickView;
    view->engine()->addImportPath(m_modulePath);

    view->setSource(QUrl::fromLocalFile("SimpleApp.qml"));
    QVERIFY(view->rootObject() != 0);
    view->show();
    bool window_shown = QTest::qWaitForWindowExposed(view);
    Q_ASSERT(window_shown);
    delete view;

    view = new QQuickView;
    view->engine()->addImportPath(m_modulePath);

    view->setSource(QUrl::fromLocalFile("SimpleApp.qml"));
    QVERIFY(view->rootObject() != 0);
    view->show();
    window_shown = QTest::qWaitForWindowExposed(view);
    Q_ASSERT(window_shown);
    delete view;
}

QTEST_MAIN(RecreateViewTest)

#include "tst_recreateview.moc"
