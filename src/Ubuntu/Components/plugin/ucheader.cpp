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
#include <QtQuick/private/qquickanimation_p.h>
#include "ucubuntuanimation.h"
#include "ucunits.h"


/*!
    \qmltype Header
    \instantiates UCHeader
    \inherits StyledItem
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \since Ubuntu.Components 1.3
    \brief Header bar that can contain the title and controls for the current view.

    The Header component will by default be positioned at the top
    of its parent spanning the full width of the parent.
    It can be exposed and hidden by setting the
    \l exposed property, and when a \l flickable is set, the header will
    scroll together with the flickable and expose or hide when the
    Flickable movement ends.

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    Item {
        width: units.gu(50)
        height: units.gu(70)

        Header {
            id: header
            z: 1 // ensure the header goes on top of the flickable contents
            flickable: scrollableContent

            Rectangle {
                // to visualize the header
                anchors.fill: parent
                color: UbuntuColors.blue
            }
        }

        Flickable {
            id: scrollableContent
            anchors.fill: parent
            contentHeight: height * 2
            Label {
                text: "Flick me"
            }
        }
    }
    \endqml

    The default implicitWidth of the Header it its parent's width. The default
    z-value is 0, so declare the Header after any Items that it should overlay, or
    set its z-value to be larger than that of the other Items. The initial y-value
    is 0, but scrolling the flickable or setting \l exposed to false will change
    the y-value in the range of -height and 0.
*/

UCUbuntuAnimation *UCHeader::s_ubuntuAnimation = new UCUbuntuAnimation();

#define IMPLICIT_HEADER_HEIGHT_GU 6

UCHeader::UCHeader(QQuickItem *parent)
    : UCStyledItemBase(parent)
    , m_exposed(true)
    , m_moving(false)
    , m_previous_contentY(0)
    , m_showHideAnimation(new QQuickNumberAnimation)
    , m_flickable(Q_NULLPTR)
{
    m_showHideAnimation->setTargetObject(this);
    m_showHideAnimation->setProperty("y");
    m_showHideAnimation->setEasing(s_ubuntuAnimation->StandardEasing());
    m_showHideAnimation->setDuration(s_ubuntuAnimation->BriskDuration());

    connect(m_showHideAnimation, SIGNAL(runningChanged(bool)),
            this, SLOT(q_showHideAnimationRunningChanged()));

    // Watch grid unit size change and set implicit size
//    connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(q_updateSize()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(q_heightChanged()));

    // Width and height will be updated when itemChange() is called because the parent is set.
}

void UCHeader::q_heightChanged() {
    this->updateFlickableMargins();
    if (m_exposed || (!m_flickable.isNull() && m_flickable->contentY() <= 0.0)) {
        // Header was exposed before, or the flickable is scrolled up close to
        //  the top so that the header should be visible.
        this->show();
    } else {
        this->hide();
    }
}

/*!
 * \qmlproperty Flickable Header::flickable
 *
 * When flickable is set, scrolling vertically in the flickable, or setting the
 * Flickable's y-value will move the header y-position by the same amount as the
 * flickable content movement. When scrolling the flickable, upon release, the header
 * will animate to fully exposed or fully hidden state, depending on whether it was
 * more or less than half exposed when the user stopped moving the flickable.
 *
 * When flickable is null, the header can be exposed or
 * hidden by setting the \l exposed property.
 *
 * The topMargin of the flickable will automatically be updated to always match
 * the height of the header. When changing the flickable, the topMargin of the previous
 * flickable is set to 0.
 */
QQuickFlickable* UCHeader::flickable() {
    return m_flickable.data();
}

void UCHeader::setFlickable(QQuickFlickable *flickable) {
    if (m_flickable != flickable) {
        if (!m_flickable.isNull()) {
            // Finish the current header movement in case the current
            //  flickable is disconnected while scrolling.
            this->q_flickableMovementEnded();
            m_flickable->disconnect(this);
            m_flickable->setTopMargin(0.0);
        }

        m_flickable = flickable;
        Q_EMIT flickableChanged();

        if (!m_flickable.isNull()) {
            connect(m_flickable, SIGNAL(contentYChanged()),
                    this, SLOT(q_scrolledContents()));
            connect(m_flickable, SIGNAL(movementEnded()),
                    this, SLOT(q_flickableMovementEnded()));
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
            //  pull up contents when header shrinks.
            m_flickable->setContentY(previousContentY - headerHeight + previousHeaderHeight);
        }
    }
}

void UCHeader::show() {
    if (!m_exposed) {
        m_exposed = true;
        Q_EMIT exposedChanged();
        if (m_showHideAnimation->isRunning()) {
            // The header was in the process of hiding.
            m_showHideAnimation->stop();
        }
    }
    m_showHideAnimation->setFrom(this->y());
    m_showHideAnimation->setTo(0.0);
    m_showHideAnimation->start();
}

void UCHeader::hide() {
    if (m_exposed) {
        m_exposed = false;
        Q_EMIT exposedChanged();
        if (m_showHideAnimation->isRunning()) {
            // The header was in the process of showing.
            m_showHideAnimation->stop();
        }
    }
    m_showHideAnimation->setFrom(this->y());
    m_showHideAnimation->setTo(-1.0*this->height());
    m_showHideAnimation->start();
}

void UCHeader::q_showHideAnimationRunningChanged() {
    if (!m_showHideAnimation->isRunning()) {
        // Animation finished.
        Q_ASSERT(m_moving);
        m_moving = false;
        Q_EMIT movingChanged();
    } else if (!m_moving) {
        // Animation started.
        m_moving = true;
        Q_EMIT movingChanged();
    } // else: Transition from flickable movement to showHideAnimation running.
}

/*!
 * \qmlproperty bool Header::exposed
 * Exposes and hides the header by animating its y-value between -height and 0
 * to move it in or out of its parent Item. The value of exposed can be set directly,
 * or it will be automatically updated when the user exposes or hides the Header
 * by scrolling the Header's \l flickable.
 */
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

/*!
 * \qmlproperty bool Header::moving
 * \readonly
 * Indicates whether the header is currently moving, either because contentY of
 * the \l flickable changes (due to user interaction or by setting it directly),
 * or because the header is animating in or out because the value of \l exposed
 * was updated.
 */
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
        // Restrict the header y between -height and 0.
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
        //  that it is visible.
        this->show();
    }
}

void UCHeader::q_flickableInteractiveChanged() {
    Q_ASSERT(!m_flickable.isNull());
    if (!m_flickable->isInteractive()) {
        // The user cannot scroll down to expose the header, so ensure
        //  that it is visible.
        this->show();
    }
}
