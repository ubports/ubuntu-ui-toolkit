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

class QQuickMimeData;
class QQmlEngine;
class QJSEngine;
class QQuickClipboardPrivate;
class QQuickClipboard : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickMimeData *data READ data NOTIFY dataChanged)
public:
    explicit QQuickClipboard(QObject *parent = 0);

    Q_INVOKABLE QQuickMimeData *newData();

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

#endif // QQUICKCLIPBOARD_H
