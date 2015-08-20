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

class QQuickMouseArea;
class UCAbstractButton : public UCActionItem
{
    Q_OBJECT
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool hovered READ hovered NOTIFY hoveredChanged)

    // internal, declared to support the deprecated ListItem module
    Q_PROPERTY(bool __acceptEvents MEMBER m_acceptEvents)
    Q_PROPERTY(QQuickMouseArea *__mouseArea READ privateMouseArea CONSTANT)
public:
    explicit UCAbstractButton(QQuickItem *parent = 0);

    bool pressed() const;
    bool hovered() const;

    bool privateAcceptEvents() const;
    void setPrivateAcceptEvents(bool accept);
    QQuickMouseArea *privateMouseArea() const;

protected:
    void classBegin();
    void componentComplete();
    void keyPressEvent(QKeyEvent *key);

Q_SIGNALS:
    void pressedChanged();
    void hoveredChanged();
    void clicked();
    void pressAndHold();

protected Q_SLOTS:
    void _q_mouseAreaClicked();
    void _q_mouseAreaPressAndHold();

protected:
    QQuickMouseArea *m_mouseArea;
    bool m_acceptEvents:1;

    bool isPressAndHoldConnected();
};

#endif // UCABSTRACTBUTTON_H
