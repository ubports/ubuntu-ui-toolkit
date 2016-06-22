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
#include <asyncloader.h>
#include <functional>
#include <QtQml/QQmlEngine>

UT_USE_NAMESPACE

class LoaderSpy : public QObject
{
    Q_OBJECT
public:
    AsyncLoader *m_loader;
    QScopedPointer<QObject> m_object;
    bool m_done = false;
    bool m_error = false;
    QList<AsyncLoader::LoadingStatus> m_statusList;
public:
    LoaderSpy(AsyncLoader *loader)
        : QObject(0)
        , m_loader(loader)
    {
        connect(loader, &AsyncLoader::loadingStatus,
                this, &LoaderSpy::onLoadingStatusChanged);
    }

protected Q_SLOTS:
    virtual void onLoadingStatusChanged(UT_PREPEND_NAMESPACE(AsyncLoader)::LoadingStatus status, QObject *object)
    {
        m_statusList << status;
        if (status == AsyncLoader::Ready) {
            m_done = true;
            m_object.reset(object);
        }
        if (status == AsyncLoader::Error) {
            m_error = true;
        }
    }
};

class ResetLoaderSpy : public LoaderSpy
{
    Q_OBJECT
public:
    bool m_reset = false;
public:
    ResetLoaderSpy(AsyncLoader *loader)
        : LoaderSpy(loader)
    {
    }

    void onLoadingStatusChanged(UT_PREPEND_NAMESPACE(AsyncLoader)::LoadingStatus status, QObject *object) override
    {
        if (status == AsyncLoader::Loading) {
            m_loader->reset();
            m_reset = true;
        }
        LoaderSpy::onLoadingStatusChanged(status, object);
    }
};

class SecondLoaderSpy : public LoaderSpy
{
    Q_OBJECT
public:
    bool m_loadResult = false;
public:
    SecondLoaderSpy(AsyncLoader *loader, AsyncLoader::LoadingStatus loadAt,
                    const QUrl &url, QQmlContext *context)
        : LoaderSpy(loader)
        , m_loadAt(loadAt)
        , m_url(url)
        , m_context(context)
    {
    }

    void onLoadingStatusChanged(AsyncLoader::LoadingStatus status, QObject *object) override
    {
        if (status == (AsyncLoader::LoadingStatus)m_loadAt) {
            m_loadAt = -1;
            delete object;
            object = nullptr;
            m_loadResult = m_loader->load(m_url, m_context);
        }
        LoaderSpy::onLoadingStatusChanged(status, object);
    }

private:
    int m_loadAt;
    QUrl m_url;
    QQmlContext *m_context;
};

/********************************************************************
 * Test
 ********************************************************************/
