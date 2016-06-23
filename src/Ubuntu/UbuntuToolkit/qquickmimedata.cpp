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

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtCore/QMimeData>
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>
#include "qquickmimedata.h"

#include <QDebug>

UT_NAMESPACE_BEGIN

/*!
 * \qmltype MimeData
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief MimeData type provides interface to access and store data to the
 * Clipboard.
 *
 */

QQuickMimeData::QQuickMimeData(QObject *parent) :
    QObject(parent),
    m_refData(false),
    m_mimeData(new QMimeData)
{
}
QQuickMimeData::QQuickMimeData(const QMimeData *mimeData, bool refData, QObject *parent) :
    QObject(parent),
    m_refData(refData),
    m_mimeData(const_cast<QMimeData*>(mimeData))
{
}
QQuickMimeData::~QQuickMimeData()
{
    // if the clipboard doesn't own the MimeData yet, delete it
    if (QGuiApplication::clipboard()->mimeData(QClipboard::Clipboard) != m_mimeData)
        delete m_mimeData;
    m_mimeData = 0;
}

/*!
 * \internal
 * Updates the internal MIME data from the clipboard.
 */
void QQuickMimeData::fromMimeData(const QMimeData *data)
{
    // copy
    if (!data || (m_mimeData == data))
        return;
    if (!m_refData)
        delete m_mimeData;
    m_mimeData = const_cast<QMimeData*>(data);
}

/*
 * This function is called when a standalone MimeData instance is passed as parameter to push()
 */
QMimeData *QQuickMimeData::toMimeData()
{
    QMimeData *ret = m_mimeData;
    if (!m_refData) {
        m_mimeData = new QMimeData;
        // copy data so we keep the properties as they were
        Q_FOREACH(const QString &format, ret->formats()) {
            m_mimeData->setData(format, ret->data(format));
        }
        // set teh color separately
        m_mimeData->setColorData(ret->colorData());
    }
    return ret;
}

/*!
 * \qmlproperty list<string> MimeData::format
 * Returns a list of formats supported by the object. This is a list of MIME
 * types for which the object can return suitable data. The formats in the list
 * are in a priority order.
 *
 * \sa data
 */
QStringList QQuickMimeData::formats() const
{
    return (m_mimeData) ? m_mimeData->formats() : QStringList();
}
/*!
 * \qmlproperty string MimeData::text
 * Contains a plain text (MIME type text/plain) representation of the data.
 */
QString QQuickMimeData::text() const
{
    return m_mimeData ? m_mimeData->text() : QString();
}
void QQuickMimeData::setText(const QString &text)
{
    if (!m_refData) {
        m_mimeData->setText(text);
        Q_EMIT textChanged();
    }
}
/*!
 * \qmlproperty string MimeData::html
 * Contains a string if the data stored in the object is HTML (MIME type text/html);
 * otherwise contains an empty string.
 */
QString QQuickMimeData::html() const
{
    return m_mimeData ? m_mimeData->html() : QString();
}
void QQuickMimeData::setHtml(const QString &html)
{
    if (!m_refData) {
        m_mimeData->setHtml(html);
        Q_EMIT htmlChanged();
    }
}
/*!
 * \qmlproperty list<url> MimeData::urls
 * Contains a list of URLs contained within the MIME data object. URLs correspond
 * to the MIME type text/uri-list.
 */
QList<QUrl> QQuickMimeData::urls() const
{
    return m_mimeData ? m_mimeData->urls() : QList<QUrl>();
}
void QQuickMimeData::setUrls(const QList<QUrl> &urls)
{
    if (!m_refData) {
        m_mimeData->setUrls(urls);
        Q_EMIT urlsChanged();
    }
}
/*!
 * \qmlproperty color MimeData::color
 */
