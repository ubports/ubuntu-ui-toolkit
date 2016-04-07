/*
 * Copyright 2014 Canonical Ltd.
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
 * QML launcher with the ability to setup the QQuickView/ QQmlEngine differently
 *
 * Rationale: Different variants of qmlscene exist as well as C++ and Go apps
 * This is to write Autopilot test cases that exhibit specific behavior
 */

#include <iostream>
#include <QtCore/qdebug.h>
#include <QtQuick/QQuickView>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QFileInfo>
#include <QLibrary>
#include <QOpenGLContext>
#include <QtGui/private/qopenglcontext_p.h>
#include <QtQuick/private/qsgcontext_p.h>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <MouseTouchAdaptor>
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

    args.addOption(_import);
    args.addOption(_enableTouch);
    args.addOption(_testability);
    args.addOption(_frameless);
    args.addOption(_engine);
    args.addOption(_desktop_file_hint);
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

    QQmlEngine* engine;
    // The default constructor affects the components tree (autopilot vis)
    QScopedPointer<QQuickView> view;
    if (args.isSet(_engine)) {
        view.reset(new QQuickView());
        engine = view->engine();
    } else {
        engine = new QQmlEngine();
        view.reset(new QQuickView(engine, NULL));
        engine->setParent(view.data());
    }

    if (args.isSet(_import)) {
        QStringList paths = args.values(_import);
        Q_FOREACH(const QString &path, paths) {
            engine->addImportPath(path);
        }
    }

    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->setTitle("UI Toolkit QQuickView");
    if (args.isSet(_frameless)) {
        view->setFlags(Qt::FramelessWindowHint);
    }

    if (args.isSet(_enableTouch)) {
        // has no effect if we have touch screen
        new UbuntuToolkit::MouseTouchAdaptor(&application);
    }

    QUrl source(QUrl::fromLocalFile(filename));
    view->setSource(source);
    if (view->errors().count() > 0) {
        args.showHelp(3);
    }
    view->show();

    return application.exec();
}

