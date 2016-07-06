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

#ifndef QQUICKIMAGEEXTENSION_H
#define QQUICKIMAGEEXTENSION_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>
#include <QtCore/QTemporaryFile>
#include <QtCore/QSharedPointer>
#include "ubuntutoolkitglobal.h"

class QQuickImageBase;

namespace UbuntuToolkit {

class UBUNTUTOOLKIT_EXPORT UCQQuickImageExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY extendedSourceChanged)

public:
    explicit UCQQuickImageExtension(QObject *parent = 0);

    QUrl source() const;
    virtual void setSource(const QUrl& url);

Q_SIGNALS:
    void extendedSourceChanged();

protected Q_SLOTS:
    void reloadSource();

protected:
    bool rewriteSciFile(const QString &sciFilePath, const QString &scaleFactor, QTextStream& output);
    QString scaledBorder(const QString &border, const QString &scaleFactor);
    QString scaledSource(QString source, const QString &sciFilePath, const QString &scaleFactor);

private:
    QQuickImageBase* m_image;
    QUrl m_source;
    static QHash<QUrl, QSharedPointer<QTemporaryFile> > s_rewrittenSciFiles;
};

UT_NAMESPACE_END

#endif // QQUICKIMAGEEXTENSION_H
