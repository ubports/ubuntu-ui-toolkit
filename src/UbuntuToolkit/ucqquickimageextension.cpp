/*
 * Copyright 2012 Canonical Ltd.
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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "ucqquickimageextension_p.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickimagebase_p.h>
#include <QtQuick/private/qquickpixmapcache_p.h>

#define foreach Q_FOREACH
#include <QtQml/private/qqmlengine_p.h>
#undef foreach

#include "ucunits_p.h"

UT_NAMESPACE_BEGIN

QHash<QUrl, QSharedPointer<QTemporaryFile> > UCQQuickImageExtension::s_rewrittenSciFiles;

/*!
    \internal

    The UCQQuickImageExtension class makes sure that for images loaded via
    QQuickImageBase the right version is loaded and that they are scaled
    appropriately. It does so by overriding the QQuickImageBase's
    'source' property.
*/
UCQQuickImageExtension::UCQQuickImageExtension(QObject *parent) :
    QObject(parent),
    m_image(static_cast<QQuickImageBase*>(parent))
{
    // FIXME(loicm) When the grid unit is changed following a QPA plugin scale
    //     notification, we experienced a crash in reloadSource() while setting
    //     the QQuickImageBase source. It seems like a resource handling issue
    //     in Qt but we have not managed to identify it exactly. We work around
    //     it by using a queued connection for gridUnitChanged() signal for now.
    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     this, SLOT(reloadSource()), Qt::QueuedConnection);

    if (m_image) {
        QObject::connect(m_image, &QQuickImageBase::sourceChanged,
                         this, &UCQQuickImageExtension::extendedSourceChanged);
    }
}

QUrl UCQQuickImageExtension::source() const
{
    return m_source;
}


void UCQQuickImageExtension::setSource(const QUrl& url)
{
    if (url != m_source) {
        m_source = url;
        // We need to wait until the component is complete
        // so that m_image->sourceSize() is actually valid
        if (QQuickItemPrivate::get(m_image)->componentComplete) {
            reloadSource();
        } else {
            // This is a bit convoluted but i couldn't find a better way to get notified of when
            // the image actually finishes constructing.
            // Since what we're interested in reloadSource() is the image having the sourceSize set,
            // what we do is connect to the sourceSizeChanged signal.
            // The problem is that this signal isn't fired if the Image {} doesn't have sourceSize set
            // so we tell the engine to fire the sourceSizeChanged signal when the image finishes constructing
            // This way if the Image {} has a sourceSize set the lambda gets called because of it
            // and if there's no sourceSize set the lambda gets called because we registered the finalize callback

            // WARNING do not convert this to a "modern-style" connect, i.e. &QQuickImageBase::sourceSizeChanged,
            // it will break if m_image is a QQuickBorderImage since it redeclares the sourceSizeChanged
            // See https://codereview.qt-project.org/#/c/187967/
            connect(m_image, SIGNAL(sourceSizeChanged()), this, SLOT(onSourceSizeChanged()));
            QQmlEnginePrivate *engPriv = QQmlEnginePrivate::get(qmlEngine(m_image));
            engPriv->registerFinalizeCallback(m_image, m_image->metaObject()->indexOfSignal("sourceSizeChanged()"));
        }
    }
}

void UCQQuickImageExtension::onSourceSizeChanged()
{
    // See WARNING above
    QObject::disconnect(m_image, SIGNAL(sourceSizeChanged()), this, SLOT(onSourceSizeChanged()));
    reloadSource();
}

