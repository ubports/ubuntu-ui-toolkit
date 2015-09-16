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

#include "ucheader.h"
#include <QDebug>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickanimation_p.h>
#include "ucubuntuanimation.h"
#include "ucunits.h"


/*!
    \qmltype Header
    \instantiates UCHeader
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \since Ubuntu.Components 1.3
    \brief The Header component will by default appear at the top
    of its parent, and can be exposed and hidden by setting the
    \l exposed property. When a \l flickable is set, the header will
    scroll together with the flickable and expose or hide when the
    Flickable movement ends.

    TODO TIM: Set z-value to FLT_MAX

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    Item {
        width: units.gu(50)
        height: units.gu(70)

        Header {
            id: header
            flickable: flickable

            Rectangle {
            // to visualize the header
            anchors.fill: parent
            color: "red"
            opacity: 0.5
            border {
                color: "black"
                width: 2
            }
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: height * 2
    \endqml
*/

UCUbuntuAnimation *UCHeader::s_ubuntuAnimation = new UCUbuntuAnimation();

#define IMPLICIT_HEADER_HEIGHT_GU 6

UCHeader::UCHeader(QQuickItem *parent)
    : QQuickItem(parent)
    , m_exposed(true)
    , m_moving(false)
    , m_previous_contentY(0)
    , m_showHideAnimation(new QQuickNumberAnimation)
    , m_flickable(Q_NULLPTR)
    , m_previous_parent(Q_NULLPTR)
{
    m_showHideAnimation->setTargetObject(this);
    m_showHideAnimation->setProperty("y");
    m_showHideAnimation->setEasing(s_ubuntuAnimation->StandardEasing());
    m_showHideAnimation->setEasing(s_ubuntuAnimation->StandardEasing());
    m_showHideAnimation->setDuration(s_ubuntuAnimation->BriskDuration());

    connect(m_showHideAnimation, SIGNAL(runningChanged(bool)),
            this, SLOT(q_showHideAnimationRunningChanged()));

    // watch grid unit size change and set implicit size
    connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(q_updateSize()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(q_heightChanged()));

    // Width and height will be updated when itemChange() is called because the parent is set.
}

void UCHeader::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickItem::itemChange(change, data);
    if (change == QQuickItem::ItemParentHasChanged) {
        // connect to the new parent to update header.implicitWidth when parent.width changes.
        if (!m_previous_parent.isNull()) {
            disconnect(m_previous_parent, SIGNAL(widthChanged()), this, SLOT(q_updateSize()));
        }
        m_previous_parent = data.item;
        if (!m_previous_parent.isNull()) {
            connect(m_previous_parent, SIGNAL(widthChanged()), this, SLOT(q_updateSize()));
        }
        this->q_updateSize();
    }
}

// called when GU size changes or parent or parent.width changes.
void UCHeader::q_updateSize()
{
    this->setImplicitHeight(UCUnits::instance().gu(IMPLICIT_HEADER_HEIGHT_GU));
    if (Q_NULLPTR != this->parentItem()) {
        this->setImplicitWidth(this->parentItem()->width());
    }
}

void UCHeader::q_heightChanged() {
    this->updateFlickableMargins();
    if (m_exposed || (!m_flickable.isNull() && m_flickable->contentY() <= 0.0)) {
        // header was exposed before, or the flickable is scrolled up close to
        //  the top so that the header should be visible
        this->show();
    } else {
        this->hide();
    }
}

QQuickFlickable* UCHeader::flickable() {
    return m_flickable.data();
}

void UCHeader::setFlickable(QQuickFlickable *flickable) {
    if (m_flickable != flickable) {
        if (!m_flickable.isNull()) {
            // Finish the current header movement in case the current
            //  flickable is disconnected while scrolling:
            this->q_flickableMovementEnded();
            m_flickable->disconnect(this);
        }

        m_flickable = flickable;
        Q_EMIT flickableChanged();

        if (!m_flickable.isNull()) {
            connect(m_flickable, SIGNAL(contentYChanged()),
                    this, SLOT(q_scrolledContents()));
            connect(m_flickable, SIGNAL(movementEnded()),
                    this, SLOT(q_flickableMovementEnded()));
            // TODO TIM: connect contentHeightChanged and interactiveChanged?
            connect(m_flickable, SIGNAL(contentHeightChanged()),
                    this, SLOT(q_contentHeightChanged()));
            connect(m_flickable, SIGNAL(interactiveChanged()),
                    this, SLOT(q_flickableInteractiveChanged()));
            m_previous_contentY = m_flickable->contentY();
            this->updateFlickableMargins();
            this->show();
        }
    }
}

