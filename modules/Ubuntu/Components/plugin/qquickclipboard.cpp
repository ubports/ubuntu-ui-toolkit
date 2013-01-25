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

#include "qquickclipboard.h"
#include "qquickclipboard_p.h"
#include "qquickmimedata.h"

/*!
 * \qmltype Clipboard
 * \instantiates QQuickClipboard
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief This is a singleton type providing access to the system clipboard.
 *
 * Clipboard data can be retrieved through data property, which returns an object
 * of MimeData type. The actual content of the clipboard can be accessed through
 * this object. Data can be pushed to the clipboard using push() function.
 *
 * The clipboard data cannot be modified through the type returned by the data
 * property, for this a new instance of MimeType type must be used. This instance
 * can be either a standard MimeType component or an object created using newData()
 * function.
 *
 * Examples of use:
 *
 * 1. Using standard MimeType component - the following example copies the selected
 * text from the text area into the clipboard pushing also a color
 * \qml
 * TextArea {
 *     id: editor
 * }
 * \qmlend
 */

QQuickClipboardPrivate::QQuickClipboardPrivate(QQuickClipboard *qq) :
    q_ptr(qq),
    clipboard(QGuiApplication::clipboard()),
    mode(QClipboard::Clipboard),
    data(new QQuickMimeData(clipboard->mimeData(mode), q_ptr))
{
    // connect to the system clipboard's dataChanged signal so we get update
    QObject::connect(clipboard, SIGNAL(dataChanged()), q_ptr, SIGNAL(dataChanged()));
}


QQuickClipboard::QQuickClipboard(QObject *parent) :
    QObject(parent),
    d_ptr(new QQuickClipboardPrivate(this))
{
}

QQuickMimeData *QQuickClipboard::data() const
{
    Q_D(const QQuickClipboard);
    d->data->fromMimeData(d->clipboard->mimeData(d->mode));
    return d->data;
}

QQuickMimeData *QQuickClipboard::newData()
{
    // create it so that we give a QMimeData instance so it won't create a new
    // instance of that when data is pushed tro clipboard
    return new QQuickMimeData(new QMimeData, this);
}

void QQuickClipboard::push(const QVariant& data)
{
    if (!data.isValid())
        return;

    Q_D(QQuickClipboard);
    // check whether we have a MimeData to push
    QQuickMimeData *mimeData = qvariant_cast<QQuickMimeData*>(data);
    if (mimeData)
        d->clipboard->setMimeData(mimeData->toMimeData(), d->mode);
    else {
        QQuickMimeData newData(new QMimeData, true);
        newData.m_refData = false;
        newData.setMimeData(data);
        d->clipboard->setMimeData(newData.m_mimeData, d->mode);
    }
}

void QQuickClipboard::clear()
{
    Q_D(QQuickClipboard);
    d->clipboard->clear(d->mode);
}

#include "moc_qquickclipboard.cpp"
