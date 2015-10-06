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
#include "quickutils.h"
#include "uctestcase.h"
#include "ucstyleditembase_p.h"
#include "ucnamespace.h"
#include "ucunits.h"

class ThemeTestCase : public UbuntuTestCase
{
    Q_OBJECT
public:
    ThemeTestCase(const QString& file, bool assertOnFailure = true, QWindow* parent = 0)
        : UbuntuTestCase(file, assertOnFailure, parent)
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

    void setGlobalTheme(const QString &theme)
    {
        UCTheme *rootTheme = globalTheme();
        QVERIFY(rootTheme);
        rootTheme->setName(theme);
    }

    void setTheme(const QString &theme, QQuickItem *watchedItem)
    {
        QSignalSpy spy(watchedItem, SIGNAL(themeChanged()));
        rootObject()->setProperty("themeName", theme);
        waitForSignal(&spy);
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
    QString m_themesPath;

private Q_SLOTS:
    void initTestCase()
    {
        m_xdgDataPath = QLatin1String(getenv("XDG_DATA_DIRS"));
        m_themesPath = QLatin1String(getenv("UBUNTU_UI_TOOLKIT_THEMES_PATH"));
        qputenv("SUPPRESS_DEPRECATED_NOTE", "yes");
    }

    void cleanup()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", m_themesPath.toLatin1());
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
        QTest::addColumn<QString>("warning");
        QTest::newRow("Existing style") << "TestStyle" << "SimpleItem.qml" << true << QString();
        QTest::newRow("Non existing style") << "NotExistingTestStyle" << "SimpleItem.qml" << false << "QML SimpleItem: Warning: Style NotExistingTestStyle.qml not found in theme TestModule.TestTheme";
    }
    void test_create_style_component()
    {
        QFETCH(QString, styleName);
        QFETCH(QString, parentName);
        QFETCH(bool, success);
        QFETCH(QString, warning);

        if (!warning.isEmpty()) {
            ThemeTestCase::ignoreWarning(parentName, 20, 1, warning);
        }
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "./themes");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase(parentName));
        view->setTheme("TestModule.TestTheme", view->rootObject());
        view->rootObject()->setProperty("styleName", styleName);
        QQuickItem *testStyle = view->rootObject()->findChild<QQuickItem*>("TestStyle");
        QCOMPARE(testStyle != NULL, success);
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
                << "./themes" << ""
                << "TestModule.TestTheme" << "TestStyle"
                << "" << true;
        QTest::newRow("Two toolkit theme paths")
                << "./themes:./themes/TestModule" << ""
                << "CustomTheme" << "TestStyle"
                << "" << true;
        QTest::newRow("One XDG path")
                << "" << "./themes"
                << "TestModule.TestTheme" << "TestStyle"
                << "" << true;
        QTest::newRow("Two XDG paths")
                << "" << "./themes:./themes/TestModule"
                << "CustomTheme" << "TestStyle"
                << "" << true;
        QTest::newRow("No variables")
                << "" << ""
                << QString() << "TestStyle"
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
        if (!theme.isEmpty()) {
            view->setTheme(theme, view->rootObject());
        }
        view->rootObject()->setProperty("styleName", style);
        QQuickItem *styleItem = view->rootObject()->findChild<QQuickItem*>("TestStyle");
        QCOMPARE(styleItem != NULL, success);
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
        qputenv("QML2_IMPORT_PATH", "/no/plugins/here");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        view->setTheme("CustomModule.TestTheme", view->rootObject());
    }

    void test_theme_not_root_theme()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        view->setTheme("CustomModule.TestTheme", view->rootObject());
        UCTheme *theme = view->theme();
        UCTheme *globalTheme = view->globalTheme();
        QVERIFY(theme);
        QVERIFY(globalTheme);
        QVERIFY(theme != globalTheme);
    }

    void test_theme_reset_name()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "./themes");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        view->setTheme("TestModule.TestTheme", view->rootObject());
        UCTheme *theme = view->theme();
        QCOMPARE(theme->name(), QString("TestModule.TestTheme"));
        // reset
        theme->resetName();
        QCOMPARE(theme->name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_parent_change_when_assinged()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DynamicAssignment.qml"));
        UCTheme *testSet = view->findItem<UCTheme*>("testSet");
        UCStyledItemBase *testItem = view->findItem<UCStyledItemBase*>("testItem");
        UCStyledItemBase *mainItem = qobject_cast<UCStyledItemBase*>(view->rootObject());

        QSignalSpy parentChangeSpy(testSet, SIGNAL(parentThemeChanged()));
        UCStyledItemBasePrivate::get(testItem)->setTheme(testSet);
        parentChangeSpy.wait(200);
        QCOMPARE(parentChangeSpy.count(), 1);
        // test if the parent is correct
        QCOMPARE(testSet->parentTheme(), UCStyledItemBasePrivate::get(mainItem)->getTheme());
    }

    void test_parent_set_reset_triggers_parent_change()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ParentChanges.qml"));
        UCTheme *testSet = view->findItem<UCTheme*>("testSet");
        UCTheme *topTheme = view->findItem<UCTheme*>("top");
        UCStyledItemBase *mainItem = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QCOMPARE(testSet->parentTheme(), topTheme);

        // reset mainItem's theme should trigger parentChanged on testSet
        QSignalSpy parentChangeSpy(testSet, SIGNAL(parentThemeChanged()));
        UCStyledItemBasePrivate::get(mainItem)->resetTheme();
        parentChangeSpy.wait(200);
        QCOMPARE(parentChangeSpy.count(), 1);
        QCOMPARE(UCStyledItemBasePrivate::get(mainItem)->getTheme(), &UCTheme::defaultTheme());
        QCOMPARE(testSet->parentTheme(), UCStyledItemBasePrivate::get(mainItem)->getTheme());
    }

    void test_parent_set_namechange_triggers_parent_change()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ParentChanges.qml"));
        UCTheme *testSet = view->findItem<UCTheme*>("testSet");
        UCStyledItemBase *mainItem = qobject_cast<UCStyledItemBase*>(view->rootObject());

        // change mainItem.theme.name should trigger parentChanged on testSet
        QSignalSpy parentChangeSpy(testSet, SIGNAL(parentThemeChanged()));
        QSignalSpy themeChangeSpy(mainItem, SIGNAL(themeChanged()));
        UCStyledItemBasePrivate::get(mainItem)->getTheme()->setName("Ubuntu.Components.Themes.SuruDark");
        UbuntuTestCase::waitForSignal(&themeChangeSpy);
        parentChangeSpy.wait(200);
        QCOMPARE(parentChangeSpy.count(), 1);
        QCOMPARE(testSet->parentTheme(), UCStyledItemBasePrivate::get(mainItem)->getTheme());
    }


    // testing global context property 'theme' name changes
    void test_set_global_theme_name()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestMain.qml"));
        QSignalSpy themeChaneSpy(view->rootObject(), SIGNAL(themeChanged()));
        view->setGlobalTheme("Ubuntu.Components.Themes.SuruDark");
        UbuntuTestCase::waitForSignal(&themeChaneSpy);
        // verify theme changes
        UCStyledItemBase *styled = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QVERIFY(styled);
        QCOMPARE(UCStyledItemBasePrivate::get(styled)->getTheme()->name(), QString("Ubuntu.Components.Themes.SuruDark"));
        // a deeper item's style name
        styled = view->findItem<UCStyledItemBase*>("secondLevelStyled");
        QCOMPARE(UCStyledItemBasePrivate::get(styled)->getTheme()->name(), QString("Ubuntu.Components.Themes.SuruDark"));
    }

    // changing StyledItem.theme.name for different items within a tree where
    // no sub-theming si applied
    void test_set_styleditem_theme_name_data()
    {
        QTest::addColumn<QString>("itemName");

        QTest::newRow("Subtheming enabled, set firstLevelStyled") << "firstLevelStyled";
        QTest::newRow("Subtheming enabled, set secondLevelStyled") << "secondLevelStyled";
    }
    void test_set_styleditem_theme_name()
    {
        QFETCH(QString, itemName);
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestMain.qml"));
        // change theme name (theme)
        UCStyledItemBase *styled = view->findItem<UCStyledItemBase*>(itemName);
        QSignalSpy themeChangeSpy(styled, SIGNAL(themeChanged()));
        UCStyledItemBasePrivate::get(styled)->getTheme()->setName("Ubuntu.Components.Themes.SuruDark");
        UbuntuTestCase::waitForSignal(&themeChangeSpy);
        UCTheme *theme = view->globalTheme();
        QCOMPARE(theme->name(), QString("Ubuntu.Components.Themes.SuruDark"));
    }

    void test_theme_change_data()
    {
        QTest::addColumn<QString>("themeName");
        QTest::addColumn<QString>("applyOnItem"); // empty means root object
        QTest::addColumn<QStringList>("testItems"); // list of objectNames to be tested
        QTest::addColumn<QStringList>("expectedStyleNames"); // list of expected style names on the tested items
        QTest::addColumn< QList<bool> >("sameTheme"); // list of theme vs styledItem.theme camparison results expected

        QString suru("Ubuntu.Components.Themes.SuruDark");
        QString ambiance("Ubuntu.Components.Themes.Ambiance");
        QTest::newRow("Suru on mainStyled")
                << suru << ""
                << (QStringList() << "firstLevelStyled" << "secondLevelStyled" << "firstLevelLoaderStyled")
                << (QStringList() << suru << suru << suru)
                << (QList<bool>() << true << true << true);
        QTest::newRow("Suru on firstLevelStyled")
                << suru << "firstLevelStyled"
                << (QStringList() << "secondLevelStyled" << "firstLevelLoaderStyled")
                << (QStringList() << suru << ambiance)
                << (QList<bool>() << true << false);
    }
    void test_theme_change()
    {
        QFETCH(QString, themeName);
        QFETCH(QString, applyOnItem);
        QFETCH(QStringList, testItems);
        QFETCH(QStringList, expectedStyleNames);
        QFETCH(QList<bool>, sameTheme);

        bool validTest = (testItems.count() == expectedStyleNames.count()) &&
                (testItems.count() == sameTheme.count());
        QVERIFY2(validTest, "testItems, expectedStyleNames, and sametheme must have same amount of values");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestStyleChange.qml"));
        UCTheme *theme = view->findItem<UCTheme*>("Theme");
        UCStyledItemBase *styledItem = applyOnItem.isEmpty() ?
                    qobject_cast<UCStyledItemBase*>(view->rootObject()) :
                    view->findItem<UCStyledItemBase*>(applyOnItem);
        QVERIFY(styledItem);
        // the theme is declared, but should not be set yet!!!
        QVERIFY2(UCStyledItemBasePrivate::get(styledItem)->getTheme() != theme, "ThemeSettings should not be set yet!");
        theme->setName(themeName);
        // set the style on the item
        QSignalSpy themeChangeSpy(styledItem, SIGNAL(themeChanged()));
        UCStyledItemBasePrivate::get(styledItem)->setTheme(theme);
        UbuntuTestCase::waitForSignal(&themeChangeSpy);
        // test on the items
        for (int i = 0; i < testItems.count(); i++) {
            QString itemName = testItems[i];
            QString themeName = expectedStyleNames[i];
            bool success = sameTheme[i];
            styledItem = view->findItem<UCStyledItemBase*>(itemName);
            QCOMPARE(UCStyledItemBasePrivate::get(styledItem)->getTheme() == theme, success);
            QCOMPARE(UCStyledItemBasePrivate::get(styledItem)->getTheme()->name(), themeName);
        }
    }

    void test_reparent_styled_data()
    {
        QTest::addColumn<QString>("suruStyledItem");
        QTest::addColumn<QString>("testStyledItem");
        QTest::addColumn<QStringList>("testStyledItemThemes");
        QTest::addColumn<QString>("sourceItemName");
        QTest::addColumn<QString>("newParentItemName");

        QString suru("Ubuntu.Components.Themes.SuruDark");
        QString ambiance("Ubuntu.Components.Themes.Ambiance");
        QTest::newRow("Move item out of Suru sub-themed")
                << "firstLevelStyled" << "secondLevelStyled"
                << (QStringList() << ambiance << suru << ambiance)
                << "secondLevelStyled" << "firstLevelLoader";
        QTest::newRow("Move item into Suru sub-themed")
                << "firstLevelStyled" << "firstLevelLoaderStyled"
                << (QStringList() << ambiance << ambiance << suru)
                << "firstLevelLoader" << "secondLevelStyled";
        QTest::newRow("Move dynamic item out of Suru sub-themed")
                << "firstLevelStyled" << "thirdLevelLoaderStyled"
                << (QStringList() << ambiance << suru << ambiance)
                << "secondLevelStyled" << "firstLevelLoader";
    }
    void test_reparent_styled()
    {
        QFETCH(QString, suruStyledItem);
        QFETCH(QString, testStyledItem);
        QFETCH(QStringList, testStyledItemThemes);
        QFETCH(QString, sourceItemName);
        QFETCH(QString, newParentItemName);

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestStyleChange.qml"));
        UCStyledItemBase *testItem = view->findItem<UCStyledItemBase*>(testStyledItem);
        QCOMPARE(UCStyledItemBasePrivate::get(testItem)->getTheme()->name(), testStyledItemThemes[0]);

        UCTheme *theme = view->findItem<UCTheme*>("Theme");
        theme->setName("Ubuntu.Components.Themes.SuruDark");
        UCStyledItemBase *suruItem = view->findItem<UCStyledItemBase*>(suruStyledItem);
        QSignalSpy themeChangeSpy(suruItem, SIGNAL(themeChanged()));
        UCStyledItemBasePrivate::get(suruItem)->setTheme(theme);
        UbuntuTestCase::waitForSignal(&themeChangeSpy);
        QCOMPARE(UCStyledItemBasePrivate::get(testItem)->getTheme()->name(), testStyledItemThemes[1]);

        // get items and reparent
        QQuickItem *sourceItem = view->findItem<QQuickItem*>(sourceItemName);
        QQuickItem *newParentItem = view->findItem<QQuickItem*>(newParentItemName);
        QVERIFY(sourceItem != newParentItem);
        QVERIFY(sourceItem->parentItem() != newParentItem);

        QSignalSpy themeSpy(testItem, SIGNAL(themeChanged()));
        sourceItem->setParentItem(newParentItem);
        themeSpy.wait(200);
        QCOMPARE(UCStyledItemBasePrivate::get(testItem)->getTheme()->name(), testStyledItemThemes[2]);
        QCOMPARE(themeSpy.count(), 1);
    }

    void test_theme_name()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DifferentThemes.qml"));
        UCStyledItemBase *main = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QVERIFY(main);
        UCStyledItemBase *test = view->findItem<UCStyledItemBase*>("firstLevelStyled");
        QCOMPARE(UCStyledItemBasePrivate::get(main)->getTheme()->name(),
                 UCStyledItemBasePrivate::get(test)->getTheme()->name());
    }

    void test_no_change_in_other_suru_dark()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SameNamedPaletteSettings.qml"));
        UCTheme *firstTheme = view->findItem<UCTheme*>("firstTheme");
        UCTheme *secondTheme = view->findItem<UCTheme*>("secondTheme");
        QVERIFY(firstTheme->getPaletteColor("normal", "background") != secondTheme->getPaletteColor("normal", "background"));
    }

    void test_keep_palette_value_when_theme_changes()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ChangePaletteValueWhenParentChanges.qml"));
        UCTheme *firstTheme = view->findItem<UCTheme*>("firstTheme");

        QCOMPARE(firstTheme->getPaletteColor("normal", "background"), QColor("blue"));
        // change the theme
        QSignalSpy themeChaneSpy(view->rootObject(), SIGNAL(themeChanged()));
        view->setGlobalTheme("Ubuntu.Components.Themes.SuruDark");
        UbuntuTestCase::waitForSignal(&themeChaneSpy);
        QCOMPARE(firstTheme->getPaletteColor("normal", "background"), QColor("blue"));
    }

    void test_change_default_palette_in_children_kept_after_child_deletion()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ChangeDefaultPaletteInChildren.qml"));
        QQuickItem *loader = view->findItem<QQuickItem*>("loader");
        UCStyledItemBase *main = qobject_cast<UCStyledItemBase*>(view->rootObject());
        UCTheme *mainSet = UCStyledItemBasePrivate::get(main)->getTheme();
        QQuickItem *item = loader->property("item").value<QQuickItem*>();
        QVERIFY(item);

        QCOMPARE(mainSet->getPaletteColor("normal", "background"), QColor("blue"));
        // unload component
        QSignalSpy itemSpy(item, SIGNAL(destroyed()));
        loader->setProperty("sourceComponent", QVariant());
        itemSpy.wait();
        // palette stays!
        QCOMPARE(mainSet->getPaletteColor("normal", "background"), QColor("blue"));
    }

    void test_reset_palette_to_theme_default()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ChangePaletteValueWhenParentChanges.qml"));
        UCTheme *firstTheme = view->findItem<UCTheme*>("firstTheme");
        QColor prevColor = firstTheme->getPaletteColor("normal", "background");

        QVERIFY(prevColor.isValid());
        // reset palette
        QSignalSpy spy(firstTheme, SIGNAL(paletteChanged()));
        firstTheme->setPalette(NULL);
        spy.wait(200);
        QVERIFY(firstTheme->getPaletteColor("normal", "background") != prevColor);
    }

    void test_multiple_palette_instances()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("MultiplePaletteInstances.qml"));
        UCTheme *theme = view->findItem<UCTheme*>("theme");
        QObject *palette1 = view->findItem<QObject*>("palette1");
        QObject *palette2 = view->findItem<QObject*>("palette2");
        QColor prevColor = theme->getPaletteColor("normal", "background");

        // set the first palette
        QSignalSpy spy(theme, SIGNAL(paletteChanged()));
        theme->setPalette(palette1);
        spy.wait(200);
        QVERIFY(prevColor != theme->getPaletteColor("normal", "background"));
        QCOMPARE(theme->getPaletteColor("normal", "background"), QColor("blue"));

        spy.clear();
        theme->setPalette(palette2);
        spy.wait(200);
        QVERIFY(prevColor != theme->getPaletteColor("normal", "background"));
        QCOMPARE(theme->getPaletteColor("normal", "background"), QColor("pink"));
    }

    void test_dynamic_palette()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DynamicPalette.qml"));
        UCTheme *theme = view->findItem<UCTheme*>("theme");
        QQuickItem *loader = view->findItem<QQuickItem*>("paletteLoader");

        QVERIFY(theme->getPaletteColor("normal", "background") == QColor("blue"));

        QSignalSpy spy(loader, SIGNAL(itemChanged()));
        loader->setProperty("sourceComponent", QVariant());
        spy.wait(200);
        // unloaded palette by Loader should remove palette configuration
        QVERIFY(theme->getPaletteColor("normal", "background") != QColor("blue"));
    }

    void test_invalid_palette_object()
    {
        ThemeTestCase::ignoreWarning("InvalidPalette.qml", 22, 20, "QML QtObject: Not a Palette component.");
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("InvalidPalette.qml"));
    }

    void test_removing_closest_parent_styled()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "./themes:./themes/TestModule");
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ReparentStyledItemFollowsNewPathOnly.qml"));
        UCTheme *suruTheme = view->findItem<UCTheme*>("suruTheme");
        UCStyledItemBase *root = static_cast<UCStyledItemBase*>(view->rootObject());
        UCStyledItemBase *movableItem = view->findItem<UCStyledItemBase*>("movable");

        // verify movableItem's theme
        QCOMPARE(UCStyledItemBasePrivate::get(root)->getTheme(), UCStyledItemBasePrivate::get(movableItem)->getTheme());

        // set the theme for root
        QSignalSpy themeChangeSpy(root, SIGNAL(themeChanged()));
        UCStyledItemBasePrivate::get(root)->setTheme(suruTheme);
        UbuntuTestCase::waitForSignal(&themeChangeSpy);
        QCOMPARE(UCStyledItemBasePrivate::get(root)->getTheme(), UCStyledItemBasePrivate::get(movableItem)->getTheme());

        // set the parent item of the test item to 0
        QSignalSpy spy(movableItem, SIGNAL(themeChanged()));
        movableItem->setParentItem(Q_NULLPTR);
        spy.wait(500);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(UCStyledItemBasePrivate::get(movableItem)->getTheme(), &UCTheme::defaultTheme());
    }

    void test_reparented_styleditem_special_case()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "./themes:./themes/TestModule");
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ReparentStyledItemFollowsNewPathOnly.qml"));
        UCStyledItemBase *root = static_cast<UCStyledItemBase*>(view->rootObject());
        UCTheme *suruTheme = view->findItem<UCTheme*>("suruTheme");
        UCStyledItemBase *customThemedItem = view->findItem<UCStyledItemBase*>("customThemed");
        UCStyledItemBase *movableItem = view->findItem<UCStyledItemBase*>("movable");

        // check the themes
        QCOMPARE(UCStyledItemBasePrivate::get(root)->getTheme()->name(), QString("Ubuntu.Components.Themes.Ambiance"));
        QCOMPARE(UCStyledItemBasePrivate::get(customThemedItem)->getTheme()->name(), QString("CustomTheme"));
        QCOMPARE(UCStyledItemBasePrivate::get(movableItem)->getTheme()->name(), QString("Ubuntu.Components.Themes.Ambiance"));

        // move the movableItem under customThemedItem
        movableItem->setParentItem(customThemedItem);
        QCOMPARE(UCStyledItemBasePrivate::get(movableItem)->getTheme()->name(), QString("CustomTheme"));

        // set a new theme for the root, and make sure our theme stays the same
        QSignalSpy themeChangeSpy(root, SIGNAL(themeChanged()));
        UCStyledItemBasePrivate::get(root)->setTheme(suruTheme);
        UbuntuTestCase::waitForSignal(&themeChangeSpy);
        QCOMPARE(UCStyledItemBasePrivate::get(movableItem)->getTheme()->name(), QString("CustomTheme"));
    }

    void test_theme_versions_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<QString>("testValue");

        QTest::newRow("Theming version 1.3")
                << "StyledItemV13.qml"
                << "version1.3";
        QTest::newRow("App theme versioned")
                << "StyledItemAppThemeVersioned.qml"
                << "version1.3";
    }
    void test_theme_versions()
    {
        QFETCH(QString, document);
        QFETCH(QString, testValue);

        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "./themes:./themes/TestModule");
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase(document));
        UCStyledItemBase *styledItem = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QVERIFY(UCStyledItemBasePrivate::get(styledItem)->styleItem);
        QString newProperty(UCStyledItemBasePrivate::get(styledItem)->styleItem->property("newProperty").toString());
        QCOMPARE(newProperty, testValue);
        // NOTE TestTheme resets the theme therefore the theming will look for the tested style under Ambiance theme
        // which will cause a warning; therefore we mark the warning to be ignored
        ThemeTestCase::ignoreWarning(document, 19, 1, "QML StyledItem: Warning: Style TestStyle.qml not found in theme Ubuntu.Components.Themes.Ambiance");
    }

    void test_deprecated_theme()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "./themes:./themes/TestModule:" + m_themesPath.toLatin1());
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DeprecatedTheme.qml"));
    }

    void test_style_change_has_precedence()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("StyleOverride.qml"));
        QQmlComponent *style = view->rootObject()->property("customStyle").value<QQmlComponent*>();
        QQuickItem *button = view->findItem<QQuickItem*>("TestButton");

        button->setProperty("style", QVariant::fromValue(style));
        QVERIFY(button->findChild<QQuickItem*>("TestStyle"));
    }

    void test_style_kept_when_stylename_changes_data()
    {
        QTest::addColumn<QString>("styleName");

        QTest::newRow("Empty style name") << QString();
        QTest::newRow("Any style name") << "Bumblebee";
    }
    void test_style_kept_when_stylename_changes()
    {
        QFETCH(QString, styleName);
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("StyleKept.qml"));
        QQuickItem *button = view->findItem<QQuickItem*>("TestButton");

        button->setProperty("styleName", styleName);
        QVERIFY(button->findChild<QQuickItem*>("TestStyle"));
    }

    void test_style_kept_when_theme_changes()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("StyleKept.qml"));
        QQuickItem *button = view->findItem<QQuickItem*>("TestButton");
        UCTheme *theme = button->property("theme").value<UCTheme*>();
        QVERIFY(theme);

        theme->setName("Ubuntu.Components.Themes.SuruDark");
        QVERIFY(button->findChild<QQuickItem*>("TestStyle"));
    }

    void test_style_reset_to_theme_style()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("StyleKept.qml"));
        UCStyledItemBase *button = view->findItem<UCStyledItemBase*>("TestButton");
        UCTheme *theme = UCStyledItemBasePrivate::get(button)->getTheme();
        QVERIFY(theme);

        QVERIFY(button->findChild<QQuickItem*>("TestStyle"));
        // reset style property, which should reset style to the theme style
        UCStyledItemBasePrivate::get(button)->resetStyle();
        QQuickItem *styleItem = UCStyledItemBasePrivate::get(button)->styleInstance();
        QCOMPARE(QuickUtils::className(styleItem), QString("ButtonStyle"));
    }

    void test_stylename_extension_failure()
    {
        ThemeTestCase::ignoreWarning("DeprecatedTheme.qml", 19, 1, "QML StyledItem: Warning: Style OptionSelectorStyle.qml.qml not found in theme Ubuntu.Components.Themes.SuruGradient");
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DeprecatedTheme.qml"));
        view->rootObject()->setProperty("styleName", "OptionSelectorStyle.qml");
    }

    void test_stylehints_errors_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<int>("row");
        QTest::addColumn<int>("col");
        QTest::addColumn<QString>("xfail");

        QTest::newRow("No signals")
                << "StyleHintsWithSignal.qml" << 26 << 13 << "Signal properties are not supported. \n" \
