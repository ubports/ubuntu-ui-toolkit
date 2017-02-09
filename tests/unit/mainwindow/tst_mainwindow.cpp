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
#include <QtCore/QCryptographicHash>
#include <QtCore/QSettings>

#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>

#include <UbuntuToolkit/ubuntutoolkitmodule.h>
#include <UbuntuToolkit/private/quickutils_p.h>
#include <UbuntuToolkit/private/ucapplication_p.h>
#include <UbuntuToolkit/private/ucmainwindow_p.h>
#include <UbuntuToolkit/private/ucunits_p.h>

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
        if (!QDir(modulePath).exists()) {
            qWarning("'%s' doesn't exist", qPrintable(modulePath));
            return 0;
        }
        engine->addImportPath(modulePath);
        UbuntuToolkitModule::initializeContextProperties(engine);
        QPointer<QQmlComponent> component(new QQmlComponent(engine));
        component->loadUrl(QUrl::fromLocalFile(document), QQmlComponent::Asynchronous);
        while (component->isLoading())
            QCoreApplication::processEvents();
        QObject *toplevel(component->create());
        if (component->errorString() != "") {
            qWarning("%s", qPrintable(component->errorString()));
            return 0;
        }
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
        return window;
    }

    QQuickItem *testItem(QObject *that, const QString &identifier)
    {
        if (that->property(identifier.toLocal8Bit()).isValid())
            return that->property(identifier.toLocal8Bit()).value<QQuickItem*>();

        QList<QQuickItem*> children = that->findChildren<QQuickItem*>(identifier);
        return (children.count() > 0) ? children[0] : 0;
    }

private Q_SLOTS:

    void cleanup()
    {
        // Delete engine, and thereby also the UCApplication instance
        QObject* engine(UCApplication::instance()->parent());
        delete engine;
        QCoreApplication::setApplicationName(QStringLiteral(""));
        QCoreApplication::setOrganizationName(QStringLiteral(""));
    }

    // Note: tests/unit/mainview13 contains the UCApplication bits

    void testCase_AppName()
    {
        // Sanity check: no name set yet
        QCOMPARE(QStringLiteral("mainwindow"), QCoreApplication::applicationName());

        QString applicationName("org.gnu.wildebeest");
        QQuickWindow *mainWindow(loadTest("AppName.qml"));
        QVERIFY(mainWindow);
        QCOMPARE(applicationName, mainWindow->property("applicationName").toString());
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(QString(""), QCoreApplication::organizationName());
    }

    void testCase_OrganizationName()
    {
        // Sanity check: no organization set yet
        QCOMPARE(QStringLiteral(""), QCoreApplication::organizationName());

        QString applicationName("tv.island.pacific");
        QString organizationName("pacifist");
        QQuickWindow *mainWindow(loadTest("OrganizationName.qml"));
        QVERIFY(mainWindow);
        QCOMPARE(applicationName, mainWindow->property("applicationName").toString());
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(organizationName, mainWindow->property("organizationName").toString());
        QCOMPARE(organizationName, QCoreApplication::organizationName());
    }

    void testCase_NoOrganizationName()
    {
        // Sanity check: no organization set yet
        QCOMPARE(QStringLiteral(""), QCoreApplication::organizationName());

        // Custom values set outside of QML
        QString applicationName("com.example.random");
        QString organizationName(QStringLiteral("Example"));
        QCoreApplication::setApplicationName(applicationName);
        QCoreApplication::setOrganizationName(organizationName);

        QQuickWindow *mainWindow(loadTest("VisualRoot.qml"));
        QVERIFY(mainWindow);
        // MainView shouldn't interfere as applicationName wasn't set in QML
        QCOMPARE(QStringLiteral(""), mainWindow->property("applicationName").toString());
        QCOMPARE(applicationName, QCoreApplication::applicationName());
        QCOMPARE(QStringLiteral(""), mainWindow->property("organizationName").toString());
        QCOMPARE(organizationName, QCoreApplication::organizationName());
    }

    void testCase_VisualRoot()
    {
        QQuickWindow *mainWindow(loadTest("VisualRoot.qml"));
        QVERIFY(mainWindow);
        QQuickItem* myLabel(testItem(mainWindow, "myLabel"));
        QQuickItem* visualRoot(QuickUtils::instance()->rootItem(myLabel));
        QQuickItem* myRoot(testItem(mainWindow, "myRoot"));
        QCOMPARE(visualRoot, myRoot);
    }
};

QTEST_MAIN(tst_MainWindow)

#include "tst_mainwindow.moc"
