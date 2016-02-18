/*
 * Copyright 2016 Canonical Ltd.
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

#include <QtTest/QtTest>
#include "uctestcase.h"
#include "uctestextras.h"
#include <AsyncLoader>
#include <functional>
#include <QtQml/QQmlEngine>

class LoaderSpy : public QObject
{
    Q_OBJECT
public:
    UbuntuToolkit::AsyncLoader *m_loader;
    QObject *m_object = nullptr;
    bool m_loaded = false;
    QList<UbuntuToolkit::AsyncLoader::LoadingStatus> m_statusList;
public:
    LoaderSpy(UbuntuToolkit::AsyncLoader *loader)
        : QObject(0)
        , m_loader(loader)
    {
        connect(loader, &UbuntuToolkit::AsyncLoader::loadingStatus,
                this, &LoaderSpy::onLoadingStatusChanged);
    }

protected Q_SLOTS:
    virtual void onLoadingStatusChanged(UbuntuToolkit::AsyncLoader::LoadingStatus status, QObject *object)
    {
        qDebug() << "STATUS" << status;
        m_statusList << status;
        if (status == UbuntuToolkit::AsyncLoader::Ready) {
            m_loaded = true;
            m_object = object;
        }
    }
};

class ResetLoaderSpy : public LoaderSpy
{
    Q_OBJECT
public:
    bool m_reset = false;
public:
    ResetLoaderSpy(UbuntuToolkit::AsyncLoader *loader)
        : LoaderSpy(loader)
    {
    }

    void onLoadingStatusChanged(UbuntuToolkit::AsyncLoader::LoadingStatus status, QObject *object) override
    {
        if (status == UbuntuToolkit::AsyncLoader::Null) {
            qDebug() << "SHIT!!";
        }
        LoaderSpy::onLoadingStatusChanged(status, object);
        if (status == UbuntuToolkit::AsyncLoader::Loading) {
            m_loader->reset();
            m_reset = true;
        }
    }
};

class tst_AsyncLoader : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void test_load_data()
    {
        QTest::addColumn<bool>("loadAsDocument");
        QTest::addColumn<QString>("document");
        QTest::addColumn<int>("mode");

        QTest::newRow("document") << true << "Document.qml" << (int)QQmlComponent::Asynchronous;
        QTest::newRow("component, asynchronous") << false << "Document.qml" << (int)QQmlComponent::Asynchronous;
        QTest::newRow("component, synchronous") << false << "Document.qml" << (int)QQmlComponent::PreferSynchronous;
    }
    void test_load()
    {
        QFETCH(bool, loadAsDocument);
        QFETCH(QString, document);
        QFETCH(int, mode);

        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("TestApp.qml"));
        UbuntuToolkit::AsyncLoader loader;
        LoaderSpy spy(&loader);
        QScopedPointer<QQmlComponent> component;

        if (loadAsDocument) {
            loader.load(QUrl::fromLocalFile(document), view->rootContext());
        } else {
            // create a component
            component.reset(new QQmlComponent(view->engine(), QUrl::fromLocalFile(document), (QQmlComponent::CompilationMode)mode));
            loader.load(component.data(), view->rootContext());
        }
        QTRY_VERIFY_WITH_TIMEOUT(spy.m_object != nullptr, 2000);
        // check the statuses
        QCOMPARE(spy.m_statusList.length(), 4);
        QCOMPARE(spy.m_statusList[0], UbuntuToolkit::AsyncLoader::Reset);
        QCOMPARE(spy.m_statusList[1], UbuntuToolkit::AsyncLoader::Loading);
        QCOMPARE(spy.m_statusList[2], UbuntuToolkit::AsyncLoader::ComponentCreated);
        QCOMPARE(spy.m_statusList[3], UbuntuToolkit::AsyncLoader::Ready);
    }

    void test_load_with_error()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("TestApp.qml"));
        UbuntuToolkit::AsyncLoader loader;
        loader.load(QUrl::fromLocalFile("FaultyDocument.qml"), view->rootContext());
    }

    void test_load_and_cancel_data()
    {
        QTest::addColumn<bool>("loadAsDocument");
        QTest::addColumn<QString>("document");
        QTest::addColumn<int>("mode");

        QTest::newRow("document") << true << "HeavyDocument.qml" << (int)QQmlComponent::Asynchronous;
        QTest::newRow("component, asynchronous") << false << "HeavyDocument.qml" << (int)QQmlComponent::Asynchronous;
        QTest::newRow("component, synchronous") << false << "HeavyDocument.qml" << (int)QQmlComponent::PreferSynchronous;
    }
    void test_load_and_cancel()
    {
        QFETCH(bool, loadAsDocument);
        QFETCH(QString, document);
        QFETCH(int, mode);

        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("TestApp.qml"));
        UbuntuToolkit::AsyncLoader loader;
        ResetLoaderSpy spy(&loader);
        QScopedPointer<QQmlComponent> component;

        if (loadAsDocument) {
            loader.load(QUrl::fromLocalFile(document), view->rootContext());
        } else {
            // create a component
            component.reset(new QQmlComponent(view->engine(), QUrl::fromLocalFile(document), (QQmlComponent::CompilationMode)mode));
            loader.load(component.data(), view->rootContext());
        }
//        QTRY_VERIFY_WITH_TIMEOUT(spy.m_reset, 2000);
        QTest::qWait(2000);
    }

};

QTEST_MAIN(tst_AsyncLoader)

#include "tst_asyncloader.moc"

