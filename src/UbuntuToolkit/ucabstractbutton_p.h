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
 */

#ifndef UCABSTRACTBUTTON_H
#define UCABSTRACTBUTTON_H

#include "ucactionitem_p.h"
#include "ucmargins_p.h"
#include <QtQuick/private/qquickevents_p_p.h>

class QQuickMouseArea;
class QQuickMouseEvent;

UT_NAMESPACE_BEGIN

class UCAbstractButtonPrivate;
class UBUNTUTOOLKIT_EXPORT UCAbstractButton : public UCActionItem
{
    Q_OBJECT
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool hovered READ hovered NOTIFY hoveredChanged)
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCMargins) *sensingMargins READ sensingMargins CONSTANT FINAL)
#else
    Q_PROPERTY(UCMargins *sensingMargins READ sensingMargins CONSTANT FINAL)
#endif

    // internal, declared to support the deprecated ListItem module
    Q_PROPERTY(bool __acceptEvents READ acceptEvents WRITE setAcceptEvents)
    Q_PROPERTY(QQuickMouseArea *__mouseArea READ privateMouseArea CONSTANT)
public:
    explicit UCAbstractButton(QQuickItem *parent = 0);

    bool pressed() const;
    bool hovered() const;
    UCMargins *sensingMargins();

    bool privateAcceptEvents() const;
    void setPrivateAcceptEvents(bool accept);
    bool acceptEvents() const;
    void setAcceptEvents(bool value);
    QQuickMouseArea *privateMouseArea() const;

    // override containment check
    bool contains(const QPointF &point) const override;

protected:
    void classBegin() override;
    void geometryChanged(const QRectF &newGeometry,
                         const QRectF &oldGeometry) override;
    void keyReleaseEvent(QKeyEvent *key) override;
    void touchEvent(QTouchEvent *event) override;

Q_SIGNALS:
    void pressedChanged();
    void hoveredChanged();
    void clicked();
    void pressAndHold();

protected:
    UCAbstractButton(UCAbstractButtonPrivate &&, QQuickItem *parent = 0);

    Q_DECLARE_PRIVATE(UCAbstractButton)
    Q_PRIVATE_SLOT(d_func(), void _q_mouseAreaPressed())
    Q_PRIVATE_SLOT(d_func(), void _q_mouseAreaClicked())
    Q_PRIVATE_SLOT(d_func(), void _q_mouseAreaPressAndHold())
    Q_PRIVATE_SLOT(d_func(), void _q_adjustSensingArea())
};

UT_NAMESPACE_END

QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(UCMargins))

#endif // UCABSTRACTBUTTON_H
