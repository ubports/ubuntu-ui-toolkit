/*
 * Copyright 2014 Canonical Ltd.
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
 */

#ifndef UCINVERSEMOUSE_H
#define UCINVERSEMOUSE_H

#include "ucmouse.h"
#include <QtQml>

class UCInverseMouse : public UCMouse {
    Q_OBJECT
public:
    explicit UCInverseMouse(QObject *parent = 0);

    static UCInverseMouse *qmlAttachedProperties(QObject *owner);

    virtual void setEnabled(bool enabled);
    virtual void setPriority(Priority priority);
    bool excludeInputArea() const;
    void setExcludeInputArea(bool value);

protected:
    QMouseEvent mapMouseToOwner(QObject *target, QMouseEvent* event);
    QHoverEvent mapHoverToOwner(QObject *target, QHoverEvent *event);
    virtual bool eventFilter(QObject *, QEvent *);
    virtual bool mouseEvents(QObject *target, QMouseEvent *event);
    virtual bool hoverEvents(QObject *target, QHoverEvent *event);
    virtual bool hasAttachedFilter(QQuickItem *item);
    bool pointInOSK(const QPointF &point);
    bool contains(QMouseEvent *mouse);
};
QML_DECLARE_TYPE(UCInverseMouse)
QML_DECLARE_TYPEINFO(UCInverseMouse, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCINVERSEMOUSE_H
