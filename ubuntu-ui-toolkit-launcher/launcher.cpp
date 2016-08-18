/*
 * Copyright 2014-2016 Canonical Ltd.
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
 */

// Dedicated QML launcher with the ability to setup QQuickView/QQmlEngine
// differently and with various extensions. Used internally to write Autopilot
// test cases that exhibit specific behavior.

#include <iostream>
#include <QtCore/qdebug.h>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtCore/QFileInfo>
#include <QLibrary>
#include <QOpenGLContext>
#include <QtGui/private/qopenglcontext_p.h>
#include <QtQuick/private/qsgcontext_p.h>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <UbuntuToolkit/private/mousetouchadaptor_p.h>
#include <UbuntuMetrics/applicationmonitor.h>
#include <QtGui/QTouchDevice>
#include <QtQml/qqml.h>

static QObject *s_testRootObject = 0;
static QObject *testRootObject(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(jsEngine);
    if (!s_testRootObject) {
        s_testRootObject = new QObject(engine);
    }
    return s_testRootObject;
}

int main(int argc, const char *argv[])
{
    // QPlatformIntegration::ThreadedOpenGL
    setenv("QML_FORCE_THREADED_RENDERER", "1", 1);
    // QPlatformIntegration::BufferQueueingOpenGL
    setenv("QML_FIXED_ANIMATION_STEP", "1", 1);
    // Oxide and QWebEngine need a shared context
    QScopedPointer<QOpenGLContext> shareContext;
    shareContext.reset(new QOpenGLContext);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    qt_gl_set_global_share_context(shareContext.data());
#elif QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    QOpenGLContextPrivate::setGlobalShareContext(shareContext.data());
#else
    QSGContext::setSharedOpenGLContext(shareContext.data());
#endif
    QGuiApplication::setApplicationName("UITK Launcher");
    QGuiApplication application(argc, (char**)argv);

    QCommandLineParser args;
    QCommandLineOption _import("I", "Add <path> to the list of import paths", "path");
    QCommandLineOption _enableTouch("touch", "Enables mouse to touch conversion on desktop");
    QCommandLineOption _testability("testability", "Loads the testability driver");
    QCommandLineOption _frameless("frameless", "Run without borders");
    QCommandLineOption _engine("engine", "Use quick engine from quick view");
    QCommandLineOption _desktop_file_hint("desktop_file_hint", "Desktop file - ignored", "desktop_file");
    QCommandLineOption _metricsOverlay("metrics-overlay", "Enable the metrics overlay");
    QCommandLineOption _metricsLogging(
        "metrics-logging", "Enable metrics logging, <device> can be 'stdout', 'lttng', a local or "
        "absolute filename", "device");
    QCommandLineOption _metricsLoggingFilter(
        "metrics-logging-filter", "Filter metrics logging, <filter> is a list of events separated "
        "by a comma ('window', 'process', 'frame' or '*'), events not filtered are discarded",
        "filter");

    args.addOption(_import);
    args.addOption(_enableTouch);
    args.addOption(_testability);
    args.addOption(_frameless);
    args.addOption(_engine);
    args.addOption(_desktop_file_hint);
    args.addOption(_metricsOverlay);
    args.addOption(_metricsLogging);
    args.addOption(_metricsLoggingFilter);
    args.addPositionalArgument("filename", "Document to be viewed");
    args.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    args.addHelpOption();
    if (!args.parse(application.arguments())) {
        qWarning() << args.errorText();
        args.showHelp(1);
    }

    QString filename;
    if (args.positionalArguments().count() > 0) {
        filename = args.positionalArguments()[0];
    }
    if (filename.isEmpty()) {
        // show usage and exit
        args.showHelp(1);
    }

    // Testability is only supported out of the box by QApplication not QGuiApplication
    if (args.isSet(_testability) || getenv("QT_LOAD_TESTABILITY")) {
        QLibrary testLib(QLatin1String("qttestability"));
        if (testLib.load()) {
            typedef void (*TasInitialize)(void);
            TasInitialize initFunction = (TasInitialize)testLib.resolve("qt_testability_init");
            if (initFunction) {
                initFunction();
            } else {
                qCritical("Library qttestability resolve failed!");
                return 1;
            }
        } else {
            qCritical("Library qttestability load failed!");
            return 1;
        }
    }

    // Allow manual execution of unit tests using Qt.Test
    qmlRegisterSingletonType<QObject>("Qt.test.qtestroot", 1, 0, "QTestRootObject", testRootObject);

    QPointer<QQmlEngine> engine;
    QScopedPointer<QQuickWindow> window;
    QString testCaseImport;
    // Let's see if the source file exists
    QFile sourceCode(filename);
    if (!sourceCode.open(QIODevice::ReadOnly)) {
        qCritical("%s", qPrintable(sourceCode.errorString()));
        return 1;
    }
    while (!sourceCode.atEnd()) {
        QByteArray line(sourceCode.readLine());
        if (line.contains("{"))
            break;
        // Hack to avoid assertion if QtTest or Ubuntu.Test is used:
        // ASSERT: "QTest::TestLoggers::loggerCount() != 0" in file qtestlog.cpp, line 278
        if ((line.startsWith("import ") && QString(line).split("//")[0].split("/*")[0].contains("Test"))) {
            testCaseImport = line;
            break;
        }
    }
    QUrl source(QUrl::fromLocalFile(filename));

    // The default constructor affects the components tree (autopilot vis)
    if (args.isSet(_engine) || !testCaseImport.isEmpty()) {
        QQuickView *view(new QQuickView());
        engine = view->engine();
        if (args.isSet(_import)) {
            QStringList paths = args.values(_import);
            Q_FOREACH(const QString &path, paths) {
                engine->addImportPath(path);
            }
        }

        view->setSource(source);
        while (view->status() == QQuickView::Loading)
            QCoreApplication::processEvents();
        if (view->errors().count() > 0) {
            args.showHelp(3);
        }
        // An unsupported root object is not technically an error
        if (!view->rootObject()) {
            if (!testCaseImport.isEmpty())
                qCritical("Note: QtTest or Ubuntu.Test was detected here: %s", qPrintable(testCaseImport));
            return 1;
        }

        window.reset(view);
    } else {
        engine = new QQmlEngine();
        if (args.isSet(_import)) {
            QStringList paths = args.values(_import);
            Q_FOREACH(const QString &path, paths) {
                engine->addImportPath(path);
            }
        }

        QObject::connect(engine, SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));
        QPointer<QQmlComponent> component(new QQmlComponent(engine));
        component->loadUrl(source, QQmlComponent::Asynchronous);
        while (component->isLoading())
            QCoreApplication::processEvents();
        QObject *toplevel(component->create());
        if (!toplevel && component->isError()) {
            qCritical("%s", qPrintable(component->errorString()));
            return 1;
        }

        window.reset(qobject_cast<QQuickWindow *>(toplevel));
        if (window)
            engine->setIncubationController(window->incubationController());
        else {
            QQuickItem *rootItem = qobject_cast<QQuickItem *>(toplevel);
            if (rootItem) {
                QQuickView *view(new QQuickView(engine, 0));
                window.reset(view);
                view->setResizeMode(QQuickView::SizeRootObjectToView);
                view->setContent(source, component, rootItem);
            }
        }
    }

    // Application monitoring.
    UMApplicationMonitor* applicationMonitor = UMApplicationMonitor::instance();
    if (args.isSet(_metricsLoggingFilter)) {
        QStringList filterList = QString(args.value(_metricsLoggingFilter)).split(
            QChar(','), QString::SkipEmptyParts);
        UMApplicationMonitor::LoggingFilters filter = 0;
        const int size = filterList.size();
        for (int i = 0; i < size; ++i) {
            if (filterList[i] == "*") {
                filter |= UMApplicationMonitor::AllEvents;
                break;
            } else if (filterList[i] == "window") {
                filter |= UMApplicationMonitor::WindowEvent;
            } else if (filterList[i] == "process") {
                filter |= UMApplicationMonitor::ProcessEvent;
            } else if (filterList[i] == "frame") {
                filter |= UMApplicationMonitor::FrameEvent;
            } else if (filterList[i] == "generic") {
                filter |= UMApplicationMonitor::GenericEvent;
            }
        }
        applicationMonitor->setLoggingFilter(filter);
    }
    if (args.isSet(_metricsLogging)) {
        UMLogger* logger;
        QString device = args.value(_metricsLogging);
        if (device.isEmpty() || device == "stdout") {
            logger = new UMFileLogger(stdout);
        } else if (device == "lttng") {
            logger = new UMLTTNGLogger();
        } else {
            logger = new UMFileLogger(device);
        }
        if (logger->isOpen()) {
            applicationMonitor->installLogger(logger);
            applicationMonitor->setLogging(true);
        } else {
            delete logger;
        }
    }
    if (args.isSet(_metricsOverlay)) {
        applicationMonitor->setOverlay(true);
    }

    if (window->title().isEmpty())
        window->setTitle("UI Toolkit QQuickView");
    if (args.isSet(_frameless)) {
        window->setFlags(Qt::FramelessWindowHint);
    }
    window->show();

    if (args.isSet(_enableTouch)) {
        // has no effect if we have touch screen
        new UT_PREPEND_NAMESPACE(MouseTouchAdaptor)(&application);
    }

    return application.exec();
}
