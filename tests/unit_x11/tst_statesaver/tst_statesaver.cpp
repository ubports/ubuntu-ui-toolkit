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
#include "quickutils.h"

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
        QCoreApplication::setApplicationName("tst_statesaver");
        QCoreApplication::setOrganizationName("");
        QDir modules ("../../../modules");
        QVERIFY(modules.exists());
        m_modulePath = modules.absolutePath();
        // invoke initialization
        StateSaverBackend::instance();
    }

    void cleanupTestCase()
    {
        StateSaverBackend::instance().reset();
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

    void test_SavePropertyGroup()
    {
        QQuickView *view = createView("SavePropertyGroups.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);
        QVERIFY(testItem->setProperty("color", QColor("red")));
        // set property groups using QQmlProperty
        QQmlProperty borderColor(testItem, "border.color");
        borderColor.write(QColor("green"));
        QQmlProperty propertyGroup(testItem, "propertyGroup.color");
        propertyGroup.write(QColor("blue"));
        delete view;

        view = createView("SavePropertyGroups.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
        QCOMPARE(testItem->property("color"), QVariant(QColor("red")));
        // read property groups using QQmlProperty
        QQmlProperty verifyBorderColor(testItem, "border.color");
        QCOMPARE(verifyBorderColor.read(), QVariant(QColor("green")));
        QQmlProperty verifyPropertyGroup(testItem, "propertyGroup.color");
        QCOMPARE(verifyPropertyGroup.read(), QVariant(QColor("blue")));
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
        testItem->setProperty("object", QVariant::fromValue(obj));
        delete view;

        view = createView("SaveObject.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
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
        QSignalSpy *spy;
        QQuickView *view = createView("InvalidUID.qml", &spy);
        QVERIFY(view);
        QCOMPARE(spy->count(), 1);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("updated");
        delete view;

        view = createView("InvalidUID.qml", &spy);
        QVERIFY(view);
        QCOMPARE(spy->count(), 1);
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
        QSignalSpy *spy;
        QQuickView *view = createView("InvalidGroupProperty.qml", &spy);
        QVERIFY(view);
        QCOMPARE(spy->count(), 1);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setObjectName("group");
        delete view;

        view = createView("InvalidGroupProperty.qml", &spy);
        QVERIFY(view);
        QCOMPARE(spy->count(), 1);
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

    void test_SameIdsInDifferentComponents()
    {
        QQuickView *view = createView("SameIdsInDifferentComponents.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);

        testItem->setProperty("source", "SecondComponent.qml");
        // check if the SecondComponent state saver is enabled
        QObject *item = testItem->property("item").value<QObject*>();
        QVERIFY(item);
        QObject *innerItem = item->findChild<QObject*>("secondComponent");
        UCStateSaverAttached *stateSaver = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(innerItem, false));
        QVERIFY(stateSaver);
        QVERIFY(stateSaver->enabled());
        delete view;

        view = createView("SameIdsInDifferentComponents.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("secondComponent");
        QVERIFY(testItem);
        delete view;
    }

    void test_ComponentsWithStateSavers()
    {
        QQuickView *view = createView("ComponentsWithStateSavers.qml");
        QVERIFY(view);
        QObject *control1 = view->rootObject()->findChild<QObject*>("control1");
        QVERIFY(control1);
        QObject *control2 = view->rootObject()->findChild<QObject*>("control2");
        QVERIFY(control2);
        UCStateSaverAttached *stateSaver1 = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(control1, false));
        QVERIFY(stateSaver1);
        QVERIFY(stateSaver1->enabled());
        UCStateSaverAttached *stateSaver2 = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(control2, false));
        QVERIFY(stateSaver2);
        QVERIFY(stateSaver2->enabled());

        QVERIFY(control1->setProperty("color", QColor("green")));
        QVERIFY(control2->setProperty("color", QColor("blue")));
        delete view;

        view = createView("ComponentsWithStateSavers.qml");
        control1 = view->rootObject()->findChild<QObject*>("control1");
        QVERIFY(control1);
        control2 = view->rootObject()->findChild<QObject*>("control2");
        QVERIFY(control2);
        stateSaver1 = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(control1, false));
        QVERIFY(stateSaver1);
        QVERIFY(stateSaver1->enabled());
        stateSaver2 = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(control2, false));
        QVERIFY(stateSaver2);
        QVERIFY(stateSaver2->enabled());
        QVERIFY(view);

        QCOMPARE(control1->property("color"), QVariant(QColor("green")));
        QCOMPARE(control2->property("color"), QVariant(QColor("blue")));
        delete view;
    }

    void test_ComponentsWithStateSaversNoId()
    {
        QSignalSpy *spy;
        QQuickView *view = createView("ComponentsWithStateSaversNoId.qml", &spy);
        QVERIFY(view);
        QObject *control1 = view->rootObject()->findChild<QObject*>("control1");
        QVERIFY(control1);
        QObject *control2 = view->rootObject()->findChild<QObject*>("control2");
        QVERIFY(control2);
        UCStateSaverAttached *stateSaver1 = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(control1, false));
        QVERIFY(stateSaver1);
        QVERIFY(stateSaver1->enabled() == false);
        QCOMPARE(spy->count(), 1);
        UCStateSaverAttached *stateSaver2 = qobject_cast<UCStateSaverAttached*>(qmlAttachedPropertiesObject<UCStateSaver>(control2, false));
        QVERIFY(stateSaver2);
        QVERIFY(stateSaver2->enabled());
        delete view;
    }

    void test_nestedDynamics()
    {
        QQuickView *view = createView("NestedDynamics.qml");
        QVERIFY(view);
        QObject *topLoader = view->rootObject()->findChild<QObject*>("outerLoader");
        QVERIFY(topLoader);

        topLoader->setProperty("source", "Dynamic.qml");
        QTest::waitForEvents();

        QObject *testItem = view->rootObject()->findChild<QObject*>("testItem");
        QVERIFY(testItem);
        testItem->setObjectName("updated");
        delete view;

        view = createView("NestedDynamics.qml");
        QVERIFY(view);
        testItem = view->rootObject()->findChild<QObject*>("updated");
        QVERIFY(testItem);
        delete view;
    }

    void test_repeaterStates()
    {
        QScopedPointer<QQuickView> view(createView("RepeaterStates.qml"));
        QVERIFY(view);
        QQuickItem *column = view->rootObject()->findChild<QQuickItem*>("column");
        QVERIFY(column);

        QList<QQuickItem*> items = column->childItems();
        QCOMPARE(items.count(), 5); // 4 Rectangles + 1 Repeater

        Q_FOREACH(QQuickItem *item, items) {
            if (QuickUtils::instance().className(item) == "QQuickRectangle") {
                item->setHeight(25);
            }
        }
        view.reset();

        view.reset(createView("RepeaterStates.qml"));
        QVERIFY(view);
        column = view->rootObject()->findChild<QQuickItem*>("column");
        QVERIFY(column);

        items = column->childItems();
        QCOMPARE(items.count(), 5); // 4 Rectangles + 1 Repeater

        Q_FOREACH(QQuickItem *item, items) {
            if (QuickUtils::instance().className(item) == "QQuickRectangle") {
                QCOMPARE(item->height(), 25.0);
            }
        }
    }
};

QTEST_MAIN(tst_StateSaverTest)

#include "tst_statesaver.moc"