void UCQQuickImageExtension::reloadSource()
{
    if (!m_image) {
        // nothing to do, we don't have the image instance
        return;
    }
    if (m_source.isEmpty()) {
        m_image->setSource(m_source);
        return;
    }

    // If the url we're trying to load is already in the cache and
    // the devicePixelRatio is 1, we save calling UCUnits::resolveResource
    // and just set that image directly.
    // UCUnits::resolveResource is not cheap (does a stat on disk)
    if (qFuzzyCompare(qGuiApp->devicePixelRatio(), (qreal)1.0)) {
        QSize ss = m_image->sourceSize();
        if (ss.isNull() && m_image->image().isNull()) {
            // For some reason QQuickImage returns 0x0 as sourceSize
            // when the sourceSize is not set (and the image has not yet been loaded)
            // so set it back to -1x-1
            ss = QSize(-1, -1);
        }

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        if (QQuickPixmap::isCached(m_source, QRect(), ss, 0, QQuickImageProviderOptions())) {
#elif QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        if (QQuickPixmap::isCached(m_source, ss, 0, QQuickImageProviderOptions())) {
#else
        if (QQuickPixmap::isCached(m_source, ss, QQuickImageProviderOptions())) {
#endif
            m_image->setSource(m_source);
            return;
        }
    }

    QString resolved = UCUnits::instance()->resolveResource(m_source);

    if (resolved.isEmpty()) {
        m_image->setSource(m_source);
        return;
    }

    int separatorPosition = resolved.indexOf(QStringLiteral("/"));
    QString scaleFactor = resolved.left(separatorPosition);
    QString selectedFilePath = resolved.mid(separatorPosition+1);
    QString fragment(m_source.hasFragment() ? "#" + m_source.fragment() : QStringLiteral(""));

    if (scaleFactor == QStringLiteral("1")) {
        if (qFuzzyCompare(qGuiApp->devicePixelRatio(), (qreal)1.0)
            || selectedFilePath.endsWith(QStringLiteral(".svg"))
            || selectedFilePath.endsWith(QStringLiteral(".svgz"))) {
            // Take care to pass the original fragment
            QUrl selectedFileUrl(QUrl::fromLocalFile(selectedFilePath));
            if (m_source.hasFragment()) {
                selectedFileUrl.setFragment(fragment);
            }
            m_image->setSource(selectedFileUrl);
        } else {
            // Need to scale the pixel-based image to suit the devicePixelRatio setting ourselves.
            // If we let Qt do it, Qt will not choose the UITK-supported "@gu" scaled images.
            m_image->setSource(QUrl("image://scaling/1/" + selectedFilePath + fragment));
            // explicitly set the source size in the QQuickImageBase, this persuades it that the
            // supplied image is suitable for the current devicePixelRatio.
            m_image->setSourceSize(m_image->sourceSize());
        }
    } else {
        // Prepend "image://scaling" for the image to be loaded by UCScalingImageProvider.
        if (!m_source.path().endsWith(QStringLiteral(".sci"))) {
          // Regular image file
            m_image->setSource(QUrl("image://scaling/" + resolved + fragment));
        } else {
            // .sci image file. Rewrite the .sci file into a temporary file.
            bool rewritten = true;
            QTemporaryFile* rewrittenSciFile;

            /* Ensure that only one temporary rewritten .sci file is created
               for each source .sci file by storing the path to the temporary
               file in a global hash.
            */
            rewrittenSciFile = UCQQuickImageExtension::s_rewrittenSciFiles.value(m_source).data();
            if (rewrittenSciFile == NULL) {
                rewrittenSciFile = new QTemporaryFile;
                rewrittenSciFile->setFileTemplate(QDir::tempPath() + "/XXXXXX.sci");
                rewrittenSciFile->open();
                QTextStream output(rewrittenSciFile);

                if (qFuzzyCompare(qGuiApp->devicePixelRatio(), (qreal)1.0)) {
                    rewritten = rewriteSciFile(selectedFilePath, scaleFactor, output);
                } else {
                    QString scaleFactorInDevicePixels = QString::number(scaleFactor.toFloat() / qGuiApp->devicePixelRatio());
                    rewritten = rewriteSciFile(selectedFilePath, scaleFactorInDevicePixels, output);
                }
                rewrittenSciFile->close();

                s_rewrittenSciFiles.insert(m_source, QSharedPointer<QTemporaryFile>(rewrittenSciFile));
            }

            if (rewritten) {
                // Take care to pass the original fragment
                QUrl rewrittenSciFileUrl(QUrl::fromLocalFile(rewrittenSciFile->fileName()));
                rewrittenSciFileUrl.setFragment(fragment);
                m_image->setSource(rewrittenSciFileUrl);
            } else {
                m_image->setSource(m_source);
            }
        }
        // explicitly set the source size in the QQuickImageBase, this persuades it that the
        // supplied image is suitable for the current devicePixelRatio.
        m_image->setSourceSize(m_image->sourceSize());
    }
}

bool UCQQuickImageExtension::rewriteSciFile(const QString &sciFilePath, const QString &scaleFactor, QTextStream& output)
{
    QFile sciFile(sciFilePath);
    if (sciFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream sciStream(&sciFile);
        while (!sciStream.atEnd()) {
            QString line = sciStream.readLine();
            if (line.startsWith(QStringLiteral("border"))) {
                output << scaledBorder(line, scaleFactor) << endl;
            } else if (line.startsWith(QStringLiteral("source"))) {
                output << scaledSource(line, sciFilePath, scaleFactor) << endl;
            } else {
                output << line << endl;
            }
        }
        return true;
    } else {
        return false;
    }
}

QString UCQQuickImageExtension::scaledBorder(const QString &border, const QString &scaleFactor)
{
    // Rewrite the border line with a scaled border value
    QStringList parts = border.split(QStringLiteral(":"));
    float scaledValue = parts[1].toFloat() * scaleFactor.toFloat();
    return parts[0] + ": " + QString::number(qRound(scaledValue));
}

QString UCQQuickImageExtension::scaledSource(QString source, const QString &sciFilePath, const QString &scaleFactor)
{
    // Rewrite the source line by prepending "image://scaling" to the source value
    QString sciDirectory = QFileInfo(sciFilePath).dir().path() + "/";
    QString baseUrl = "image://scaling/" + scaleFactor + "/" + sciDirectory;

    // If the source url is between quotes "", remove them
    const QChar quote = '"';
    const int quoteFirstIndex = source.indexOf(quote);
    if (quoteFirstIndex != -1) {
        source.remove(quoteFirstIndex, 1);
    }
    const int quoteLastIndex = source.lastIndexOf(quote);
    if (quoteLastIndex != -1) {
        source.remove(quoteLastIndex, 1);
    }

    return source.replace(
        QStringLiteral("source: "), "source: " + QString(quote) + baseUrl).append(quote);
}

UT_NAMESPACE_END
