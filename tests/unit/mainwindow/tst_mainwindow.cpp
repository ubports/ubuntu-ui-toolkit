/*
 * Copyright 2012-2016 Canonical Ltd.
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

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>

#include <UbuntuToolkit/private/ucapplication_p.h>
#include <UbuntuToolkit/private/ucunits_p.h>
#include <ubuntutoolkitmodule.h>

UT_USE_NAMESPACE

class tst_MainWindow : public QObject
{
    Q_OBJECT

public:
    tst_MainWindow()
    {
    }

    QQuickWindow *loadTest(const QString &document)
    {
        // Can't use UbuntuTestCase: We need a Window root item
        QPointer<QQmlEngine> engine(new QQmlEngine());
        QString modulePath(UBUNTU_QML_IMPORT_PATH);
        Q_ASSERT(QDir(modulePath).exists());
        engine->addImportPath(modulePath);
        UbuntuToolkitModule::initializeContextProperties(engine);
        QPointer<QQmlComponent> component(new QQmlComponent(engine));
        component->loadUrl(QUrl::fromLocalFile(document), QQmlComponent::Asynchronous);
        while (component->isLoading())
            QCoreApplication::processEvents();
        QObject *toplevel(component->create());
        Q_ASSERT(toplevel);
        Q_ASSERT("" != component->errorString());
        QQuickWindow* window(qobject_cast<QQuickWindow *>(toplevel));
        if (window)
            engine->setIncubationController(window->incubationController());
        else {
            QQuickItem *rootItem = qobject_cast<QQuickItem *>(toplevel);
            if (rootItem) {
                QQuickView *view(new QQuickView(engine, 0));
                window = view;
                view->setResizeMode(QQuickView::SizeRootObjectToView);
                view->setContent(document, component, rootItem);
            }
        }
        window->show();
        QTest::qWaitForWindowExposed(window);
        return window;
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
    }

    void cleanupTestCase()
    {
    }

    // Note: tests/unit/mainview contains the UCApplication bits

    void testCase_AppName()
    {
        QQuickWindow *mainWindow(loadTest("AppName.qml"));
        QVERIFY(mainWindow);
        QString applicationName(mainWindow->property("applicationName").toString());
        QCOMPARE(applicationName, QString("org.gnu.wildebeest"));
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(QString(""), QCoreApplication::organizationName());
    }

    void testLocalStorage() {
        QString applicationName("org.gnu.wildebeest");
        // Delete file if it exists to avoid false positives
        QString dataFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
        QString databaseFolder(dataFolder + "/Databases");
        QString hash(QCryptographicHash::hash(qPrintable(applicationName), QCryptographicHash::Md5).toHex());
        QString database(databaseFolder + "/" + hash + ".sqlite");
        QFile::remove(database);

        QQuickWindow *mainWindow(loadTest("AppName.qml"));
        QVERIFY(mainWindow);
        QCOMPARE(applicationName, mainWindow->property("applicationName").toString());
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(QString(""), QCoreApplication::organizationName());
        QVERIFY2(QFile::exists(databaseFolder), qPrintable(databaseFolder));
        QVERIFY2(QFile::exists(database), qPrintable(database));
    }

    QString getConfFile(QString applicationName) {
        QString configFolder(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
        QString subFolder(configFolder + "/" + applicationName);
        QString filename(subFolder + "/" + applicationName + ".conf");
        return filename;
    }

    void testLabsSettings() {
        QString applicationName("org.gnu.wildebeest");
        // Delete file if it exists to avoid false positives
        QString filename(getConfFile(applicationName));
        QFile::remove(filename);

        QQuickWindow *mainWindow(loadTest("AppName.qml"));
        QCOMPARE(applicationName, mainWindow->property("applicationName").toString());
        QCOMPARE(QString(applicationName), QCoreApplication::organizationDomain());
        QQuickItem *textField(testItem(mainWindow->contentItem(), "textfield"));
        QVERIFY(textField);
        textField->setProperty("text", "Blue");
        delete mainWindow;
        QVERIFY(QFile::exists(filename));
    }
};

QTEST_MAIN(tst_MainWindow)

#include "tst_mainwindow.moc"
