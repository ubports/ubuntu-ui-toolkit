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
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief This is a singleton type providing access to the system clipboard.
 *
 * Clipboard data can be retrieved through data property, which returns an object
 * of MimeData type. The actual content of the clipboard can be accessed through
 * this object. Data can be pushed to the clipboard using push() function.
 *
 * The clipboard data cannot be modified through the type returned by the data
 * property, for this a new instance of MimeData type must be used. This instance
 * can be either a standalone MimeDala component or an object created using newData()
 * function. Remember that standalone MimeData types duplicate the clipboard data
 * which may cause extensive memory use.
 *
 * Examples of use:
 *
 * 1. Using standard MimeType component - the following example copies the selected
 * text from the text area into the clipboard pushing also a color
 * \qml
 * Item {
 *     TextArea {
 *         id: editor
 *     }
 *
 *     MimeData {
 *         id: mimeData
 *         color: "green"
 *         text: editor.text
 *     }
 *
 *     Button {
 *         text: "Copy"
 *         onClicked: Clipboard.push(mimeData)
 *     }
 * }
 * \endqml
 *
 * 2. Pushing data straight to clipboard
 * \qml
 * Item {
 *     TextArea {
 *         id: editor
 *     }
 *
 *     Button {
 *         text: "Copy"
 *         onClicked: {
 *             Clipboard.push(editor.text);
 *             Clipboard.push(["application/x-color", "green"]);
 *         }
 *     }
 * }
 * \endqml
 *
 * 3. Pushing data using MimeData object
 * \qml
 * Item {
 *     TextArea {
 *         id: editor
 *     }
 *
 *     Button {
 *         text: "Copy"
 *         onClicked: {
 *             var mimeData = Clipboard.newData();
 *             mimeData.text = editor.text;
 *             mimeData.color = "green";
 *             Clipboard.push(mimeData);
 *         }
 *     }
 * }
 * \endqml
 *
 */

/*!
 * \qmlsignal Clipboard::dataChanged()
 * The signal is triggered when clipboard content gets changed.
 */

QQuickClipboardPrivate::QQuickClipboardPrivate(QQuickClipboard *qq) :
    q_ptr(qq),
    clipboard(QGuiApplication::clipboard()),
    mode(QClipboard::Clipboard),
    mimeData(0)
{
    // connect to the system clipboard's dataChanged signal so we get update
    QObject::connect(clipboard, SIGNAL(dataChanged()), q_ptr, SIGNAL(dataChanged()));
}

void QQuickClipboardPrivate::updateMimeData()
{
    const QMimeData *data = clipboard->mimeData(mode);
    if (!mimeData)
        mimeData = new QQuickMimeData(clipboard->mimeData(mode), true, q_ptr);
    else
        mimeData->fromMimeData(data);
}


QQuickClipboard::QQuickClipboard(QObject *parent) :
    QObject(parent),
    d_ptr(new QQuickClipboardPrivate(this))
{
}

/*!
 * \qmlproperty MimeData Clipboard::data
 * Provides access to the Clipboard's current data.
 */
QQuickMimeData *QQuickClipboard::data()
{
    Q_D(QQuickClipboard);
    d->updateMimeData();
    return d->mimeData;
}

/*!
 * \qmlmethod MimeData Clipboard::newData()
 * The function returns a new MimeData object that can be used in Java script code
 * to push pultiple MIME types at the same time.
 */
QQuickMimeData *QQuickClipboard::newData()
{
    // create it so that we give a QMimeData instance so it won't create a new
    // instance of that when data is pushed to clipboard
    return new QQuickMimeData(new QMimeData, false, this);
}

/*!
 * \qmlmethod Clipboard::push(var data)
 * The function copies data provided as parameter to the clipboard. The parameter
 * can be a MimeData instance or object created using newData() beside
 * the ones listed at MimeData::data.
 *
 * When MimeData instance or object is given, the entire object content will be
 * pushed to the clipboard.
 *
 */
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
        QQuickMimeData newData(new QMimeData, false);
        if (data.userType() == qMetaTypeId<QJSValue>()) {
            newData.setMimeData(data.value<QJSValue>().toVariant());
        } else {
            newData.setMimeData(data);
        }
        d->clipboard->setMimeData(newData.m_mimeData, d->mode);
    }
}

/*!
 * \qmlmethod Clipboard::clear()
 * The function clears the clipboard content.
 */
void QQuickClipboard::clear()
{
    Q_D(QQuickClipboard);
    d->clipboard->clear(d->mode);
}

#include "moc_qquickclipboard.cpp"
