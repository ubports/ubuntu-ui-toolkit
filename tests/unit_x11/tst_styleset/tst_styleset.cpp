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
#include "ucstyleset.h"
#include "uctestcase.h"
#include "ucstyleditembase_p.h"

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
        UCStyleSet *styleSet = rootContext()->contextProperty("styleSet").value<UCStyleSet*>();
        if (styleSet) {
            styleSet->resetName();
        } else {
            qWarning() << "No theme instance found!";
        }
    }

    QString globalTheme()
    {
        return globalStyleSet()->name();
    }

    void setGlobalTheme(const QString &theme)
    {
        UCStyleSet *rootSet = globalStyleSet();
        QVERIFY(rootSet);
        rootSet->setName(theme);
        QTest::waitForEvents();
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

    UCStyleSet *rootStyleSet()
    {
        return rootObject()->property("styleSet").value<UCStyleSet*>();
    }

    UCStyleSet *globalStyleSet()
    {
        return rootContext()->contextProperty("styleSet").value<UCStyleSet*>();
    }
};

class tst_UCStyleSet : public QObject
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
    }

    void cleanup()
    {
        qputenv("UITK_SUBTHEMING", "yes");
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", m_themesPath.toLatin1());
        qputenv("XDG_DATA_DIRS", m_xdgDataPath.toLocal8Bit());
    }

    void test_default_styleset()
    {
        UCStyleSet::defaultSet();
    }

    void test_default_name()
    {
        UCStyleSet styleSet;
        QCOMPARE(styleSet.name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_name_set()
    {
        QTest::ignoreMessage(QtWarningMsg, "Theme not found: \"MyBeautifulTheme\"");

        UCStyleSet styleSet;
        styleSet.setName("MyBeautifulTheme");
        QCOMPARE(styleSet.name(), QString("MyBeautifulTheme"));
    }

    void test_name_reset()
    {
        UCStyleSet styleSet;
        styleSet.setName("Ubuntu.Components.Themes.SuruDark");
        QCOMPARE(styleSet.name(), QString("Ubuntu.Components.Themes.SuruDark"));
        // reset
        styleSet.resetName();
        QCOMPARE(styleSet.name(), QString("Ubuntu.Components.Themes.Ambiance"));
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

        UCStyleSet styleSet;
        QCOMPARE(styleSet.name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_bogus_import_path_set()
    {
        if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
            QSKIP("This can only be tested if the UITK is installed");

        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "");
        qputenv("QML2_IMPORT_PATH", "/no/plugins/here");

        UCStyleSet styleSet;
        QCOMPARE(styleSet.name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_multiple_import_paths_set()
    {
        if (!QFile(QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "/Ubuntu/Components").exists())
            QSKIP("This can only be tested if the UITK is installed");

        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
        qputenv("XDG_DATA_DIRS", "");
        qputenv("QML2_IMPORT_PATH", "/no/plugins/here:.");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        view->setTheme("TestModule.TestTheme");
    }

    void test_styleset_not_root_stylset()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        view->setTheme("TestModule.TestTheme");
        UCStyleSet *styleSet = view->rootStyleSet();
        UCStyleSet *globalStyleSet = view->globalStyleSet();
        QVERIFY(styleSet);
        QVERIFY(globalStyleSet);
        QVERIFY(styleSet != globalStyleSet);
    }

    void test_styleset_reset_name()
    {
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("SimpleItem.qml"));
        view->setTheme("TestModule.TestTheme");
        UCStyleSet *styleSet = view->rootStyleSet();
        QCOMPARE(styleSet->name(), QString("TestModule.TestTheme"));
        // reset
        styleSet->resetName();
        QCOMPARE(styleSet->name(), QString("Ubuntu.Components.Themes.Ambiance"));
    }

    void test_parent_change_when_assinged()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DynamicAssignment.qml"));
        UCStyleSet *testSet = view->findItem<UCStyleSet*>("testSet");
        UCStyledItemBase *testItem = view->findItem<UCStyledItemBase*>("testItem");
        UCStyledItemBase *mainItem = qobject_cast<UCStyledItemBase*>(view->rootObject());

        QSignalSpy parentChangeSpy(testSet, SIGNAL(parentChanged()));
        UCStyledItemBasePrivate::get(testItem)->setStyleSet(testSet);
        parentChangeSpy.wait(200);
        QCOMPARE(parentChangeSpy.count(), 1);
        // test if the parent is correct
        QCOMPARE(testSet->parentSet(), UCStyledItemBasePrivate::get(mainItem)->getStyleSet());
    }

    void test_parent_set_reset_triggers_parent_change()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ParentChanges.qml"));
        UCStyleSet *testSet = view->findItem<UCStyleSet*>("testSet");
        UCStyledItemBase *mainItem = qobject_cast<UCStyledItemBase*>(view->rootObject());

        // reset mainItem's styleSet should trigger parentChanged on testSet
        QSignalSpy parentChangeSpy(testSet, SIGNAL(parentChanged()));
        UCStyledItemBasePrivate::get(mainItem)->resetStyleSet();
        parentChangeSpy.wait(200);
        QCOMPARE(parentChangeSpy.count(), 1);
        QCOMPARE(UCStyledItemBasePrivate::get(mainItem)->getStyleSet(), &UCStyleSet::defaultSet());
        QCOMPARE(testSet->parentSet(), UCStyledItemBasePrivate::get(mainItem)->getStyleSet());
    }

    void test_parent_set_namechange_triggers_parent_change()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("ParentChanges.qml"));
        UCStyleSet *testSet = view->findItem<UCStyleSet*>("testSet");
        UCStyledItemBase *mainItem = qobject_cast<UCStyledItemBase*>(view->rootObject());

        // change mainItem.styleSet.name should trigger parentChanged on testSet
        QSignalSpy parentChangeSpy(testSet, SIGNAL(parentChanged()));
        UCStyledItemBasePrivate::get(mainItem)->getStyleSet()->setName("Ubuntu.Components.Themes.SuruDark");
        parentChangeSpy.wait(200);
        QCOMPARE(parentChangeSpy.count(), 1);
        QCOMPARE(testSet->parentSet(), UCStyledItemBasePrivate::get(mainItem)->getStyleSet());
    }


    // testing global context property 'styleSet' name changes
    void test_set_global_styleset_name_data()
    {
        QTest::addColumn<QByteArray>("subtheming");

        QTest::newRow("Subtheming disabled") << QByteArray("no");
        QTest::newRow("Subtheming enabled") << QByteArray("yes");
    }
    void test_set_global_styleset_name()
    {
        QFETCH(QByteArray, subtheming);
        qputenv("UITK_SUBTHEMING", subtheming);
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestMain.qml"));
        view->setGlobalTheme("Ubuntu.Components.Themes.SuruDark");
        // verify theme changes
        UCStyledItemBase *styled = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QVERIFY(styled);
        QCOMPARE(UCStyledItemBasePrivate::get(styled)->getStyleSet()->name(), QString("Ubuntu.Components.Themes.SuruDark"));
        // a deeper item's style name
        styled = view->findItem<UCStyledItemBase*>("secondLevelStyled");
        QCOMPARE(UCStyledItemBasePrivate::get(styled)->getStyleSet()->name(), QString("Ubuntu.Components.Themes.SuruDark"));
    }

    // changing StyledItem.styleSet.name for different items within a tree where
    // no sub-theming si applied
    void test_set_styleditem_styleset_name_data()
    {
        QTest::addColumn<QByteArray>("subtheming");
        QTest::addColumn<QString>("itemName");

        QTest::newRow("Subtheming disabled, set firstLevelStyled") << QByteArray("no") << "firstLevelStyled";
        QTest::newRow("Subtheming disabled, set secondLevelStyled") << QByteArray("no") << "secondLevelStyled";
        QTest::newRow("Subtheming enabled, set firstLevelStyled") << QByteArray("yes") << "firstLevelStyled";
        QTest::newRow("Subtheming enabled, set secondLevelStyled") << QByteArray("yes") << "secondLevelStyled";
    }
    void test_set_styleditem_styleset_name()
    {
        QFETCH(QByteArray, subtheming);
        QFETCH(QString, itemName);
        qputenv("UITK_SUBTHEMING", subtheming);
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestMain.qml"));
        // change styleset name (theme)
        UCStyledItemBase *styled = view->findItem<UCStyledItemBase*>(itemName);
        UCStyledItemBasePrivate::get(styled)->getStyleSet()->setName("Ubuntu.Components.Themes.SuruDark");
        QTest::waitForEvents();
        UCStyleSet *style = view->globalStyleSet();
        QCOMPARE(style->name(), QString("Ubuntu.Components.Themes.SuruDark"));
    }

    void test_styleset_change_data()
    {
        QTest::addColumn<QString>("styleSetName");
        QTest::addColumn<QString>("applyOnItem"); // empty means root object
        QTest::addColumn<QStringList>("testItems"); // list of objectNames to be tested
        QTest::addColumn<QStringList>("expectedStyleNames"); // list of expected style names on the tested items
        QTest::addColumn< QList<bool> >("sameStyleSet"); // list of styleset vs styledItem.styleSet camparison results expected

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
    void test_styleset_change()
    {
        QFETCH(QString, styleSetName);
        QFETCH(QString, applyOnItem);
        QFETCH(QStringList, testItems);
        QFETCH(QStringList, expectedStyleNames);
        QFETCH(QList<bool>, sameStyleSet);

        bool validTest = (testItems.count() == expectedStyleNames.count()) &&
                (testItems.count() == sameStyleSet.count());
        QVERIFY2(validTest, "testItems, expectedStyleNames, and sameStyleSet must have same amount of values");

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestStyleChange.qml"));
        UCStyleSet *styleSet = view->findItem<UCStyleSet*>("StyleSet");
        UCStyledItemBase *styledItem = applyOnItem.isEmpty() ?
                    qobject_cast<UCStyledItemBase*>(view->rootObject()) :
                    view->findItem<UCStyledItemBase*>(applyOnItem);
        QVERIFY(styledItem);
        // the styleset is declared, but should not be set yet!!!
        QVERIFY2(UCStyledItemBasePrivate::get(styledItem)->getStyleSet() != styleSet, "StyleSet should not be set yet!");
        styleSet->setName(styleSetName);
        // set the style on the item
        UCStyledItemBasePrivate::get(styledItem)->setStyleSet(styleSet);
        QTest::waitForEvents();
        // test on the items
        for (int i = 0; i < testItems.count(); i++) {
            QString itemName = testItems[i];
            QString themeName = expectedStyleNames[i];
            bool success = sameStyleSet[i];
            styledItem = view->findItem<UCStyledItemBase*>(itemName);
            QCOMPARE(UCStyledItemBasePrivate::get(styledItem)->getStyleSet() == styleSet, success);
            QCOMPARE(UCStyledItemBasePrivate::get(styledItem)->getStyleSet()->name(), themeName);
        }
    }

    void test_reparent_styled_data()
    {
        QTest::addColumn<QString>("suruStyledItem");
        QTest::addColumn<QString>("testStyledItem");
        QTest::addColumn<QStringList>("testStyledItemStyleSets");
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
        QFETCH(QStringList, testStyledItemStyleSets);
        QFETCH(QString, sourceItemName);
        QFETCH(QString, newParentItemName);

        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("TestStyleChange.qml"));
        UCStyledItemBase *testItem = view->findItem<UCStyledItemBase*>(testStyledItem);
        QCOMPARE(UCStyledItemBasePrivate::get(testItem)->getStyleSet()->name(), testStyledItemStyleSets[0]);

        UCStyleSet *styleSet = view->findItem<UCStyleSet*>("StyleSet");
        styleSet->setName("Ubuntu.Components.Themes.SuruDark");
        UCStyledItemBase *suruItem = view->findItem<UCStyledItemBase*>(suruStyledItem);
        UCStyledItemBasePrivate::get(suruItem)->setStyleSet(styleSet);
        QTest::waitForEvents();
        QCOMPARE(UCStyledItemBasePrivate::get(testItem)->getStyleSet()->name(), testStyledItemStyleSets[1]);

        // get items and reparent
        QQuickItem *sourceItem = view->findItem<QQuickItem*>(sourceItemName);
        QQuickItem *newParentItem = view->findItem<QQuickItem*>(newParentItemName);
        QVERIFY(sourceItem != newParentItem);
        QVERIFY(sourceItem->parentItem() != newParentItem);

        QSignalSpy styleSetSpy(testItem, SIGNAL(styleSetChanged()));
        sourceItem->setParentItem(newParentItem);
        styleSetSpy.wait(200);
        QCOMPARE(UCStyledItemBasePrivate::get(testItem)->getStyleSet()->name(), testStyledItemStyleSets[2]);
        QCOMPARE(styleSetSpy.count(), 1);
    }

    void test_styleset_name()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DifferentStylesets.qml"));
        UCStyledItemBase *main = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QVERIFY(main);
        UCStyledItemBase *test = view->findItem<UCStyledItemBase*>("firstLevelStyled");
        QCOMPARE(UCStyledItemBasePrivate::get(main)->getStyleSet()->name(),
                 UCStyledItemBasePrivate::get(test)->getStyleSet()->name());
    }
};

QTEST_MAIN(tst_UCStyleSet)

#include "tst_styleset.moc"
