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
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include "inversemouseareatype.h"

class tst_InverseMouseAreaTest : public QObject
{
    Q_OBJECT
    
public:
    tst_InverseMouseAreaTest() {}

private:
    QQuickView *quickView;
    QQmlEngine *quickEngine;
    QObjectCleanupHandler eventCleanup;

    InverseMouseAreaType *testArea(const QString &document)
    {
        // delete previous root
        QObject *rootObject = quickView->rootObject();
        if (rootObject)
            delete rootObject;
        QTest::waitForEvents();

        quickView->setSource(QUrl::fromLocalFile(document));
        QCoreApplication::processEvents();

        rootObject = quickView->rootObject();
        if (!rootObject)
            return 0;

        QList<QQuickItem*> items = rootObject->findChildren<QQuickItem*>();
        Q_FOREACH(QQuickItem *item, items) {
            InverseMouseAreaType *area = qobject_cast<InverseMouseAreaType*>(item);
            if (area)
                return area;
        }
        return 0;
    }

protected Q_SLOTS:
    void capturePressed(QQuickMouseEvent *event)
    {
        eventCleanup.add((QObject*)event);
    }

private Q_SLOTS:

    void initTestCase()
    {
        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        quickView = new QQuickView(0);
        quickEngine = quickView->engine();

        quickView->setGeometry(0,0, 240, 320);
        //add modules folder so we have access to the plugin from QML
        QStringList imports = quickEngine->importPathList();
        imports << QDir(modules).absolutePath();
        quickEngine->setImportPathList(imports);
    }

    void cleanupTestCase()
    {
        delete quickView;
    }

    void testCase_DoNotPropagateEvents()
    {
        eventCleanup.clear();
        InverseMouseAreaType *area = testArea("InverseMouseAreaDoNotPropagateEvents.qml");
        QVERIFY(area);
        // connect pressed signal to capture mouse object
        QObject::connect(area, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
        QTest::waitForEvents();
        QVERIFY(eventCleanup.isEmpty());
    }


    void testCase_PropagateEvents()
    {
        eventCleanup.clear();
        InverseMouseAreaType *area = testArea("InverseMouseAreaPropagateEvents.qml");
        QVERIFY(area);
        // connect pressed signal to capture mouse object
        QObject::connect(area, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
        QTest::waitForEvents();
        QVERIFY(eventCleanup.isEmpty());
    }

    void testCase_sensingAreaError()
    {
        InverseMouseAreaType *area = testArea("SensingAreaError.qml");
        QVERIFY(area);

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(20, 20));
        QTest::waitForEvents();
        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("IMA"));
    }

    void testCase_InverseMouseAreInWindow()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaInWindow.qml");
        QVERIFY(area);
        quickView->show();

        QList<QQuickWindow *> l = quickView->rootObject()->findChildren<QQuickWindow*>("isawindow");
        QVERIFY(l.count());

        QTest::mouseClick(l[0], Qt::LeftButton, 0, QPoint(20, 10));
        QTest::waitForEvents();
        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("IMA"));
    }

    void testCase_OverlapperMouseArea()
    {
        InverseMouseAreaType *area = testArea("OverlappedMouseArea.qml");
        QVERIFY(area);
        quickView->show();

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(20, 10));
        QTest::waitForEvents();
        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("MA"));
    }

};

QTEST_MAIN(tst_InverseMouseAreaTest)

#include "tst_inversemouseareatest.moc"
