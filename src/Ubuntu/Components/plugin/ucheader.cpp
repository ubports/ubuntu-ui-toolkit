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
    \brief TODO

    By default, Header will anchor to the left, right and top of its
    parent, and it hides by changing its top-margin to be -height.
    TODO: NOPE. But topMargin will change.
*/

UCUbuntuAnimation *UCHeader::s_ubuntuAnimation = new UCUbuntuAnimation();

#define IMPLICIT_HEADER_HEIGHT_GU 6

UCHeader::UCHeader(QQuickItem *parent)
//    : UCStyledItemBase(parent)
    : QQuickItem(parent)
    , m_exposed(true)
    , m_moving(false)
    , m_previous_contentY(0)
    , m_showHideAnimation(new QQuickNumberAnimation)
    , m_flickable(Q_NULLPTR)
{
//    this->setFocus();
    m_showHideAnimation->setTargetObject(this);
    m_showHideAnimation->setProperty("y");
    m_showHideAnimation->setEasing(s_ubuntuAnimation->StandardEasing());
    m_showHideAnimation->setEasing(s_ubuntuAnimation->StandardEasing());
    m_showHideAnimation->setDuration(s_ubuntuAnimation->BriskDuration());

    connect(m_showHideAnimation, SIGNAL(runningChanged(bool)),
            this, SLOT(q_showHideAnimationRunningChanged()));

    // watch grid unit size change and set implicit size
    connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(q_updateSize()));
    // TODO: connect to parent changes too
    q_updateSize();
}

//// called when units size changes
void UCHeader::q_updateSize()
{
    // update divider thickness
//    divider->setImplicitHeight(UCUnits::instance().dp(DIVIDER_THICKNESS_DP));
//    QQuickItem *owner = qobject_cast<QQuickItem*>(q->sender());
//    if (!owner && parentAttached) {
//        owner = static_cast<QQuickItem*>(parentAttached->parent());
//    }
//    q->setImplicitWidth(owner ? owner->width() : UCUnits::instance().gu(IMPLICIT_LISTITEM_WIDTH_GU));
    this->setImplicitHeight(UCUnits::instance().gu(IMPLICIT_HEADER_HEIGHT_GU));
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
            flickable->disconnect(this);
        }

        m_flickable = flickable;
        Q_EMIT flickableChanged();

        if (!m_flickable.isNull()) {
            this->updateFlickableMargins();
            connect(m_flickable, SIGNAL(contentYChanged()),
                    this, SLOT(q_scrolledContents()));
            connect(m_flickable, SIGNAL(movementEnded()),
                    this, SLOT(q_flickableMovementEnded()));
            m_previous_contentY = m_flickable->contentY();
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
    }
    m_showHideAnimation->setFrom(this->y());
    m_showHideAnimation->setTo(0.0);
    m_showHideAnimation->start();
}

void UCHeader::hide() {
    if (m_exposed) {
        m_exposed = false;
        Q_EMIT exposedChanged();
    }
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

void UCHeader::q_scrolledContents() {
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
}

void UCHeader::q_flickableMovementEnded() {
    if ((!m_flickable.isNull() && m_flickable->contentY() < 0)
            || (this->y() > -this->height()/2.0)) {
        this->show();
    } else {
        this->hide();
    }
}