QColor QQuickMimeData::color() const
{
    return (m_mimeData) ? qvariant_cast<QColor>(m_mimeData->colorData()) : QColor();
}
void QQuickMimeData::setColor(const QColor &color)
{
    if (!m_refData) {
        m_mimeData->setColorData(color);
        Q_EMIT colorChanged();
    }
}

/*!
 * \qmlproperty var MimeData::data
 * Reading the property returns a list of MIME type and data pairs representing
 * the data stored in the object. Users can set custom MIME types which are not
 * covered by the other properties.
 *
 * When setting the property value can take one of the following types:
 * \list
 * \li - string - the data will be set as \c{text/plain} MIME type, or
 *     as \c{text/html} in case HTML tags are detected
 * \li - color - the data will be set as \c{application/x-color} MIME type
 * \li - list<url> - the data will be set as \c{text/uri-list}
 * \li - list<string> - the data will be set as \c{text/uri-list} in case the
 *     first element of the list is a valid URL with scheme; otherwise if the list
 *     size is even, the list will be set as pairs of (MIME type,data)
 * \li - list<var> - same as list<url> or list<string>
 * \endlist
 */
QVariant QQuickMimeData::mimeData() const
{
    if (!m_mimeData)
        return QVariant();
    QVariantList ret;
    Q_FOREACH(const QString &format, formats()) {
        ret << format;
        ret << QVariant(m_mimeData->data(format));
    }
    return QVariant::fromValue(ret);
}

static bool setMimeType(QMimeData *mimeData, QVariantList &mlist)
{
    bool ret = false;
    QMimeDatabase db;
    for (int i = 0; i < mlist.length() / 2; i++) {
        QString type = mlist[2 * i].toString();
        if (db.mimeTypeForName(type).isValid() || type == "application/x-color") {
            QByteArray data = mlist[2 * i + 1].toByteArray();
            mimeData->setData(type, data);
            ret = true;
        } else if (type == "application/x-color") {
            // for some reason colors are not taken in other way...
            mimeData->setColorData(mlist[2 * i + 1]);
            ret = true;
        }
    }
    return ret;
}

void QQuickMimeData::setMimeData(const QVariant &mimeData)
{
    if (m_refData)
        return;

    QMimeDatabase db;
    bool emitSignal = false;

    if (mimeData.type() == QVariant::List) {
        // the list can be a list of URLs or a list of (type, data) pairs or a
        // plain list formatted so that every 2k element describes a type and
        // every 2k+1 defines the data
        QVariantList mlist= mimeData.toList();
        if (mlist[0].type() == QVariant::List) {
            Q_FOREACH(const QVariant &item, mlist) {
                // should not have more than two elements
                QVariantList block = item.toList();
                if (setMimeType(m_mimeData, block))
                    emitSignal = true;
            }
        } else {
            QUrl url(mlist[0].toUrl());
            if (!url.scheme().isEmpty()) {
                // we have a list or URLs, push them
                QList<QUrl> ulist;
                Q_FOREACH(const QVariant &v, mlist) {
                    ulist << v.toUrl();
                }
                setUrls(ulist);
                emitSignal = true;
            } else if (!(mlist.length() % 2)) {
                // the count of the list is a pair number, so we might have the
                // mime (type,data) pairs
                emitSignal = setMimeType(m_mimeData, mlist);
            }
        }
    } else if (mimeData.type() == QVariant::Color){
        // color must e handled separately
        m_mimeData->setColorData(mimeData);
        emitSignal = true;
    } else {
        // try to detect the mime data within the variant
        // text/plain, text/html and application/x-color types
        QByteArray data = mimeData.toByteArray();
        QMimeType type = db.mimeTypeForData(mimeData.toByteArray());

        if (type.isValid()) {
            m_mimeData->setData(type.name(), data);
            emitSignal = true;
        } else {
            qWarning() << "UNHANDLED" << mimeData;
        }
    }

    if (emitSignal)
        Q_EMIT dataChanged();
}

UT_NAMESPACE_END
