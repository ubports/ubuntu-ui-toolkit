/*
 * Copyright 2012-2013 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QStandardPaths>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QCryptographicHash>
#include <QSettings>

#include <UbuntuToolkit/private/ucapplication_p.h>
#include <UbuntuToolkit/private/ucunits_p.h>
#include "uctestcase.h"
#include <ubuntutoolkitmodule.h>

UT_USE_NAMESPACE

class tst_MainView : public QObject
{
    Q_OBJECT

private:
    QQuickView *view;

public:
    tst_MainView() :
        view(0)
    {
    }

    QQuickItem *loadTest(const QString &document)
    {
        // load the document
        view->setSource(QUrl::fromLocalFile(document));
        QTest::waitForEvents();

        return view->rootObject();
    }

    QQuickItem *testItem(QQuickItem *that, const QString &identifier)
    {
        if (that->property(identifier.toLocal8Bit()).isValid())
            return that->property(identifier.toLocal8Bit()).value<QQuickItem*>();

        QList<QQuickItem*> children = that->findChildren<QQuickItem*>(identifier);
        return (children.count() > 0) ? children[0] : 0;
    }

private Q_SLOTS:

    void initTestCase()
    {
        QString modules(UBUNTU_QML_IMPORT_PATH);
        QVERIFY(QDir(modules).exists());

        view = new QQuickView;
        QQmlEngine *quickEngine = view->engine();
        UbuntuToolkitModule::initializeContextProperties(quickEngine);

        view->setGeometry(0,0, UCUnits::instance()->gu(40), UCUnits::instance()->gu(30));
        //add modules folder so we have access to the plugin from QML
        QStringList imports = quickEngine->importPathList();
        imports.prepend(QDir(modules).absolutePath());
        quickEngine->setImportPathList(imports);
    }

    void cleanupTestCase()
    {
        delete view;
    }

    void testCase_AppName()
    {
        QQuickItem *root = loadTest("AppName.qml");
        QVERIFY(root);
        QQuickItem *mainView = root;
        QString applicationName(mainView->property("applicationName").toString());
        QCOMPARE(applicationName, QString("org.gnu.wildebeest"));
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(QString(""), QCoreApplication::organizationName());
    }

    void testSetApplicationName() {
        QString appName("com.ubuntu.foo");
        UCApplication::instance()->setApplicationName(appName);
        QCOMPARE(UCApplication::instance()->applicationName(), appName);
        QCOMPARE(QCoreApplication::applicationName(), appName);
        QCOMPARE(QString(""), QCoreApplication::organizationName());
    }

    void testExpectedDataFolder() {
        QString appName("net.weight.gain");
        UCApplication::instance()->setApplicationName(appName);
        QString dataFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
        QString xdgDataHome(QProcessEnvironment::systemEnvironment().value("XDG_DATA_HOME",
            QProcessEnvironment::systemEnvironment().value("HOME") + "/.local/share"));
        QString expectedDataFolder(xdgDataHome + "/" + appName);
        QCOMPARE(dataFolder, expectedDataFolder);
    }

    void testExpectedCacheFolder() {
        QString appName("cat.long.very");
        UCApplication::instance()->setApplicationName(appName);
        QString cacheFolder(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        QString xdgCacheHome(QProcessEnvironment::systemEnvironment().value("XDG_CACHE_HOME",
            QProcessEnvironment::systemEnvironment().value("HOME") + "/.cache"));
        QString expectedCacheFolder(xdgCacheHome + "/" + appName);
        QCOMPARE(cacheFolder, expectedCacheFolder);
    }

    void testLocalStorage() {
        QQuickItem *root = loadTest("LocalStorage.qml");
        QVERIFY(root);
        QQuickItem *mainView = root;
        QString applicationName(mainView->property("applicationName").toString());
        QCOMPARE(applicationName, QString("tv.island.pacific"));
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(QString(""), QCoreApplication::organizationName());
        QString dataFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
        QString databaseFolder(dataFolder + "/Databases");
        QVERIFY(QFile::exists(databaseFolder));
        QString hash(QCryptographicHash::hash("pacific.island.tv", QCryptographicHash::Md5).toHex());
        QString database(databaseFolder + "/" + hash + ".sqlite");
        QVERIFY(QFile::exists(database));
    }

    QString getConfFile(QString applicationName) {
        QString configFolder(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
        QString subFolder(configFolder + "/" + applicationName);
        QString filename(subFolder + "/" + applicationName + ".conf");
        return filename;
    }

    void testLabsSettings() {
        QString applicationName("red.riding.hood");
        // Delete file if it exists to avoid false positives
        QString filename(getConfFile(applicationName));
        QFile::remove(filename);

        QQuickItem *root = loadTest("Settings.qml");
        QVERIFY(root);
        QQuickItem *mainView = root;
        QCOMPARE(applicationName, mainView->property("applicationName").toString());
        QCOMPARE(QString(applicationName), QCoreApplication::organizationDomain());
        QQuickItem *textField(testItem(mainView, "textfield"));
        textField->setProperty("text", "Blue");
        delete root;
        QVERIFY(QFile::exists(filename));
    }

    void testQSettings() {
        QString applicationName("i.prefer.pi");
        // Delete file if it exists to avoid false positives
        QString filename(getConfFile(applicationName));
        QFile::remove(filename);

        UCApplication::instance()->setApplicationName(applicationName);
        // QSettings with defaults
        QSettings mySettings;
        mySettings.setValue("spam", "eggs");
        // Force writing to disk
        mySettings.sync();
        QVERIFY(QFile::exists(filename));
    }

    void testNoWarnings_bug186065() {
        // An empty MainView would suffice
        QScopedPointer<UbuntuTestCase>testCase (new UbuntuTestCase("AppName.qml"));

        // No warnings from QML
        QCOMPARE(testCase->warnings(), 0);
    }

    void testWindowTitleFromPage() {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("PageTitle.qml"));
        QQuickItem *page = testCase->findItem<QQuickItem*>("page");
        QCOMPARE(QString("Once upon a time"), page->property("title").toString());
        QCOMPARE(testCase->title(), page->property("title").toString());
    }

    void testWindowTitleFromStack() {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("PageStack.qml"));
        QQuickItem *page = testCase->findItem<QQuickItem*>("page");
        QCOMPARE(QString("Far far away"), page->property("title").toString());
        QCOMPARE(testCase->title(), page->property("title").toString());
    }

    void testWindowTitleFromTabs() {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("TabsTitle.qml"));
        QQuickItem *page = testCase->findItem<QQuickItem*>("page");
        QCOMPARE(QString("Long long ago"), page->property("title").toString());
        QCOMPARE(testCase->title(), page->property("title").toString());
    }
};

QTEST_MAIN(tst_MainView)

#include "tst_mainview.moc"
