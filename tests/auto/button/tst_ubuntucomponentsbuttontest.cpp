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

#include <QtTest>
#include <QtDeclarative>
#include "../../shared/ubuntucomponentstestcase.h"

class tst_ubuntucomponentsbuttontest : public UbuntuComponentsTestCase
{
    Q_OBJECT

public:
    tst_ubuntucomponentsbuttontest() {
        setComponentFileName("../button/tst_ubuntucomponentsbuttontest.qml");
    }
    
private slots:
    void initTestCase();

    void textColor();
    void textSize();
    void text();
    void iconPosition();
    void enabled();
    void iconSource();
    void clicked();

    void cleanupTestCase();
};

void tst_ubuntucomponentsbuttontest::initTestCase() {
    int retval = loadAndCreateComponentObject();
    QVERIFY(retval==NO_ERRORS);
}

void tst_ubuntucomponentsbuttontest::cleanupTestCase() {
    // empty
}

void tst_ubuntucomponentsbuttontest::text() {
    QVERIFY(checkPropertyAndExpectedType("text",QVariant::String));

    QCOMPARE(componentObject()->property("text").toString(), QString("Hello World"));
    componentObject()->setProperty("text","World Hello!");
    QCOMPARE(componentObject()->property("text").toString(), QString("World Hello!"));
}

void tst_ubuntucomponentsbuttontest::iconSource() {
    QVERIFY(checkPropertyAndExpectedType("iconSource",QVariant::Url));

    QCOMPARE(QFileInfo(componentObject()->property("iconSource").toString()).fileName(), QString("icon.png"));
    componentObject()->setProperty("iconSource","icon2.png");
    QCOMPARE(componentObject()->property("iconSource").toString(), QString("icon2.png"));
}

void tst_ubuntucomponentsbuttontest::enabled() {
    QVERIFY(checkPropertyAndExpectedType("enabled",QVariant::Bool));

    QCOMPARE(componentObject()->property("enabled").toBool(), false);
    componentObject()->setProperty("enabled",true);
    QCOMPARE(componentObject()->property("enabled").toBool(), true);
}

void tst_ubuntucomponentsbuttontest::textColor() {
    QVERIFY(checkPropertyAndExpectedType("textColor",QVariant::Color));

    QCOMPARE(componentObject()->property("textColor").toString(), QString("#ff0000"));
    componentObject()->setProperty("textColor",QString("#0000ff"));
    QCOMPARE(componentObject()->property("textColor").toString(),QString("#0000ff"));
}

void tst_ubuntucomponentsbuttontest::textSize() {
    QVERIFY(checkPropertyAndExpectedType("textSize",QVariant::String));

    QCOMPARE(componentObject()->property("textSize").toString(), QString("x-large"));
    componentObject()->setProperty("textSize","medium");
    QCOMPARE(componentObject()->property("textSize").toString(), QString("medium"));
}

void tst_ubuntucomponentsbuttontest::iconPosition() {
    QVERIFY(checkPropertyAndExpectedType("iconPosition",QVariant::String));

    QCOMPARE(componentObject()->property("iconPosition").toString(),  QString("right"));
    componentObject()->setProperty("iconPosition",QString("left"));
    QCOMPARE(componentObject()->property("iconPosition").toString(), QString("left"));
}

void tst_ubuntucomponentsbuttontest::clicked() {
    QSignalSpy signalSpy(componentObject(), SIGNAL(clicked()));
    QMetaObject::invokeMethod(componentObject() ,"clicked",Qt::DirectConnection);
    QCOMPARE(signalSpy.count(), 1);
    QVERIFY(signalSpy.isValid());
}


QTEST_MAIN(tst_ubuntucomponentsbuttontest)

#include "tst_ubuntucomponentsbuttontest.moc"
