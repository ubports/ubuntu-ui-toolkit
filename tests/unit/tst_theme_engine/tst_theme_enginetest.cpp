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
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include "uctheme.h"


class tst_UCTheme : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testInstance();
    void testNameDefault();
    void testNameSet();
    void testCreateStyleComponent();
    void testCreateStyleComponent_data();
    void testThemesRelativePath();
};

void tst_UCTheme::testInstance()
{
    UCTheme& theme = UCTheme::instance();
}

void tst_UCTheme::testNameDefault()
{
    UCTheme theme;
    QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCTheme::testNameSet()
{
    UCTheme theme;
    theme.setName("MyBeautifulTheme");
    QCOMPARE(theme.name(), QString("MyBeautifulTheme"));
}

void tst_UCTheme::testCreateStyleComponent()
{
    QFETCH(QString, styleName);
    QFETCH(QString, parentName);
    QFETCH(bool, success);

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

    UCTheme theme;
    theme.setName("TestModule.TestTheme");
    QQmlEngine engine;
    QQmlComponent parentComponent(&engine, parentName);
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent(styleName, parent);

    QCOMPARE(component != NULL, success);
}

void tst_UCTheme::testCreateStyleComponent_data() {
    QTest::addColumn<QString>("styleName");
    QTest::addColumn<QString>("parentName");
    QTest::addColumn<bool>("success");
    QTest::newRow("Existing style") << "TestStyle.qml" << "Parent.qml" << true;
    QTest::newRow("Non existing style") << "NotExistingTestStyle.qml" << "Parent.qml" << false;
    QTest::newRow("No parent") << "TestStyle.qml" << "" << false;
}

void tst_UCTheme::testThemesRelativePath()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../tst_theme_engine");

    UCTheme theme;
    theme.setName("TestModule.TestTheme");
    QQmlEngine engine;
    QQmlComponent parentComponent(&engine, "Parent.qml");
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent("TestStyle.qml", parent);

    QCOMPARE(component != NULL, true);
}


QTEST_MAIN(tst_UCTheme)

#include "tst_theme_enginetest.moc"
