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
#include "ucdeprecatedtheme.h"
#include "uctestcase.h"
#include <private/qquicktext_p.h>

Q_DECLARE_METATYPE(QList<QQmlError>)

class tst_UCDeprecatedTheme : public QObject
{
    Q_OBJECT
private:
    QString m_xdgDataPath;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testInstance();
    void testNameDefault();
    void testNameSet();
    void testCreateStyleComponent();
    void testCreateStyleComponent_data();
    void testThemesRelativePath();
    void testThemesRelativePathWithParent();
    void testThemesRelativePathWithParentXDGDATA();
    void testThemesRelativePathWithParentNoVariablesSet();
    void testThemesRelativePathWithParentOneXDGPathSet();
    void testAppTheme();
    void testNoImportPathSet();
    void testBogusImportPathSet();
    void testMultipleImportPathsSet();
};

void tst_UCDeprecatedTheme::initTestCase()
{
    m_xdgDataPath = QLatin1String(getenv("XDG_DATA_DIRS"));
}

void tst_UCDeprecatedTheme::cleanupTestCase()
{
    qputenv("XDG_DATA_DIRS", m_xdgDataPath.toLocal8Bit());
}

void tst_UCDeprecatedTheme::testInstance()
{
    UCDeprecatedTheme::instance();
}

void tst_UCDeprecatedTheme::testNameDefault()
{
    UCDeprecatedTheme theme;
    QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCDeprecatedTheme::testNameSet()
{
    QTest::ignoreMessage(QtWarningMsg, "Theme not found: \"MyBeautifulTheme\"");

    UCDeprecatedTheme theme;
    theme.setName("MyBeautifulTheme");
    QCOMPARE(theme.name(), QString("MyBeautifulTheme"));
}

void tst_UCDeprecatedTheme::testCreateStyleComponent()
{
    QFETCH(QString, styleName);
    QFETCH(QString, parentName);
    QFETCH(bool, success);

    if (parentName.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, "QQmlComponent: Component is not ready");
    else if (styleName == "NotExistingTestStyle.qml")
        UbuntuTestCase::ignoreWarning(parentName, 19, 1, "QML Item: Warning: Style NotExistingTestStyle.qml not found in theme TestModule.TestTheme");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

    UCDeprecatedTheme theme;
    theme.setName("TestModule.TestTheme");
    QQmlEngine engine;
    QQmlComponent parentComponent(&engine, parentName);
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent(styleName, parent);

    QCOMPARE(component != NULL, success);
}

void tst_UCDeprecatedTheme::testCreateStyleComponent_data() {
    QTest::addColumn<QString>("styleName");
    QTest::addColumn<QString>("parentName");
    QTest::addColumn<bool>("success");
    QTest::newRow("Existing style") << "TestStyle.qml" << "Parent.qml" << true;
    QTest::newRow("Non existing style") << "NotExistingTestStyle.qml" << "Parent.qml" << false;
    QTest::newRow("No parent") << "TestStyle.qml" << "" << false;
}

void tst_UCDeprecatedTheme::testThemesRelativePath()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../tst_theme_engine");

    UCDeprecatedTheme theme;
    theme.setName("TestModule.TestTheme");
    QQmlEngine engine;
    QQmlComponent parentComponent(&engine, "Parent.qml");
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent("TestStyle.qml", parent);

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParent()
{
    QSKIP("https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1248982");
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../../resources/themes:../../resources/themes/TestModule");

    UCDeprecatedTheme theme;
    theme.setName("CustomTheme");
    QQmlEngine engine;
    theme.registerToContext(engine.rootContext());
    QQmlComponent parentComponent(&engine, "Parent.qml");
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent("TestStyle.qml", parent);

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParentXDGDATA()
{
    QSKIP("https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1248982");
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "../../resources/themes:../../resources/themes/TestModule");

    UCDeprecatedTheme theme;
    theme.setName("CustomTheme");
    QQmlEngine engine;
    theme.registerToContext(engine.rootContext());
    QQmlComponent parentComponent(&engine, "Parent.qml");
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent("TestStyle.qml", parent);

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParentNoVariablesSet()
{
    UbuntuTestCase::ignoreWarning("Parent.qml", 19, 1, "QML Item: Warning: Style TestStyle.qml not found in theme Ubuntu.Components.Themes.Ambiance");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");

    UCDeprecatedTheme theme;
    QQmlEngine engine;
    qRegisterMetaType<QList <QQmlError> >();
    QSignalSpy spy(&engine, SIGNAL(warnings(QList<QQmlError>)));
    theme.registerToContext(engine.rootContext());
    QQmlComponent parentComponent(&engine, "Parent.qml");
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent("TestStyle.qml", parent);
    // warning about TestStyle not defined in Ubuntu.Components.Themes.Ambiance should be shown
    QCOMPARE(spy.count(), 1);
    QCOMPARE(component == NULL, true);
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParentOneXDGPathSet()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "../tst_theme_engine");

    UCDeprecatedTheme theme;
    theme.setName("TestModule.TestTheme");
    QQmlEngine engine;
    QQmlComponent parentComponent(&engine, "Parent.qml");
    QObject* parent = parentComponent.create();
    QQmlComponent* component = theme.createStyleComponent("TestStyle.qml", parent);

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

void tst_UCDeprecatedTheme::testAppTheme()
{
    QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("TestApp.qml"));
    QColor backgroundColor = test->rootObject()->property("backgroundColor").value<QColor>();
    QCOMPARE(backgroundColor, QColor("#A21E1C"));
    QQuickText *label = test->findItem<QQuickText*>("test_label");
    QVERIFY(label);
    QCOMPARE(label->color(), QColor("lightblue"));
}

void tst_UCDeprecatedTheme::testNoImportPathSet()
{
    if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
        QSKIP("This can only be tested if the UITK is installed");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");
    qputenv("QML2_IMPORT_PATH", "");

    UCDeprecatedTheme theme;
    QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCDeprecatedTheme::testBogusImportPathSet()
{
    if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
        QSKIP("This can only be tested if the UITK is installed");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");
    qputenv("QML2_IMPORT_PATH", "/no/plugins/here");

    UCDeprecatedTheme theme;
    QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCDeprecatedTheme::testMultipleImportPathsSet()
{
    if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
        QSKIP("This can only be tested if the UITK is installed");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");
    qputenv("QML2_IMPORT_PATH", "/no/plugins/here:.");

    UCDeprecatedTheme theme;
    theme.setName("TestModule.TestTheme");
}

QTEST_MAIN(tst_UCDeprecatedTheme)

#include "tst_theme_enginetest.moc"
