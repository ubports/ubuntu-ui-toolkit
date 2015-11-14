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

#include "ucactionitem.h"
#include "ucmargins.h"
#include "QtQuick/private/qquickevents_p_p.h"

class QQuickMouseArea;
class QQuickMouseEvent;
class UCAbstractButton : public UCActionItem
{
    Q_OBJECT
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool hovered READ hovered NOTIFY hoveredChanged)
    Q_PROPERTY(UCMargins *sensingMargins READ sensingMargins CONSTANT FINAL)

    // internal, declared to support the deprecated ListItem module
    Q_PROPERTY(bool __acceptEvents MEMBER m_acceptEvents)
    Q_PROPERTY(QQuickMouseArea *__mouseArea READ privateMouseArea CONSTANT)
public:
    explicit UCAbstractButton(QQuickItem *parent = 0);

    bool pressed() const;
    bool hovered() const;
    UCMargins *sensingMargins() const
    {
        return const_cast<UCMargins*>(&m_sensingMargins);
    }

    bool privateAcceptEvents() const;
    void setPrivateAcceptEvents(bool accept);
    QQuickMouseArea *privateMouseArea() const;

protected:
    void classBegin();
    void componentComplete();
    void keyPressEvent(QKeyEvent *key);
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    void adjustSensingArea();

Q_SIGNALS:
    void pressedChanged();
    void hoveredChanged();
    void clicked();
    void pressAndHold();

protected:
    void onMouseAreaPressedChanged();
    void onMouseAreaClicked(QQuickMouseEvent*);
    void onMouseAreaPressAndHold(QQuickMouseEvent*);

protected:
    UCMargins m_sensingMargins;
    QQuickMouseArea *m_mouseArea;
    bool m_acceptEvents:1;
    bool m_pressAndHoldConnected:1;

    bool isPressAndHoldConnected();
};

QML_DECLARE_TYPE(UCMargins)

#endif // UCABSTRACTBUTTON_H
