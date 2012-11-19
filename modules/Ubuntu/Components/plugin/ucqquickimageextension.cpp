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

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtQuick/private/qquickimagebase_p.h>

#include "ucqquickimageextension.h"
#include "ucunits.h"

/*!
    \internal

    The UCQQuickImageExtension class makes sure that for images loaded via
    QQuickImageBase the right version is loaded and that they are scaled
    appropriately. It does so by overriding the QQuickImageBase's
    'source' property.
*/
UCQQuickImageExtension::UCQQuickImageExtension(QObject *parent) :
    QObject(parent),
    m_image(static_cast<QQuickImageBase*>(parent)),
    m_tmpSciFile(NULL)
{
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     this, SLOT(reloadSource()), Qt::UniqueConnection);
}

QUrl UCQQuickImageExtension::source() const
{
    return m_source;
}

void UCQQuickImageExtension::setSource(const QUrl& url)
{
    if (url != m_source) {
        m_source = url;
        reloadSource();
        Q_EMIT sourceChanged(m_source);
    }
}

void UCQQuickImageExtension::reloadSource()
{
    if (m_tmpSciFile != NULL) {
        delete m_tmpSciFile;
        m_tmpSciFile = NULL;
    }

    if (m_source.isEmpty()) {
        m_image->setSource(m_source);
        return;
    }

    QString resolved = UCUnits::instance().resolveResource(m_source);

    if (resolved.isEmpty()) {
        m_image->setSource(m_source);
        return;
    }

    int separatorPosition = resolved.indexOf("/");
    QString scaleFactor = resolved.left(separatorPosition);
    QString selectedFilePath = resolved.mid(separatorPosition+1);

    if (scaleFactor == "1") {
        // No scaling. Just pass the file as is.
        m_image->setSource(QUrl::fromLocalFile(selectedFilePath));
    } else {
        // Prepend "image://scaling" for the image to be loaded by UCScalingImageProvider.
        if (!m_source.path().endsWith(".sci")) {
            // Regular image file
            m_image->setSource(QUrl("image://scaling/" + resolved));
        } else {
            // .sci image file. Rewrite the .sci file into a temporary file.
            m_tmpSciFile = new QTemporaryFile(this);
            m_tmpSciFile->setFileTemplate(QDir::tempPath() + QDir::separator() + "XXXXXX.sci");
            m_tmpSciFile->open();
            QTextStream output(m_tmpSciFile);
            bool rewritten = rewriteSciFile(selectedFilePath, scaleFactor, output);
            m_tmpSciFile->close();
            if (rewritten) {
                m_image->setSource(QUrl::fromLocalFile(m_tmpSciFile->fileName()));
            } else {
                m_image->setSource(m_source);
            }
        }
    }
}

bool UCQQuickImageExtension::rewriteSciFile(QString sciFilePath, QString scaleFactor, QTextStream& output)
{
    QFile sciFile(sciFilePath);
    if (sciFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream sciStream(&sciFile);
        while (!sciStream.atEnd()) {
            QString line = sciStream.readLine();
            if (line.contains("border")) {
                output << scaledBorder(line, scaleFactor) << endl;
            } else if (line.contains("source")) {
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

QString UCQQuickImageExtension::scaledBorder(QString border, QString scaleFactor)
{
    // Rewrite the border line with a scaled border value
    QStringList parts = border.split(":");
    float scaledValue = parts[1].toFloat() * scaleFactor.toFloat();
    return parts[0] + ": " + QString::number(qRound(scaledValue));
}

QString UCQQuickImageExtension::scaledSource(QString source, QString sciFilePath, QString scaleFactor)
{
    // Rewrite the source line by prepending "image://scaling" to the source value
    QString sciDirectory = QFileInfo(sciFilePath).dir().path() + QDir::separator();
    QString baseUrl = "image://scaling/" + scaleFactor + "/" + sciDirectory;
    return source.replace("source: ", "source: " + baseUrl);
}
