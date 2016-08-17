
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
 * Author: Nick Dedekind <nick.dedekind@gmail.com>
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

#include <UbuntuToolkit/private/ucunits_p.h>
#include <UbuntuToolkit/private/i18n_p.h>
#include <ubuntutoolkitmodule.h>

UT_USE_NAMESPACE

class tst_I18n_RelativeTime : public QObject
{
    Q_OBJECT

private:
    QQuickView *view;

public:
    tst_I18n_RelativeTime() :
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
        QString testAppDir(QDir::currentPath() + "/ubuntu-ui-toolkit");
        setenv("APP_DIR", testAppDir.toUtf8(), 1);

        // Verify that we set it correctly
        QVERIFY(QFileInfo(testAppDir + "/share/locale/en/LC_MESSAGES/ubuntu-ui-toolkit.mo").exists());

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

    void testCase_RelativeTime()
    {
        QQmlEngine engine;
        UbuntuToolkitModule::initializeContextProperties(&engine);
        UbuntuI18n* i18n = UbuntuI18n::instance();
        // By default no domain is set
        QCOMPARE(i18n->domain(), QString(""));

        // Start out with no localization
        i18n->setLanguage("C");
        // Load the app which should pick up the locale we prepared
        QQuickItem *root = loadTest("src/RelativeTime.qml");
        QVERIFY(root);
        QQuickItem *mainView = root;

        // There no translation happens
        QQuickItem* page(testItem(mainView, "page"));
        QVERIFY(page);

        QQuickItem* timeFarAway(testItem(page, "timeFarAway"));
        QVERIFY(timeFarAway);
        QCOMPARE(timeFarAway->property("text").toString(),
                 QDateTime(QDate(2000,1,1), QTime(0,0,0,0)).toString("ddd d MMM'\u2003'HH:mm"));
        QQuickItem* timeNow(testItem(page, "timeNow"));
        QVERIFY(timeNow);
        QCOMPARE(timeNow->property("text").toString(), QString("Now"));
        QQuickItem* timeMinuteBefore(testItem(page, "timeMinuteBefore"));
        QVERIFY(timeMinuteBefore);
        QCOMPARE(timeMinuteBefore->property("text").toString(), QString("1 minute ago"));
        QQuickItem* timeMinuteAfter(testItem(page, "timeMinuteAfter"));
        QVERIFY(timeMinuteAfter);
        QCOMPARE(timeMinuteAfter->property("text").toString(), QString("1 minute"));
        QQuickItem* tenMinutesBefore(testItem(page, "tenMinutesBefore"));
        QVERIFY(tenMinutesBefore);
        QCOMPARE(tenMinutesBefore->property("text").toString(), QString("10 minutes ago"));
        QQuickItem* tenMinutesAfter(testItem(page, "tenMinutesAfter"));
        QVERIFY(tenMinutesAfter);
        QCOMPARE(tenMinutesAfter->property("text").toString(), QString("10 minutes"));

        // There no translation happens in C++ either
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime()), QString("Now"));
        QCOMPARE(i18n->relativeDateTime(QDateTime(QDate(2000,1,1), QTime(0,0,0,0))),
                 QDateTime(QDate(2000,1,1), QTime(0,0,0,0)).toString("ddd d MMM'\u2003'HH:mm"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(-60)), QString("1 minute ago"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(60)), QString("1 minute"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(-600)), QString("10 minutes ago"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(600)), QString("10 minutes"));

        // Was the locale folder detected and set?
        QString boundDomain(C::bindtextdomain(i18n->domain().toUtf8(), ((const char*)0)));
        QString testAppDir(QDir::currentPath() + "/ubuntu-ui-toolkit");
        QString expectedLocalePath(QDir(testAppDir).filePath("share/locale"));
        QCOMPARE(boundDomain, expectedLocalePath);
        // Is the domain gettext uses correct?
        QString gettextDomain(C::textdomain(((const char*)0)));
        QCOMPARE(gettextDomain, i18n->domain());
        // Is the compiled en_US message catalog in the right location?
        QString messageCatalog(boundDomain + "/en/LC_MESSAGES/ubuntu-ui-toolkit.mo");
        QVERIFY(QFileInfo(messageCatalog).exists());

        // Check if system has en_US locale, otherwise gettext won't work
        QProcess localeA;
        localeA.start("locale -a");
        QVERIFY(localeA.waitForFinished());
        QVERIFY(QString(localeA.readAll()).split("\n").contains("en_US.utf8"));

        i18n->setLanguage("en_US.utf8");
        QSignalSpy spy(i18n, SIGNAL(languageChanged()));
        spy.wait();

        // Inspect translated strings in QML
        QCOMPARE(timeNow->property("text").toString(), QString("tr:Now"));
        QCOMPARE(timeMinuteBefore->property("text").toString(), QString("tr:1 minute ago"));
        QCOMPARE(timeMinuteAfter->property("text").toString(), QString("tr:1 minute"));
        QCOMPARE(tenMinutesBefore->property("text").toString(), QString("tr:10 minutes ago"));
        QCOMPARE(tenMinutesAfter->property("text").toString(), QString("tr:10 minutes"));
        QCOMPARE(timeFarAway->property("text").toString(), QString("tr:FarAway"));

        // Translate in C++
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime()), QString("tr:Now"));
        QCOMPARE(i18n->relativeDateTime(QDateTime(QDate(2000,1,1), QTime(0,0,0,0))),  QString("tr:FarAway"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(-60)), QString("tr:1 minute ago"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(60)), QString("tr:1 minute"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(-600)), QString("tr:10 minutes ago"));
        QCOMPARE(i18n->relativeDateTime(QDateTime::currentDateTime().addSecs(600)), QString("tr:10 minutes"));
    }
};

// The C++ equivalent of QTEST_MAIN(tst_I18n_RelativeTime) with added initialization
int main(int argc, char *argv[])
{
    // LC_ALL would fail the test case; it must be unset before execution
    unsetenv("LC_ALL");

    QGuiApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    tst_I18n_RelativeTime* testObject = new tst_I18n_RelativeTime();
    return QTest::qExec(static_cast<QObject*>(testObject), argc, argv);
}

#include "tst_i18n_RelativeTime.moc"