void UCHeader::updateFlickableMargins() {
    if (!m_flickable.isNull()) {
        qreal headerHeight = this->height();
        qreal previousHeaderHeight = m_flickable->topMargin();
        if (headerHeight != previousHeaderHeight) {
            qreal previousContentY = m_flickable->contentY();
            m_flickable->setTopMargin(headerHeight);
            // Push down contents when header grows,
            //  pull up contents when header shrinks
            m_flickable->setContentY(previousContentY - headerHeight + previousHeaderHeight);
        }
    }
}

void UCHeader::show() {
    if (!m_exposed) {
        m_exposed = true;
        Q_EMIT exposedChanged();
        if (m_showHideAnimation->isRunning()) {
            // the header was in the process of hiding
            m_showHideAnimation->stop();
        }
    }
    // TODO TIM: disable contents when moving?
    m_showHideAnimation->setFrom(this->y());
    m_showHideAnimation->setTo(0.0);
    m_showHideAnimation->start();
}

void UCHeader::hide() {
    if (m_exposed) {
        m_exposed = false;
        Q_EMIT exposedChanged();
        if (m_showHideAnimation->isRunning()) {
            // the header was in the process of showing
            m_showHideAnimation->stop();
        }
    }
    // TODO TIM: disable contents when moving?
    m_showHideAnimation->setFrom(this->y());
    m_showHideAnimation->setTo(-1.0*this->height());
    m_showHideAnimation->start();
}

void UCHeader::q_showHideAnimationRunningChanged() {
    if (!m_showHideAnimation->isRunning()) {
        // Animation finished
        Q_ASSERT(m_moving);
        m_moving = false;
        Q_EMIT movingChanged();
    } else if (!m_moving) {
        // Animation started
        m_moving = true;
        Q_EMIT movingChanged();
    } // else: Transition from flickable movement to showHideAnimation running.
}

void UCHeader::setExposed(bool exposed) {
    if (exposed) {
        this->show();
    } else {
        this->hide();
    }
}

bool UCHeader::exposed() {
    return m_exposed;
}

bool UCHeader::moving() {
    return m_moving;
}

// Called when moving due to user interaction with the flickable, or by
// setting m_flickable.contentY programatically.
void UCHeader::q_scrolledContents() {
    Q_ASSERT(!m_flickable.isNull());
    // Avoid moving the header when rebounding or being dragged over the bounds.
    if (!m_flickable->isAtYBeginning() && !m_flickable->isAtYEnd()) {
        qreal dy = m_flickable->contentY() - m_previous_contentY;
        // Restrict the header y between -height and 0:
        qreal clampedY = qMin(qMax(-this->height(), this->y() - dy), 0.0);
        this->setY(clampedY);
    }
    m_previous_contentY = m_flickable->contentY();
    if (!m_moving) {
        m_moving = true;
        Q_EMIT movingChanged();
    }
    if (!m_flickable->isMoving()) {
        // m_flickable.contentY was set directly, so no user flicking.
        this->q_flickableMovementEnded();
    }
}

void UCHeader::q_flickableMovementEnded() {
    Q_ASSERT(!m_flickable.isNull());
    if ((m_flickable->contentY() < 0)
            || (this->y() > -this->height()/2.0)) {
        this->show();
    } else {
        this->hide();
    }
}

void UCHeader::q_contentHeightChanged() {
    Q_ASSERT(!m_flickable.isNull());
    if (m_flickable->height() >= m_flickable->contentHeight()) {
        // The user cannot scroll down to expose the header, so ensure
        //  that it is visible
        this->show();
    }
}

void UCHeader::q_flickableInteractiveChanged() {
    Q_ASSERT(!m_flickable.isNull());
    if (!m_flickable->isInteractive()) {
        // The user cannot scroll down to expose the header, so ensure
        //  that it is visible
        this->show();
    }
}
