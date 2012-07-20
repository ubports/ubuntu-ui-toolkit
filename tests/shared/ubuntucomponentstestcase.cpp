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
 */
#include "ubuntucomponentstestcase.h"

void UbuntuComponentsTestCase::checkPropertyExistsAndExpectedType(const QString& name, const QVariant::Type& expectedType) {
    int propertyIndex = m_pComponentObject->metaObject()->indexOfProperty(name.toLatin1().data());
    QVERIFY(propertyIndex>-1);
    QMetaProperty property = m_pComponentObject->metaObject()->property(propertyIndex);
    QVERIFY(property.isValid());
    QVERIFY(property.type()==expectedType);
}


void UbuntuComponentsTestCase::loadAndCreateComponentObject() {
    QDeclarativeView* tmpView = new QDeclarativeView;
    tmpView->engine()->addImportPath("../../../");
    QDeclarativeComponent tmpComponent(tmpView->engine());
    QVERIFY(QFile(m_sComponentFileName).exists());
    QFile componentFile(m_sComponentFileName);
    QVERIFY(componentFile.open(QIODevice::ReadOnly));
    tmpComponent.setData(componentFile.readAll(),QUrl());
    m_pComponentObject = tmpComponent.create();
    QObject::connect(qApp,SIGNAL(aboutToQuit()),tmpView,SLOT(deleteLater()));
    QVERIFY(m_pComponentObject);
}

void UbuntuComponentsTestCase::checkPropertyValue(const QString& name, const QVariant& expectedValue) {
    QCOMPARE(QTest::toString(componentObject()->property(name.toAscii().data())), QTest::toString(expectedValue));
}

void UbuntuComponentsTestCase::setPropertyValue(const QString& name, const QVariant& value) {
    componentObject()->setProperty(name.toAscii().data(),value);
}

void UbuntuComponentsTestCase::checkSimpleSignal(const char* signalName, const char* member) {
    QSignalSpy signalSpy(componentObject(), signalName);
    QMetaObject::invokeMethod(componentObject(), member,Qt::DirectConnection);
    QCOMPARE(signalSpy.count(), 1);
    QVERIFY(signalSpy.isValid());
}
