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
    loadAndCreateComponentObject();
}

void tst_ubuntucomponentsbuttontest::cleanupTestCase() {
    // empty
}

void tst_ubuntucomponentsbuttontest::text() {
    QString propertyName("text");
    QString initialValue("Hello World");
    QString newValue("World Hello!");

    checkPropertyExistsAndExpectedType(propertyName,QVariant(initialValue).type());
    checkPropertyValue(propertyName,initialValue);
    setPropertyValue(propertyName,newValue);
    checkPropertyValue(propertyName,newValue);
}

void tst_ubuntucomponentsbuttontest::iconSource() {
    QString propertyName("iconSource");
    QUrl initialValue("icon.png");
    QUrl newValue("icon2.png");

    checkPropertyExistsAndExpectedType(propertyName,QVariant(initialValue).type());
    checkPropertyValue(propertyName,initialValue);
    setPropertyValue(propertyName,newValue);
    checkPropertyValue(propertyName,newValue);
}

void tst_ubuntucomponentsbuttontest::enabled() {
    QString propertyName("enabled");
    QBool initialValue(false);
    QBool newValue(true);

    checkPropertyExistsAndExpectedType(propertyName,QVariant(initialValue).type());
    checkPropertyValue(propertyName, QVariant(initialValue));
    setPropertyValue(propertyName, QVariant(newValue));
    checkPropertyValue(propertyName,QVariant(newValue));
}

void tst_ubuntucomponentsbuttontest::textColor() {
    QString propertyName("textColor");
    QColor initialValue = QColor::fromRgb(255,0,0,0);
    QColor newValue = QColor::fromRgb(0,0,255,0);

    checkPropertyExistsAndExpectedType(propertyName,QVariant(initialValue).type());
    checkPropertyValue(propertyName,initialValue);
    setPropertyValue(propertyName,newValue);
    checkPropertyValue(propertyName,newValue);
}

void tst_ubuntucomponentsbuttontest::textSize() {
    QString propertyName("textSize");
    QString initialValue("x-large");
    QString newValue("medium");

    checkPropertyExistsAndExpectedType(propertyName,QVariant(initialValue).type());
    checkPropertyValue(propertyName, initialValue);
    setPropertyValue(propertyName, newValue);
    checkPropertyValue(propertyName, newValue);
}

void tst_ubuntucomponentsbuttontest::iconPosition() {
    QString propertyName("iconPosition");
    QString initialValue("right");
    QString newValue("left");

    checkPropertyExistsAndExpectedType(propertyName,QVariant(initialValue).type());
    checkPropertyValue(propertyName, initialValue);
    setPropertyValue(propertyName, newValue);
    checkPropertyValue(propertyName, newValue);
}

void tst_ubuntucomponentsbuttontest::clicked() {
    checkSimpleSignal(SIGNAL(clicked()),"clicked");
}

QTEST_MAIN(tst_ubuntucomponentsbuttontest)

#include "tst_ubuntucomponentsbuttontest.moc"
