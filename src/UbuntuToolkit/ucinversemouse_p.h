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

#include "ucmouse_p.h"
#include <QtQml>
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCInverseMouse : public UCMouse {
    Q_OBJECT
public:
    explicit UCInverseMouse(QObject *parent = 0);

    static UCInverseMouse *qmlAttachedProperties(QObject *owner);

    void setEnabled(bool enabled) override;
    void setPriority(Priority priority) override;
    bool excludeInputArea() const;
    void setExcludeInputArea(bool value);

protected:
    QMouseEvent mapMouseToOwner(QObject *target, QMouseEvent* event);
    QHoverEvent mapHoverToOwner(QObject *target, QHoverEvent *event);
    bool eventFilter(QObject *, QEvent *) override;
    bool mouseEvents(QObject *target, QMouseEvent *event) override;
    bool hoverEvents(QObject *target, QHoverEvent *event) override;
    bool hasAttachedFilter(QQuickItem *item) override;
    bool pointInOSK(const QPointF &point);
    bool contains(QMouseEvent *mouse);
};

UT_NAMESPACE_END

QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(UCInverseMouse))
QML_DECLARE_TYPEINFO(UT_PREPEND_NAMESPACE(UCInverseMouse), QML_HAS_ATTACHED_PROPERTIES)

#endif // UCINVERSEMOUSE_H
