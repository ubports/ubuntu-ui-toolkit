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
#include "ucstyleditembase.h"
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
        UCStyleSet *styleSet = globalStyleSet();
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

    UCStyleSet *rootStyleSet()
    {
        return rootObject()->property("styleSet").value<UCStyleSet*>();
    }

    UCStyleSet *globalStyleSet()
    {
        return rootContext()->contextProperty("styleSet").value<UCStyleSet*>();
    }
};

class tst_UCMultiStyling : public QObject
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

    void cleanupTestCase()
    {
        qputenv("XDG_DATA_DIRS", m_xdgDataPath.toLocal8Bit());
    }
    void cleanup()
    {
        qputenv("UITK_SUBTHEMING", "yes");
        qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", m_themesPath.toLatin1());
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
        QVERIFY(view);
        view->setGlobalTheme("Ubuntu.Components.Themes.SuruDark");
        // verify theme changes
        UCStyledItemBase *styled = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QVERIFY(styled);
        QCOMPARE(styled->styleSet()->name(), QString("Ubuntu.Components.Themes.SuruDark"));
        // a deeper item's style name
        styled = view->findItem<UCStyledItemBase*>("secondLevelStyled");
        QVERIFY(styled);
        QCOMPARE(styled->styleSet()->name(), QString("Ubuntu.Components.Themes.SuruDark"));
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
        QVERIFY(view);
        // change styleset name (theme)
        UCStyledItemBase *styled = view->findItem<UCStyledItemBase*>(itemName);
        QVERIFY(styled);
        styled->styleSet()->setName("Ubuntu.Components.Themes.SuruDark");
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
        QVERIFY(view);
        UCStyleSet *styleSet = view->findItem<UCStyleSet*>("StyleSet");
        QVERIFY(styleSet);
        UCStyledItemBase *styledItem = applyOnItem.isEmpty() ?
                    qobject_cast<UCStyledItemBase*>(view->rootObject()) :
                    view->findItem<UCStyledItemBase*>(applyOnItem);
        QVERIFY(styledItem);
        // the styleset is declared, but should not be set yet!!!
        QVERIFY2(styledItem->styleSet() != styleSet, "StyleSet should not be set yet!");
        styleSet->setName(styleSetName);
        // set the style on the item
        styledItem->setStyleSet(styleSet);
        QTest::waitForEvents();
        // test on the items
        for (int i = 0; i < testItems.count(); i++) {
            QString itemName = testItems[i];
            QString themeName = expectedStyleNames[i];
            bool success = sameStyleSet[i];
            styledItem = view->findItem<UCStyledItemBase*>(itemName);
            QVERIFY(styledItem);
            QCOMPARE(styledItem->styleSet() == styleSet, success);
            QCOMPARE(styledItem->styleSet()->name(), themeName);
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
        QVERIFY(view);
        UCStyledItemBase *testItem = view->findItem<UCStyledItemBase*>(testStyledItem);
        QVERIFY(testItem);
        QCOMPARE(testItem->styleSet()->name(), testStyledItemStyleSets[0]);

        UCStyleSet *styleSet = view->findItem<UCStyleSet*>("StyleSet");
        QVERIFY(styleSet);
        styleSet->setName("Ubuntu.Components.Themes.SuruDark");
        UCStyledItemBase *suruItem = view->findItem<UCStyledItemBase*>(suruStyledItem);
        QVERIFY(suruItem);
        suruItem->setStyleSet(styleSet);
        QTest::waitForEvents();
        QCOMPARE(testItem->styleSet()->name(), testStyledItemStyleSets[1]);

        // get items and reparent
        QQuickItem *sourceItem = view->findItem<QQuickItem*>(sourceItemName);
        QQuickItem *newParentItem = view->findItem<QQuickItem*>(newParentItemName);
        QVERIFY(sourceItem);
        QVERIFY(newParentItem);
        QVERIFY(sourceItem != newParentItem);
        QVERIFY(sourceItem->parentItem() != newParentItem);

        QSignalSpy styleSetSpy(testItem, SIGNAL(styleSetChanged()));
        sourceItem->setParentItem(newParentItem);
        styleSetSpy.wait(200);
        QCOMPARE(testItem->styleSet()->name(), testStyledItemStyleSets[2]);
        QCOMPARE(styleSetSpy.count(), 1);
    }

    void test_styleset_name()
    {
        QScopedPointer<ThemeTestCase> view(new ThemeTestCase("DifferentStylesets.qml"));
        QVERIFY(view);
        UCStyledItemBase *main = qobject_cast<UCStyledItemBase*>(view->rootObject());
        QVERIFY(main);
        UCStyledItemBase *test = view->findItem<UCStyledItemBase*>("firstLevelStyled");
        QCOMPARE(main->styleSet()->name(), test->styleSet()->name());
    }
};

QTEST_MAIN(tst_UCMultiStyling)

#include "tst_multi_theming.moc"
