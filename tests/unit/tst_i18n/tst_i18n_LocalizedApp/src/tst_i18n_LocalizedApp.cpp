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
#include <QtCore/QDir>
#include <QtCore/QProcessEnvironment>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtTest/qtest_gui.h>

namespace C {
#include <libintl.h>
}

#include "ucunits.h"
#include "i18n.h"

class tst_I18n_LocalizedApp : public QObject
{
    Q_OBJECT

private:
    QQuickView *view;

public:
    tst_I18n_LocalizedApp() :
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
        // Set test locale folder in the environment
        // Using setenv because QProcessEnvironment ignores changes
        QString testAppDir(QDir::currentPath() + "/localizedApp");
        setenv("APP_DIR", testAppDir.toUtf8(), 1);

        // Verify that we set it correctly
        QVERIFY(QFileInfo(testAppDir + "/share/locale/en/LC_MESSAGES/localizedApp.mo").exists());

        QString modules(UBUNTU_QML_IMPORT_PATH);
        QVERIFY(QDir(modules).exists());

        view = new QQuickView;
        QQmlEngine *quickEngine = view->engine();

        view->setGeometry(0,0, UCUnits::instance().gu(40), UCUnits::instance().gu(30));
        //add modules folder so we have access to the plugin from QML
        QStringList imports = quickEngine->importPathList();
        imports.prepend(QDir(modules).absolutePath());
        quickEngine->setImportPathList(imports);
    }

    void cleanupTestCase()
    {
        delete view;
    }

    void testCase_LocalizedApp()
    {
        UbuntuI18n* i18n = &UbuntuI18n::instance();
        // By default no domain is set
        QCOMPARE(i18n->domain(), QString(""));

        // Start out with no localization
        i18n->setLanguage("C");
        // Load the app which should pick up the locale we prepared
        QQuickItem *root = loadTest("src/LocalizedApp.qml");
        QVERIFY(root);
        QQuickItem *mainView = root;
        // Sanity checks to avoid confusion
        QString applicationName(mainView->property("applicationName").toString());
        QCOMPARE(applicationName, QString("localizedApp"));
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(applicationName, i18n->domain());

        // There no translation happens
        QQuickItem* page(testItem(mainView, "page"));
        QVERIFY(page);
        QCOMPARE(page->property("title").toString(), QString("Welcome"));
        QQuickItem* button(testItem(page, "button"));
        QVERIFY(button);
        QCOMPARE(button->property("text").toString(), QString("Count the kilometres"));
        QQuickItem* all1(testItem(page, "all1"));
        QVERIFY(all1);
        QCOMPARE(all1->property("text").toString(), QString("All"));
        QQuickItem* all2(testItem(page, "all2"));
        QVERIFY(all2);
        QCOMPARE(all2->property("text").toString(), QString("All"));

        // There no translation happens in C++ either
        QCOMPARE(i18n->dtr(i18n->domain(), QString("Welcome")), QString("Welcome"));
        QCOMPARE(i18n->tr(QString("Count the kilometres")), QString("Count the kilometres"));
        QCOMPARE(i18n->ctr(QString("All Contacts"), QString("All")), QString("All"));
        QCOMPARE(i18n->ctr(QString("All Calls"), QString("All")), QString("All"));

        // Was the locale folder detected and set?
        QString boundDomain(C::bindtextdomain(i18n->domain().toUtf8(), ((const char*)0)));
        QString testAppDir(QDir::currentPath() + "/localizedApp");
        QString expectedLocalePath(QDir(testAppDir).filePath("share/locale"));
        QCOMPARE(boundDomain, expectedLocalePath);
        // Is the domain gettext uses correct?
        QString gettextDomain(C::textdomain(((const char*)0)));
        QCOMPARE(gettextDomain, i18n->domain());
        // Is the compiled en_US message catalog in the right location?
        QString messageCatalog(boundDomain + "/en/LC_MESSAGES/localizedApp.mo");
        QVERIFY(QFileInfo(messageCatalog).exists());

        /* For manual testing one can do something like
            env LANGUAGE=en_US TEXTDOMAINDIR=./tests/unit/tst_i18n/locale/ gettext localizedApp 'Welcome'
        */

        // Check if system has en_US locale, otherwise gettext won't work
        QProcess localeA;
        localeA.start("locale -a");
        QVERIFY(localeA.waitForFinished());
        QVERIFY(QString(localeA.readAll()).split("\n").contains("en_US.utf8"));

        i18n->setLanguage("en_US.utf8");
        QSignalSpy spy(i18n, SIGNAL(languageChanged()));
        spy.wait();

        // Inspect translated strings in QML
        QCOMPARE(page->property("title").toString(), QString("Greets"));
        QCOMPARE(button->property("text").toString(), QString("Count the clicks"));
        QCOMPARE(all1->property("text").toString(), QString("Todos"));
        QCOMPARE(all2->property("text").toString(), QString("Todas"));
        // Only tagged, not actually translated
        QQuickItem* button2(testItem(page, "button2"));
        QVERIFY(button2);
        QCOMPARE(button2->property("text").toString(), QString("Count the kittens"));
        QQuickItem* all3(testItem(page, "all3"));
        QVERIFY(all3);
        QCOMPARE(all3->property("text").toString(), QString("All"));

        // Translate in C++
        QCOMPARE(i18n->dtr(i18n->domain(), QString("Welcome")), QString("Greets"));
        QCOMPARE(i18n->tr(QString("Count the kilometres")), QString("Count the clicks"));
        QCOMPARE(i18n->ctr(QString("All Contacts"), QString("All")), QString("Todos"));
        QCOMPARE(i18n->ctr(QString("All Calls"), QString("All")), QString("Todas"));
        // Only tagged, not actually translated
        QCOMPARE(i18n->tag(QString("All kittens")), QString("All kittens"));
        QCOMPARE(i18n->tag(QString("All Cats"), QString("All")), QString("All"));
        // Sanity-check that the test strings would otherwise work and not no-op by accident
        QCOMPARE(i18n->tr(QString("Count the kittens")), QString("Contar los gatitos"));
        QCOMPARE(i18n->ctr(QString("All Cats"), QString("All")), QString("Cada"));
    }
};

// The C++ equivalent of QTEST_MAIN(tst_I18n_LocalizedApp) with added initialization
int main(int argc, char *argv[])
{
    // LC_ALL would fail the test case; it must be unset before execution
    unsetenv("LC_ALL");

    QGuiApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    tst_I18n_LocalizedApp* testObject = new tst_I18n_LocalizedApp();
    return QTest::qExec(static_cast<QObject*>(testObject), argc, argv);
}

#include "tst_i18n_LocalizedApp.moc"
