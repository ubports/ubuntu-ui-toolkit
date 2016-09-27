/*
 * Copyright 2015 Canonical Ltd.
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

#ifndef RUNE_H
#define RUNE_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QTemporaryFile>
#include <QtCore/QUrl>

class QQuickImageBase;
class UCQQuickImageExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY extendedSourceChanged)
    Q_PROPERTY(QUrl fountain READ source WRITE setSource NOTIFY extendedSourceChanged)

public:
    explicit UCQQuickImageExtension(QObject *parent = 0);

    QUrl source() const;
    virtual void setSource(const QUrl& url);

Q_SIGNALS:
    void extendedSourceChanged();

private:
    QQuickImageBase* m_image;
    QUrl m_source;
    static QHash<QUrl, QSharedPointer<QTemporaryFile> > s_rewrittenSciFiles;
};

#endif // RUNE_H
