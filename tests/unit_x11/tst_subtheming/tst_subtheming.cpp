/*
 * Copyright 2015 Canonical Ltd.
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
#include "uctheme.h"
#include "uctestcase.h"

class ThemeTestCase : public UbuntuTestCase
{
    Q_OBJECT
public:
    ThemeTestCase(const QString& file, QWindow* parent = 0)
        : UbuntuTestCase(file, parent)
    {
    }

    ~ThemeTestCase()
    {
        // restore theme before quitting
        if (!rootContext()) {
            return;
        }
        UCTheme *theme = globalTheme();
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

    UCTheme *theme()
    {
        return rootObject()->property("theme").value<UCTheme*>();
    }

    UCTheme *globalTheme()
    {
        return rootContext()->contextProperty("theme").value<UCTheme*>();
    }
};

class tst_Subtheming : public QObject
{
    Q_OBJECT
private:
    QString m_xdgDataPath;

private Q_SLOTS:
    void initTestCase()
    {
        m_xdgDataPath = QLatin1String(getenv("XDG_DATA_DIRS"));
    }

    void cleanupTestCase()
    {
        qputenv("XDG_DATA_DIRS", m_xdgDataPath.toLocal8Bit());
    }

    void test_default_theme()
    {
        UCTheme::defaultTheme();
    }

    void test_default_name()
    {
        UCTheme theme;
        QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_name_set()
    {
        QTest::ignoreMessage(QtWarningMsg, "Theme not found: \"MyBeautifulTheme\"");

        UCTheme theme;
        theme.setName("MyBeautifulTheme");
        QCOMPARE(theme.name(), QString("MyBeautifulTheme"));
    }

    void test_name_reset()
    {
        UCTheme theme;
        theme.setName("Ubuntu.Components.Themes.SuruDark");
        QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.SuruDark"));
        // reset
        theme.resetName();
        QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_create_style_component_data() {
        QTest::addColumn<QString>("styleName");
        QTest::addColumn<QString>("parentName");
        QTest::addColumn<bool>("success");
        QTest::newRow("Existing style") << "TestStyle.qml" << "SimpleItem.qml" << true;
        QTest::newRow("Non existing style") << "NotExistingTestStyle.qml" << "SimpleItem.qml" << false;
    }
    void test_create_style_component()
    {
        QFETCH(QString, styleName);
        QFETCH(QString, parentName);
        QFETCH(bool, success);

        if (styleName == "NotExistingTestStyle.qml") {
            ThemeTestCase::ignoreWarning(parentName, 20, 1, "QML SimpleItem: Warning: Style NotExistingTestStyle.qml not found in theme TestModule.TestTheme");
        }
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase(parentName));
        QVERIFY(view);
        view->setTheme("TestModule.TestTheme");
        view->setStyle(styleName);
        QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
        QCOMPARE(style != NULL, success);
    }

    void test_relative_theme_paths_environment_variables_data()
    {
        QTest::addColumn<QString>("themePath");
        QTest::addColumn<QString>("xdgPath");
        QTest::addColumn<QString>("theme");
        QTest::addColumn<QString>("style");
        QTest::addColumn<QString>("warning");
        QTest::addColumn<bool>("success");

        QTest::newRow("One toolkit theme path")
                << "../tst_theme_engine" << ""
                << "TestModule.TestTheme" << "TestStyle.qml"
                << "" << true;
        QTest::newRow("Two toolkit theme paths")
                << "./themes:./themes/TestModule" << ""
                << "CustomTheme" << "TestStyle.qml"
                << "" << true;
        QTest::newRow("One XDG path")
                << "" << "../tst_theme_engine"
                << "TestModule.TestTheme" << "TestStyle.qml"
                << "" << true;
        QTest::newRow("Two XDG paths")
                << "" << "./themes:./themes/TestModule"
                << "CustomTheme" << "TestStyle.qml"
                << "" << true;
        QTest::newRow("No variables")
                << "" << ""
                << QString() << "TestStyle.qml"
                << "QML SimpleItem: Warning: Style TestStyle.qml not found in theme Ubuntu.Components.Themes.Ambiance" << false;
    }

    void test_relative_theme_paths_environment_variables()
    {
        QFETCH(QString, themePath);
        QFETCH(QString, xdgPath);
        QFETCH(QString, theme);
        QFETCH(QString, style);
        QFETCH(QString, warning);
        QFETCH(bool, success);

        if (!warning.isEmpty()) {
            ThemeTestCase::ignoreWarning("SimpleItem.qml", 20, 1, warning);
        }

        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", themePath.toLocal8Bit());
        qputenv("XDG_DATA_DIRS", xdgPath.toLocal8Bit());

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        QVERIFY(view);
        if (!theme.isEmpty()) {
            view->setTheme(theme);
        }
        view->setStyle(style);
        QQmlComponent *styleComponent = view->rootObject()->property("style").value<QQmlComponent*>();
        QCOMPARE(styleComponent != NULL, success);
    }

    void test_import_path()
    {
        if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
            QSKIP("This can only be tested if the UITK is installed");

        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "");
        qputenv("QML2_IMPORT_PATH", "");

        UCTheme theme;
        QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_bogus_import_path_set()
    {
        if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
            QSKIP("This can only be tested if the UITK is installed");

        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "");
        qputenv("QML2_IMPORT_PATH", "/no/plugins/here");

        UCTheme theme;
        QCOMPARE(theme.name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_multiple_import_paths_set()
    {
        if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
            QSKIP("This can only be tested if the UITK is installed");

        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "");
        qputenv("QML2_IMPORT_PATH", "/no/plugins/here:.");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        QVERIFY(view);
        view->setTheme("TestModule.TestTheme");
    }

    void test_theme_not_root_theme()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        QVERIFY(view);
        view->setTheme("TestModule.TestTheme");
        UCTheme *theme = view->theme();
        UCTheme *globalTheme = view->globalTheme();
        QVERIFY(theme);
        QVERIFY(globalTheme);
        QVERIFY(theme != globalTheme);
    }

    void test_theme_reset_name()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        QVERIFY(view);
        view->setTheme("TestModule.TestTheme");
        UCTheme *theme = view->theme();
        QCOMPARE(theme->name(), QString("TestModule.TestTheme"));
        // reset
        theme->resetName();
        QCOMPARE(theme->name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }
};

QTEST_MAIN(tst_Subtheming)

#include "tst_subtheming.moc"
