/*
 * Copyright 2013-2016 Canonical Ltd.
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
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <UbuntuToolkit/private/ucdeprecatedtheme_p.h>
#include <UbuntuToolkit/private/uctheme_p.h>
#include "uctestcase.h"
#include <private/qquicktext_p.h>
#include <ubuntutoolkitmodule.h>

UT_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QQmlError>)

class ThemeTestCase : public UbuntuTestCase
{
    Q_OBJECT
public:
    ThemeTestCase(const QString& file, QWindow* parent = 0)
        : UbuntuTestCase(file, QQuickView::SizeViewToRootObject, true, parent)
    {
    }

    ~ThemeTestCase()
    {
        // restore theme before quitting
        if (!rootContext()) {
            return;
        }
        UCDeprecatedTheme *theme = rootContext()->contextProperty("Theme").value<UCDeprecatedTheme*>();
        if (theme) {
            theme->resetName();
        } else {
            qWarning() << "No theme instance found!";
        }
    }

    void setTheme(const QString &theme)
    {
        rootObject()->setProperty("themeName", theme);
        QTest::waitForEvents();
    }

    void setStyle(const QString &style)
    {
        rootObject()->setProperty("styleDocument", style);
        QTest::waitForEvents();
    }
};

class tst_UCDeprecatedTheme : public QObject
{
    Q_OBJECT
private:
    QString m_xdgDataPath;

    UCDeprecatedTheme *instance(QQmlEngine &engine)
    {
        return engine.rootContext()->contextProperty("Theme").value<UCDeprecatedTheme*>();
    }
    void initDeprecatedTheme(QQmlEngine &engine)
    {
        UbuntuToolkitModule::initializeContextProperties(&engine);
    }

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testNameDefault();
    void testNameSet();
    void testCreateStyleComponent();
    void testCreateStyleComponent_data();
    void testThemesRelativePath();
    void testThemesRelativePathWithParent();
    void testThemesRelativePathWithParentXDGDATA();
    void testThemesRelativePathWithParentNoVariablesSet();
    void testThemesRelativePathWithParentOneXDGPathSet();
    void testNoImportPathSet();
    void testBogusImportPathSet();
    void testMultipleImportPathsSet();
    void testPaletteUsed_bug1549830();
};

void tst_UCDeprecatedTheme::initTestCase()
{
    m_xdgDataPath = QLatin1String(getenv("XDG_DATA_DIRS"));
    qputenv("SUPPRESS_DEPRECATED_NOTE", "yes");
}

void tst_UCDeprecatedTheme::cleanupTestCase()
{
    qputenv("XDG_DATA_DIRS", m_xdgDataPath.toLocal8Bit());
}

void tst_UCDeprecatedTheme::testNameDefault()
{
    QQmlEngine engine;
    initDeprecatedTheme(engine);
    QCOMPARE(instance(engine)->name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCDeprecatedTheme::testNameSet()
{
    QTest::ignoreMessage(QtWarningMsg, "Theme not found: \"MyBeautifulTheme\"");

    QQmlEngine engine;
    initDeprecatedTheme(engine);
    instance(engine)->setName("MyBeautifulTheme");
    QCOMPARE(instance(engine)->name(), QString("MyBeautifulTheme"));
}

void tst_UCDeprecatedTheme::testCreateStyleComponent()
{
    QFETCH(QString, styleName);
    QFETCH(QString, parentName);
    QFETCH(bool, success);

    if (parentName == "SimpleItem.qml")
        QTest::ignoreMessage(QtWarningMsg, "QQmlComponent: Component is not ready");
    else if (styleName == "NotExistingTestStyle.qml")
        ThemeTestCase::ignoreWarning(parentName, 20, 1, "QML Parent: Warning: Style NotExistingTestStyle.qml not found in theme TestModule.TestTheme");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

    QScopedPointer<ThemeTestCase> view(new ThemeTestCase(parentName));
    QVERIFY(view);
    view->setTheme("TestModule.TestTheme");
    view->setStyle(styleName);
    QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
    QCOMPARE(style != NULL, success);
}

void tst_UCDeprecatedTheme::testCreateStyleComponent_data() {
    QTest::addColumn<QString>("styleName");
    QTest::addColumn<QString>("parentName");
    QTest::addColumn<bool>("success");
    QTest::newRow("Existing style") << "TestStyle.qml" << "Parent.qml" << true;
    QTest::newRow("Non existing style") << "NotExistingTestStyle.qml" << "Parent.qml" << false;
}

void tst_UCDeprecatedTheme::testThemesRelativePath()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../tst_theme_engine");

    QScopedPointer<ThemeTestCase> view(new ThemeTestCase("Parent.qml"));
    QVERIFY(view);
    view->setTheme("TestModule.TestTheme");
    view->setStyle("TestStyle.qml");
    QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
    QCOMPARE(style != NULL, true);
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParent()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "./themes:./themes/TestModule");

    QScopedPointer<ThemeTestCase> view(new ThemeTestCase("Parent.qml"));
    QVERIFY(view);
    view->setTheme("CustomTheme");
    view->setStyle("TestStyle.qml");
    QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
    QCOMPARE(style != NULL, true);
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParentXDGDATA()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "./themes:./themes/TestModule");

    QScopedPointer<ThemeTestCase> view(new ThemeTestCase("Parent.qml"));
    QVERIFY(view);
    view->setTheme("CustomTheme");
    view->setStyle("TestStyle.qml");
    QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
    QCOMPARE(style != NULL, true);
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParentNoVariablesSet()
{
    ThemeTestCase::ignoreWarning("Parent.qml", 20, 1, "QML Parent: Warning: Style TestStyle.qml not found in theme Ubuntu.Components.Themes.Ambiance");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");

    QScopedPointer<ThemeTestCase> view(new ThemeTestCase("Parent.qml"));
    QVERIFY(view);
    view->setStyle("TestStyle.qml");
}

void tst_UCDeprecatedTheme::testThemesRelativePathWithParentOneXDGPathSet()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "../tst_theme_engine");

    QScopedPointer<ThemeTestCase> view(new ThemeTestCase("Parent.qml"));
    QVERIFY(view);
    view->setTheme("TestModule.TestTheme");
    view->setStyle("TestStyle.qml");
    QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
    QCOMPARE(style != NULL, true);
}

void tst_UCDeprecatedTheme::testNoImportPathSet()
{
    if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
        QSKIP("This can only be tested if the UITK is installed");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");
    qputenv("QML2_IMPORT_PATH", "");

    QQmlEngine engine;
    initDeprecatedTheme(engine);
    QCOMPARE(instance(engine)->name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCDeprecatedTheme::testBogusImportPathSet()
{
    if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
        QSKIP("This can only be tested if the UITK is installed");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");
    qputenv("QML2_IMPORT_PATH", "/no/plugins/here");

    QQmlEngine engine;
    initDeprecatedTheme(engine);
    QCOMPARE(instance(engine)->name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCDeprecatedTheme::testMultipleImportPathsSet()
{
    if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
        QSKIP("This can only be tested if the UITK is installed");

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");
    qputenv("QML2_IMPORT_PATH", "/no/plugins/here:.");

    QScopedPointer<ThemeTestCase> view(new ThemeTestCase("Parent.qml"));
    QVERIFY(view);
    view->setTheme("TestModule.TestTheme");
}

void tst_UCDeprecatedTheme::testPaletteUsed_bug1549830()
{
    ThemeTestCase::ignoreWarning("ErroneousPaletteUse.qml", 29, 20, "Unable to assign [undefined] to QColor", 3);
    QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ErroneousPaletteUse.qml"));
}


QTEST_MAIN(tst_UCDeprecatedTheme)

#include "tst_deprecated_theme_engine.moc"
