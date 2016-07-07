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

#ifndef QQUICKCLIPBOARD_H
#define QQUICKCLIPBOARD_H

#include <QtCore/QObject>
#include <QtQml/QJSEngine>
#include <QtQml/QQmlEngine>
#include <ubuntutoolkitglobal.h>

class QQmlEngine;
class QJSEngine;

namespace UbuntuToolkit {

class QQuickMimeData;
class QQuickClipboardPrivate;
class UBUNTUTOOLKIT_EXPORT QQuickClipboard : public QObject
{
    Q_OBJECT
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(QQuickMimeData) *data READ data NOTIFY dataChanged)
#else
    Q_PROPERTY(QQuickMimeData *data READ data NOTIFY dataChanged)
#endif
public:
    explicit QQuickClipboard(QObject *parent = 0);

    Q_INVOKABLE UT_PREPEND_NAMESPACE(QQuickMimeData) *newData();

private: //getter/setter
    QQuickMimeData *data();
    
Q_SIGNALS:
    void dataChanged();
    
public Q_SLOTS:
    void push(const QVariant& data);
    void clear();

private:
    Q_DECLARE_PRIVATE(QQuickClipboard)
    QScopedPointer<QQuickClipboardPrivate> d_ptr;
};

UT_NAMESPACE_END

#endif // QQUICKCLIPBOARD_H
