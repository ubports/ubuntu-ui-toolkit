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

class QQuickImageBase;
class UCQQuickImageExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit UCQQuickImageExtension(QObject *parent = 0);

    QUrl source() const;
    virtual void setSource(const QUrl& url);

Q_SIGNALS:
    void sourceChanged(const QUrl &);

protected Q_SLOTS:
    void reloadSource();

protected:
    bool rewriteSciFile(QString sciFilePath, QString scaleFactor, QTextStream& output);
    QString scaledBorder(QString border, QString scaleFactor);
    QString scaledSource(QString source, QString sciFilePath, QString scaleFactor);

private:
    QQuickImageBase* m_image;
    QUrl m_source;
    QTemporaryFile* m_tmpSciFile;
};

#endif // QQUICKIMAGEEXTENSION_H