"                 onDefaultColorChanged: {} \n" \
"                 ^";
        QTest::newRow("No embedded objects")
                << "StyleHintsWithObject.qml" << 26 << 26 << "StyleHints does not support creating state-specific objects. \n" \
"                 anyProperty: QtObject{} \n" \
"                              ^";
        QTest::newRow("StyleHints declared elsewhere")
                << "StyleHintsElsewhere.qml" << 24 << 5 << "QML StyleHints: StyleHints must be declared in a StyledItem or a derivate of it.";
        QTest::newRow("Invalid property")
                << "StyleHintsInvalidProperty.qml" << 25 << 9 << "QML StyleHints: Style 'ButtonStyle' has no property called 'invalidProperty'.";
    }
    void test_stylehints_errors()
    {
        QFETCH(QString, document);
        QFETCH(int, row);
        QFETCH(int, col);
        QFETCH(QString, xfail);

        if (!xfail.isEmpty()) {
            ThemeTestCase::ignoreWarning(document, row, col, xfail);
        }
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase(document, false));
    }

    void test_stylehints_simple_property()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimplePropertyHints.qml"));
        QQuickItem *button = view->findItem<QQuickItem*>("Button");
        QColor color = button->property("color").value<QColor>();
        QCOMPARE(color, QColor("blue"));
    }

    void test_stylehints_bindings()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("PropertyBindingHints.qml"));
        QQuickItem *button = view->findItem<QQuickItem*>("Button");
        QColor color = button->property("color").value<QColor>();
        QCOMPARE(color, QColor("blue"));
        // press the button
        QPointF pressPt(button->width()/2, button->height()/2);
        pressPt = view->rootObject()->mapFromItem(button, pressPt);
        QTest::mousePress(view.data(), Qt::LeftButton, 0, pressPt.toPoint());
        color = button->property("color").value<QColor>();
        QCOMPARE(color, QColor("tan"));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, pressPt.toPoint());
    }

    void test_stylehints_multiple_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<QString>("colorProperty");
        QTest::addColumn<QColor>("colorReleased");
        QTest::addColumn<QColor>("colorPressed");
        QTest::addColumn<QString>("widthProperty");
        QTest::addColumn<float>("width");

        QTest::newRow("Same document")
                << "MoreStyleHints.qml" << "defaultColor" << QColor("brown") << QColor("brown") << "minimumWidth" << UCUnits::instance().gu(20);
        QTest::newRow("Different document")
                << "GroupPropertyBindingHints.qml" << "gradientProxy.topColor" << QColor("blue") << QColor("tan") << "minimumWidth" << UCUnits::instance().gu(20);
    }
    void test_stylehints_multiple()
    {
        QFETCH(QString, document);
        QFETCH(QString, colorProperty);
        QFETCH(QColor, colorReleased);
        QFETCH(QColor, colorPressed);
        QFETCH(QString, widthProperty);
        QFETCH(float, width);

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase(document));
        UCStyledItemBase *button = view->findItem<UCStyledItemBase*>("Button");
        QQuickItem *styleItem = UCStyledItemBasePrivate::get(button)->styleItem;
        QVERIFY(styleItem);
        QQmlProperty qmlProperty(styleItem, colorProperty, qmlContext(styleItem));
        QCOMPARE(qmlProperty.read().value<QColor>(), colorReleased);
        QCOMPARE(styleItem->property(widthProperty.toUtf8()).toReal(), width);

        QPointF pressPt(button->width()/2, button->height()/2);
        pressPt = view->rootObject()->mapFromItem(button, pressPt);
        QTest::mousePress(view.data(), Qt::LeftButton, 0, pressPt.toPoint());
        QColor pressedColor = qmlProperty.read().value<QColor>();
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, pressPt.toPoint());
        QCOMPARE(pressedColor, colorPressed);
    }
};

QTEST_MAIN(tst_Subtheming)

#include "tst_subtheming.moc"
