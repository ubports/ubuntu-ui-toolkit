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
 *
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlProperty>

class tst_OrientationTest : public QObject
{
    Q_OBJECT
public:
    tst_OrientationTest() {}

private:
    QString m_modulePath;

    QQuickView *createView(const QString &file, QSignalSpy **spy = 0)
    {
        QQuickView *view = new QQuickView(0);
        if (spy) {
            *spy = new QSignalSpy(view->engine(), SIGNAL(warnings(QList<QQmlError>)));
            (*spy)->setParent(view);
        }
        view->engine()->addImportPath(m_modulePath);
        view->setSource(QUrl::fromLocalFile(file));
        if (!view->rootObject()) {
            return 0;
        }
        view->show();
        QTest::qWaitForWindowExposed(view);
        return view;
    }

private Q_SLOTS:

    void initTestCase()
    {
        QDir modules ("../../../modules");
        QVERIFY(modules.exists());
        m_modulePath = modules.absolutePath();
    }

    void cleanupTestCase()
    {
    }

    void test_defaults()
    {
        QSignalSpy *spy;
        QQuickView *view = createView("Defaults.qml", &spy);
        QVERIFY(view);
        QQuickItem *helper = view->rootObject()->findChild<QQuickItem*>("helper");
        QVERIFY(helper);
        QCOMPARE(helper->property("automaticOrientation").toBool(), true);
        // No warnings expected
        QCOMPARE(spy->count(), 0);
        delete view;
    }

    void test_manualAngle()
    {
        QSignalSpy *spy;
        QQuickView *view = createView("ManualAngle.qml", &spy);
        QVERIFY(view);
        QQuickItem *helper = view->rootObject()->findChild<QQuickItem*>("helper");
        QVERIFY(helper);
        // No warning about "window" being undefined must appear
        QCOMPARE(spy->count(), 0);
        QCOMPARE(helper->property("orientationAngle").toInt(), 90);
        // Verify expected values
        QQuickItem *checkpoint = view->rootObject()->findChild<QQuickItem*>("checkpoint");
        QVERIFY(checkpoint);
        // window.contentOrientation 
        QCOMPARE(checkpoint->property("contentOrientation").toInt(), helper->property("orientationAngle").toInt());
        delete view;
    }
};

QTEST_MAIN(tst_OrientationTest)

#include "tst_orientation.moc"



