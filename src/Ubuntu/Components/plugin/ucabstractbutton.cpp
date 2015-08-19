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

#include "ucabstractbutton.h"
#include "uchaptics.h"
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQml/private/qqmlglobal_p.h>

UCAbstractButton::UCAbstractButton(QQuickItem *parent)
    : UCActionItem(parent)
    , m_mouseArea(new QQuickMouseArea)
    , m_acceptEvents(true)
{
    setActiveFocusOnPress(true);
    connect(this, SIGNAL(clicked()), this, SLOT(trigger()));
}

bool UCAbstractButton::isPressAndHoldConnected()
{
    static QMetaMethod method = QMetaMethod::fromSignal(&UCAbstractButton::pressAndHold);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(this)->isSignalConnected(signalIdx);
}

void UCAbstractButton::classBegin()
{
    UCActionItem::classBegin();
    // make sure we have the haptics set up!
    HapticsProxy::instance().initialize();

    // set up mouse area
    QQml_setParent_noEvent(m_mouseArea, this);
    m_mouseArea->setParentItem(this);
    QQuickAnchors *anchors = QQuickItemPrivate::get(m_mouseArea)->anchors();
    anchors->setFill(this);
    m_mouseArea->setHoverEnabled(true);

    // bind mouse area
    connect(m_mouseArea, &QQuickMouseArea::pressedChanged, this, &UCAbstractButton::pressedChanged);
    connect(m_mouseArea, &QQuickMouseArea::hoveredChanged, this, &UCAbstractButton::hoveredChanged);
    connect(m_mouseArea, SIGNAL(clicked(QQuickMouseEvent*)), this, SLOT(_q_mouseAreaClicked()));
    if (isPressAndHoldConnected()) {
        connect(m_mouseArea, SIGNAL(pressAndHold(QQuickMouseEvent*)), this, SLOT(_q_mouseAreaPressAndHold()));
    }
}

void UCAbstractButton::_q_mouseAreaClicked()
{
    // required by the deprecated ListItem module
    if (!m_acceptEvents) {
        return;
    }
    // play haptics
    HapticsProxy::instance().play(QVariant());
    Q_EMIT clicked();
}

void UCAbstractButton::_q_mouseAreaPressAndHold()
{
    // required by the deprecated ListItem module
    if (!m_acceptEvents) {
        return;
    }
    Q_EMIT pressAndHold();
}

void UCAbstractButton::keyPressEvent(QKeyEvent *event)
{
    UCActionItem::keyPressEvent(event);

    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            Q_EMIT clicked();
            break;
        }
    }
}

bool UCAbstractButton::pressed() const
{
    return m_mouseArea ? m_mouseArea->pressed() : false;
}
bool UCAbstractButton::hovered() const
{
    return m_mouseArea ? m_mouseArea->hovered() : false;
}

QQuickMouseArea *UCAbstractButton::privateMouseArea() const
{
    return m_mouseArea;
}
