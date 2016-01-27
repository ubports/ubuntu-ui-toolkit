/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcustomintegration.h"
#include "qcustombackingstore.h"

#include <QtGui/private/qguiapplication_p.h>
#include <qpa/qplatformwindow.h>
#include <qpa/qplatformfontdatabase.h>

#include <QtPlatformSupport/private/qgenericunixeventdispatcher_p.h>

static const char devicePixelRatioEnvironmentVariable[] = "QT_DEVICE_PIXEL_RATIO";

QCustomScreen::QCustomScreen()
    : mDepth(32), mFormat(QImage::Format_ARGB32_Premultiplied), mDpr(1.0)
{
    if (qEnvironmentVariableIsSet(devicePixelRatioEnvironmentVariable)) {
        bool ok = false;
        const float dpr = qgetenv(devicePixelRatioEnvironmentVariable).toFloat(&ok);
        if (ok && dpr > 0) {
            mDpr = dpr;
        }
    }
}

QCustomIntegration::QCustomIntegration()
{
    QCustomScreen *mPrimaryScreen = new QCustomScreen();

    mPrimaryScreen->mGeometry = QRect(0, 0, 240, 320);
    mPrimaryScreen->mDepth = 32;
    mPrimaryScreen->mFormat = QImage::Format_ARGB32_Premultiplied;

    screenAdded(mPrimaryScreen);
}

bool QCustomIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    case MultipleWindows: return true;
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

// Dummy font database that does not scan the fonts directory to be
// used for command line tools like qmlplugindump that do not create windows
// unless DebugBackingStore is activated.
class DummyFontDatabase : public QPlatformFontDatabase
{
public:
    void populateFontDatabase() override {}
};

QPlatformFontDatabase *QCustomIntegration::fontDatabase() const
{
    return new DummyFontDatabase;
}


QPlatformWindow *QCustomIntegration::createPlatformWindow(QWindow *window) const
{
    QPlatformWindow *w = new QPlatformWindow(window);
    w->requestActivateWindow();
    return w;
}

QPlatformBackingStore *QCustomIntegration::createPlatformBackingStore(QWindow *window) const
{
    return new QCustomBackingStore(window);
}

QAbstractEventDispatcher *QCustomIntegration::createEventDispatcher() const
{
    return createUnixEventDispatcher();
}

QCustomIntegration *QCustomIntegration::instance()
{
    return static_cast<QCustomIntegration *>(QGuiApplicationPrivate::platformIntegration());
}
