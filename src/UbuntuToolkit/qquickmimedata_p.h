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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef QQUICKMIMEDATA_H
#define QQUICKMIMEDATA_H

#include <QtCore/QMimeData>
#include <QtCore/QStringList>
#include <QtGui/QColor>
#include <QtCore/QUrl>
#include "qquickclipboard_p.h"

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT QQuickMimeData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList formats READ formats NOTIFY formatsChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)
    Q_PROPERTY(QList<QUrl> urls READ urls WRITE setUrls NOTIFY urlsChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QVariant data READ mimeData WRITE setMimeData NOTIFY dataChanged)
public:
    explicit QQuickMimeData(QObject *parent = 0);
    QQuickMimeData(const QMimeData *refData, bool localData = true, QObject *parent = 0);
    ~QQuickMimeData();

    QStringList formats() const;
    QString text() const;
    void setText(const QString &text);
    QString html() const;
    void setHtml(const QString &html);
    QList<QUrl> urls() const;
    void setUrls(const QList<QUrl> &urls);
    QColor color() const;
    void setColor(const QColor &color);
    QVariant mimeData() const;
    void setMimeData(const QVariant &mimeData);
    
    void fromMimeData(const QMimeData *data);
    QMimeData *toMimeData();

Q_SIGNALS:
    void formatsChanged();
    void textChanged();
    void htmlChanged();
    void urlsChanged();
    void colorChanged();
    void typeChanged();
    void dataChanged();
    
private:
    friend class QQuickClipboard;

    bool m_refData;
    QMimeData *m_mimeData;
};

UT_NAMESPACE_END

#endif // QQUICKMIMEDATA_H