class tst_AsyncLoader : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void test_load_data()
    {
        QTest::addColumn<bool>("loadAsDocument");
        QTest::addColumn<QString>("document");
        QTest::addColumn<int>("mode");
        QTest::addColumn< QList<int> >("statuses");

        QTest::newRow("document") << true << "Document.qml" << (int)QQmlComponent::Asynchronous
                                  << (QList<int>()
                                      << AsyncLoader::Compiling
                                      << AsyncLoader::Loading
                                      << AsyncLoader::Initializing
                                      << AsyncLoader::Ready
                                      );
        QTest::newRow("component, asynchronous") << false << "Document.qml" << (int)QQmlComponent::Asynchronous
                                                 << (QList<int>()
                                                     << AsyncLoader::Compiling
                                                     << AsyncLoader::Loading
                                                     << AsyncLoader::Initializing
                                                     << AsyncLoader::Ready
                                                     );
        QTest::newRow("component, synchronous") << false << "Document.qml" << (int)QQmlComponent::PreferSynchronous
                                                << (QList<int>()
                                                    << AsyncLoader::Loading
                                                    << AsyncLoader::Initializing
                                                    << AsyncLoader::Ready
                                                    );
    }
    void test_load()
    {
        QFETCH(bool, loadAsDocument);
        QFETCH(QString, document);
        QFETCH(int, mode);
        QFETCH(QList<int>, statuses);

        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("TestApp.qml"));
        AsyncLoader loader;
        LoaderSpy spy(&loader);
        QScopedPointer<QQmlComponent> component;

        if (loadAsDocument) {
            loader.load(QUrl::fromLocalFile(document), view->rootContext());
        } else {
            // create a component
            component.reset(new QQmlComponent(view->engine(), QUrl::fromLocalFile(document), (QQmlComponent::CompilationMode)mode));
            loader.load(component.data(), view->rootContext());
        }
        QTRY_VERIFY(spy.m_object != nullptr);
        // check the statuses
        QCOMPARE(spy.m_statusList.length(), statuses.length());
        for (int i = 0; i < spy.m_statusList.length(); i++) {
            QVERIFY2((int)spy.m_statusList[i] == statuses[i], (QString(" Status at index %1 differs").arg(i)).toLocal8Bit().constData());
        }
    }

    void test_load_with_error()
    {
        QUrl document = QUrl::fromLocalFile("FaultyDocument.qml");
        UbuntuTestCase::ignoreWarning("FaultyDocument.qml", 20, "Label is not a type");
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("TestApp.qml"));
        AsyncLoader loader;
        LoaderSpy spy(&loader);
        loader.load(document, view->rootContext());
        QTRY_VERIFY(spy.m_error == true);
    }

    void test_load_no_context()
    {
        AsyncLoader loader;
        QVERIFY(!loader.load(nullptr, nullptr));
        QVERIFY(!loader.load(QUrl(), nullptr));
    }

    void test_load_invalid_url()
    {
        QQmlEngine engine;
        AsyncLoader loader;
        LoaderSpy spy(&loader);
        QVERIFY(!loader.load(QUrl(), engine.rootContext()));
    }

    void test_load_null_component()
    {
        QQmlEngine engine;
        AsyncLoader loader;
        LoaderSpy spy(&loader);
        QVERIFY(!loader.load(nullptr, engine.rootContext()));
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
        AsyncLoader loader;
        ResetLoaderSpy spy(&loader);
        QScopedPointer<QQmlComponent> component;

        if (loadAsDocument) {
            loader.load(QUrl::fromLocalFile(document), view->rootContext());
        } else {
            // create a component
            component.reset(new QQmlComponent(view->engine(), QUrl::fromLocalFile(document), (QQmlComponent::CompilationMode)mode));
            loader.load(component.data(), view->rootContext());
        }
        if (loader.status() < AsyncLoader::Loading) {
            // cannot reset yet!
            QVERIFY(!loader.reset());
        }
        QTRY_VERIFY(spy.m_reset);
    }

    void test_second_load_scenarios_data()
    {
        QTest::addColumn<QString> ("doc1");
        QTest::addColumn<QString> ("doc2");
        QTest::addColumn<int> ("when");
        QTest::addColumn<bool> ("success");

        QTest::newRow("status = Compiling")
                << "Document.qml" << "HeavyDocument.qml"
                << (int)AsyncLoader::Compiling << false;
        QTest::newRow("status = Loading")
                << "Document.qml" << "HeavyDocument.qml"
                << (int)AsyncLoader::Loading << true;
        QTest::newRow("status = Initializing")
                << "Document.qml" << "HeavyDocument.qml"
                << (int)AsyncLoader::Initializing << true;
        QTest::newRow("status = Ready")
                << "Document.qml" << "HeavyDocument.qml"
                << (int)AsyncLoader::Ready << true;
        QTest::newRow("status = Error")
                << "FaultyDocument.qml" << "HeavyDocument.qml"
                << (int)AsyncLoader::Error << true;
    }
    void test_second_load_scenarios()
    {
        QFETCH(QString, doc1);
        QFETCH(QString, doc2);
        QFETCH(int, when);
        QFETCH(bool, success);

        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("TestApp.qml"));
        AsyncLoader loader;
        if (when == (int)AsyncLoader::Error) {
            UbuntuTestCase::ignoreWarning("FaultyDocument.qml", 20, "Label is not a type");
        }

        SecondLoaderSpy spy(&loader, (AsyncLoader::LoadingStatus)when, QUrl::fromLocalFile(doc2), view->rootContext());
        // load the first document
        QVERIFY(loader.load(QUrl::fromLocalFile(doc1), view->rootContext()));
        QTRY_VERIFY(spy.m_object != nullptr);
        QCOMPARE(spy.m_loadResult, success);
    }
};

QTEST_MAIN(tst_AsyncLoader)

#include "tst_asyncloader.moc"
