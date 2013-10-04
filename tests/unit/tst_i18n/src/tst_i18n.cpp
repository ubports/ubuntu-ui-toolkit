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
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

namespace C {
#include <libintl.h>
}

#include "ucunits.h"
#include "i18n.h"

class tst_I18n : public QObject
{
    Q_OBJECT

private:
    QQuickView *view;

public:
    tst_I18n() :
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
        QString testDataFolder(QCoreApplication::applicationDirPath());
        setenv("XDG_DATA_HOME", testDataFolder.toUtf8().constData(), 1);

        // Verify that we set it correctly
        QString doubleCheckLocalePath(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
            "locale", QStandardPaths::LocateDirectory));
        QCOMPARE(doubleCheckLocalePath, testDataFolder + "/locale");
        QVERIFY(QFileInfo(testDataFolder + "/locale/en/LC_MESSAGES/localizedApp.mo").exists());

        QString modules("../../../modules");
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
        // Load the app which should pick up the locale we prepared
        QQuickItem *root = loadTest("src/LocalizedApp.qml");
        QVERIFY(root);
        QQuickItem *mainView = root;
        // Sanity checks to avoid confusion
        QString applicationName(mainView->property("applicationName").toString());
        QCOMPARE(applicationName, QString("localizedApp"));
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(applicationName, UbuntuI18n::instance().domain());

        // Was the locale folder detected and set?
        QString boundDomain(C::bindtextdomain(UbuntuI18n::instance().domain().toUtf8().constData(), ((const char*)0)));
        QString expectedLocalePath(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
            "locale", QStandardPaths::LocateDirectory));
        QCOMPARE(boundDomain, expectedLocalePath);

        /* For manual testing one can do something like
            env LANGUAGE=en_US TEXTDOMAINDIR=./tests/unit/tst_i18n/locale/ gettext localizedApp 'Welcome'
        */

        UbuntuI18n::instance().setLanguage("en_US");
        // Try to translate for real
        QCOMPARE(UbuntuI18n::instance().dtr(UbuntuI18n::instance().domain(), "Welcome"), QString("Greets"));
        QCOMPARE(UbuntuI18n::instance().tr("Count the kilometres"), QString("Count the clicks"));
        // TODO: Inspect translated strings in the QML view
    }
};

QTEST_MAIN(tst_I18n)

#include "tst_i18n.moc"
