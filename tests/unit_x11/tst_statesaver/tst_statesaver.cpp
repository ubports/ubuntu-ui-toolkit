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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QtQuick/QQuickItem>

#define protected public
#include "ucstatesaver.h"
#include "statesaverbackend_p.h"
#undef protected

class tst_StateSaverTest : public QObject
{
    Q_OBJECT
public:
    tst_StateSaverTest() {}

private:
    QString m_modulePath;

    QQuickView *createView(const QString &file)
    {
        QQuickView *view = new QQuickView(0);
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
        QCoreApplication::setApplicationName("tst_statesaver");
        QCoreApplication::setOrganizationName("");
        QDir modules ("../../../modules");
        QVERIFY(modules.exists());
        m_modulePath = modules.absolutePath();
        // invoke initialization
        StateSaverBackend::instance();
        StateSaverBackend::instance().reset();
    }

    void cleanupTestCase()
    {
    }

    void test_SaveArrays()
    {
        QQuickView *view = createView("SaveArrays.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);

        QVariantList boolValues;
        boolValues << true << true;
        QVariantList intValues;
        intValues << 100 << 200;
        QVariantList realValues;
        realValues << 123.45 << 678.9;
        QVariantList stringValues;
        stringValues << "one" << "two";
        testItem->setProperty("boolArray", boolValues);
        testItem->setProperty("intArray", intValues);
        testItem->setProperty("realArray", realValues);
        testItem->setProperty("stringArray", stringValues);
        delete view;

        view = createView("SaveArrays.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
        QVERIFY(testItem->property("boolArray") == boolValues);
        QVERIFY(testItem->property("intArray") == intValues);
        QVERIFY(testItem->property("realArray") == realValues);
        QVERIFY(testItem->property("stringArray") == stringValues);
        delete view;
    }

    void test_SaveStructures()
    {
        QQuickView *view = createView("SaveSupportedTypes.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);

        QVariantHash values;
        values.insert("intValue", 1000);
        values.insert("boolValue", true);
        values.insert("realValue", 345.12);
        values.insert("doubleValue", 65535.33244);
        values.insert("string", "test string");
        values.insert("url", QUrl::fromLocalFile("tst_statesaver.cpp"));
        values.insert("date", QDateTime::currentDateTime().addDays(5));
        values.insert("point", QPoint(100, 100));
        values.insert("rect", QRect(100, 100, 200, 200));
        values.insert("size", QSize(345, 678));
        values.insert("color", QColor("blue"));
        values.insert("font", QFont("Ubuntu"));
        values.insert("vector2d", QVector2D(100.0, 200.0));
        values.insert("vector3d", QVector3D(100.0, 200.0, 300.0));
        values.insert("vector4d", QVector4D(100.0, 200.0, 300.0, 400.0));
        values.insert("quaternion", QQuaternion(1, 100.0, 200.0, 300.0));
        values.insert("matrix4x4", QMatrix4x4(5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20));

        QHashIterator<QString, QVariant> i(values);
        while (i.hasNext()) {
            i.next();
            QVERIFY2(testItem->setProperty(i.key().toLocal8Bit().constData(), i.value()),
                                           QString("set %1").arg(i.key()).toLocal8Bit().constData());
        }

        delete view;

        view = createView("SaveSupportedTypes.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);

        UCStateSaverAttached *stateSaver = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(testItem, false));
        QVERIFY(stateSaver);

        QStringList properties = stateSaver->properties().split(',');
        Q_FOREACH(const QString &property, properties) {
            QVERIFY2(testItem->property(property.toLocal8Bit().constData()) == values.value(property), QString("verifying %1").arg(property).toLocal8Bit().constData());
        }

        delete view;
    }

    void test_SaveObject()
    {
        QQuickView *view = createView("SaveObject.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);

        QQuickItem *obj = new QQuickItem;
        obj->setObjectName("internal");
        qDebug() << "set" << obj;
        testItem->setProperty("object", QVariant::fromValue(obj));
        delete view;

        view = createView("SaveObject.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
        qDebug() << testItem->property("object");
        QVERIFY(testItem->property("object").value<QQuickItem*>() != obj);
        delete view;
        delete obj;
    }

    void test_ValidUID()
    {
        QQuickView *view = createView("ValidUID.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("updated");
        delete view;

        view = createView("ValidUID.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("updated");
        QVERIFY(testItem);
        delete view;
    }

    void test_InvalidUID()
    {
        QQuickView *view = createView("InvalidUID.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("updated");
        delete view;

        view = createView("InvalidUID.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("updated");
        QVERIFY(testItem == 0);
        delete view;
    }

    void test_ValidGroupProperty()
    {
        QQuickView *view = createView("ValidGroupProperty.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("group");
        delete view;

        view = createView("ValidGroupProperty.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("group");
        QVERIFY(testItem);
        delete view;
    }

    void test_InvalidGroupProperty()
    {
        QQuickView *view = createView("InvalidGroupProperty.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("group");
        delete view;

        view = createView("InvalidGroupProperty.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("group");
        QVERIFY(testItem == 0);
        delete view;
    }

    void test_Dynamic()
    {
        QQuickView *view = createView("Dynamic.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("updated");
        delete view;

        view = createView("Dynamic.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("updated");
        QVERIFY(testItem);
        delete view;
    }

    void test_TwoDynamics()
    {
        QQuickView *view = createView("TwoDynamics.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("updated");
        delete view;

        view = createView("TwoDynamics.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("updated");
        QVERIFY(testItem);
        delete view;
    }

    void test_DisabledStateSaver()
    {
        QQuickView *view = createView("DisabledStateSaver.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("updated");
        delete view;

        view = createView("DisabledStateSaver.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("updated");
        QVERIFY(testItem == 0);
        delete view;
    }

};

QTEST_MAIN(tst_StateSaverTest)

#include "tst_statesaver.moc"



