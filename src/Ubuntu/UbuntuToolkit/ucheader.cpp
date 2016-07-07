/*
 * Copyright 2016 Canonical Ltd.
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

UT_NAMESPACE_BEGIN

/*!
    \qmltype Header
    \instantiates UbuntuToolkit::UCHeader
    \inherits StyledItem
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \since Ubuntu.Components 1.3
    \brief Header bar that can contain the title and controls for the current view.

    The Header can be exposed and hidden by setting the
    \l exposed property, and when a \l flickable is set, the header will
    scroll together with the flickable and expose or hide when the
    Flickable movement ends.

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    Page {
        width: units.gu(50)
        height: units.gu(70)

        header: Header {
            width: parent.width
            height: units.gu(6)
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

    The initial y-value is 0, but scrolling the flickable or setting \l exposed to
    false will change the y-value in the range from -height to 0.
*/

UCUbuntuAnimation *UCHeader::s_ubuntuAnimation = new UCUbuntuAnimation();

UCHeader::UCHeader(QQuickItem *parent)
    : UCStyledItemBase(parent)
    , m_flickable(Q_NULLPTR)
    , m_showHideAnimation(new QQuickNumberAnimation)
    , m_previous_contentY(0)
    , m_previous_header_height(0)
    , m_exposed(true)
    , m_moving(false)
    , m_automaticHeight(true)
{
    m_showHideAnimation->setParent(this);
    m_showHideAnimation->setTargetObject(this);
    m_showHideAnimation->setProperty("y");
    m_showHideAnimation->setEasing(s_ubuntuAnimation->StandardEasing());
    m_showHideAnimation->setDuration(s_ubuntuAnimation->BriskDuration());

    connect(m_showHideAnimation, SIGNAL(runningChanged(bool)),
            this, SLOT(_q_showHideAnimationRunningChanged()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(_q_heightChanged()));
}

UCHeader::~UCHeader() {
    if (m_flickable != Q_NULLPTR) {
        m_flickable->setTopMargin(m_flickable->topMargin() - m_previous_header_height);
    }
}

void UCHeader::_q_heightChanged() {
    updateFlickableMargins();
    if (m_exposed || (!m_flickable.isNull() && m_flickable->contentY() <= 0.0)) {
        // Header was exposed before, or the flickable is scrolled up close to
        //  the top so that the header should be visible.
        // After Header completed, the theming engine updates its implicitHeight
        //  to that of the style and we do not want to animate then. Other height
        //  changes also do not need animations.
        show(false);
    } else {
        hide(false);
    }
}

void UCHeader::itemChange(ItemChange change, const ItemChangeData &value) {
    if (change == ItemVisibleHasChanged || change == ItemParentHasChanged) {
        updateFlickableMargins();
    }
    UCStyledItemBase::itemChange(change, value);
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
 * Note that \l exposed is not automatically updated when the value of flickable
 * changes, so if the header must be exposed when the flickable changes
 * (or is set to null), this must be done explicitly. Example:
 * \qml
 *  import QtQuick 2.4
 *  import Ubuntu.Components 1.3
 *
 *  MainView {
 *      width: units.gu(60)
 *      height: units.gu(80)
 *
 *      AdaptivePageLayout {
 *          id: layout
 *          anchors.fill: parent
 *          primaryPage: Page {
 *              id: page
 *              Flickable {
 *                  id: contentFlick
 *                  anchors.fill: parent
 *                  topMargin: page.header.flickable ? 0 : page.header.height
 *                  contentHeight: units.gu(200)
 *                  // Scrolling here can hide the header.
 *              }
 *              header: PageHeader {
 *                  title: i18n.tr("Navigation")
 *                  flickable: layout.columns === 1 ? contentFlick : null
 *                  // Show header when it gets locked in a two-column layout:
 *                  onFlickableChanged: exposed = true
 *              }
 *          }
 *      }
 *  }
 * \endqml
 *
 * The topMargin of the flickable will automatically be updated by adding the height
 * of the header to the current topMargin. When changing the flickable, the topMargin
 * of the previous flickable is restored by subtracting the header height from it.
 * Making the header invisible has the same effect on the topMargin as unsetting
 * the flickable.
 *
 * It is permitted to use a ListView as the value of flickable, but this works
 * well only if the ListView.header property is not set. Alternatively,
 * a Header component may be used for ListView.header, but in that case the flickable
 * of the Header must be null.
 */
QQuickFlickable* UCHeader::flickable() {
    return m_flickable.data();
}

void UCHeader::setFlickable(QQuickFlickable *flickable) {
    if (m_flickable == flickable) {
        return;
    }
    if (!m_flickable.isNull()) {
        // Finish the current header movement in case the current
        //  flickable is disconnected while scrolling.
        if (m_exposed) {
            show(false);
        } else {
            hide(false);
        }
        m_flickable->disconnect(this);

        // store the current sum of the topMargin and contentY so that we
        //  can add the change in topMargin+contentY to the new contentY after
        //  updating the topMargin so that the user will still see the same
        //  flickable contents at the top of the view after the header height changed.
        qreal delta = m_flickable->topMargin() + m_flickable->contentY();
        m_flickable->setTopMargin(m_flickable->topMargin() - m_previous_header_height);
        m_previous_header_height = 0;
        delta -= m_flickable->topMargin() + m_flickable->contentY();

        // restore the flickable content Y.
        m_flickable->setContentY(m_flickable->contentY() + delta);
    }

    m_flickable = flickable;
    Q_EMIT flickableChanged();

    Q_ASSERT(m_previous_header_height == 0);
    if (!m_flickable.isNull()) {
        updateFlickableMargins();
        connect(m_flickable, SIGNAL(contentYChanged()),
                this, SLOT(_q_scrolledContents()));
        connect(m_flickable, SIGNAL(movementEnded()),
                this, SLOT(_q_flickableMovementEnded()));
        connect(m_flickable, SIGNAL(contentHeightChanged()),
                this, SLOT(_q_contentHeightChanged()));
        connect(m_flickable, SIGNAL(interactiveChanged()),
                this, SLOT(_q_flickableInteractiveChanged()));
        m_previous_contentY = m_flickable->contentY();
        _q_flickableMovementEnded(); // show or hide depending on y
    }
}

void UCHeader::updateFlickableMargins() {
    if (m_flickable.isNull()) {
        return;
    }
    qreal headerHeight = 0.0;
    if (isVisible() && parentItem()) {
        headerHeight = height();
    } // else: header is not visible, so do not add to the topMargin.
    if (headerHeight != m_previous_header_height) {
        qreal previousContentY = m_flickable->contentY();
        m_flickable->setTopMargin(m_flickable->topMargin() + headerHeight - m_previous_header_height);
        // Push down contents when header grows,
        //  pull up contents when header shrinks.
        m_flickable->setContentY(previousContentY - headerHeight + m_previous_header_height);
        m_previous_header_height = headerHeight;
    }
}

void UCHeader::show(bool animate) {
    if (m_exposed && !m_moving && y() == 0.0) return;
    if (!m_exposed) {
        m_exposed = true;
        Q_EMIT exposedChanged();
        if (m_showHideAnimation->isRunning()) {
            // The header was in the process of hiding.
            m_showHideAnimation->stop();
        }
    }

    if (animate && isComponentComplete()) {
        m_showHideAnimation->setFrom(y());
        m_showHideAnimation->setTo(0.0);
        m_showHideAnimation->start();
    } else {
        // If a previous animation was showing the header, stop it.
        m_showHideAnimation->stop();
        this->setY(0.0);
        if (m_moving) {
            m_moving = false;
            Q_EMIT movingChanged();
        }
    }
}

void UCHeader::hide(bool animate) {
    if (!m_exposed && !m_moving && y() == -1.0*height()) return;
    if (m_exposed) {
        m_exposed = false;
        Q_EMIT exposedChanged();
        if (m_showHideAnimation->isRunning()) {
            // The header was in the process of showing.
            m_showHideAnimation->stop();
        }
    }

    if (animate && isComponentComplete()) {
        m_showHideAnimation->setFrom(y());
        m_showHideAnimation->setTo(-1.0*height());
        m_showHideAnimation->start();
    } else {
        // If a previous animation was hiding the header, stop it.
        m_showHideAnimation->stop();
        this->setY(-1.0*height());
        if (m_moving) {
            m_moving = false;
            Q_EMIT movingChanged();
        }
    }
}

void UCHeader::_q_showHideAnimationRunningChanged() {
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
        show(true);
    } else {
        hide(true);
    }
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

/*!
 * \qmlproperty bool Header::automaticHeight
 * The heights of the \l Page headers in an \l AdaptivePageLayout are synchronized
 * by the AdaptivePageLayout so that they all get the height of the largest
 * implicitHeight of the visible Page headers to give a uniform look to the headers.
 * To opt-out of this height synchronization, set automaticHeight to false. This will
 * prevent the AdaptivePageLayout from taking the Header's implicitHeight into account
 * for computing the maximum header height, and it will also not update the height of
 * the header that has automaticHeight set to false to match the other header heights.
 * Default value: true.
 */

// Called when moving due to user interaction with the flickable, or by
// setting m_flickable.contentY programatically.
void UCHeader::_q_scrolledContents() {
    Q_ASSERT(!m_flickable.isNull());
    // Avoid moving the header when rebounding or being dragged over the bounds.
    if (!m_flickable->isAtYBeginning() && !m_flickable->isAtYEnd()) {
        qreal dy = m_flickable->contentY() - m_previous_contentY;
        // Restrict the header y between -height and 0.
        qreal clampedY = qMin(qMax(-height(), y() - dy), 0.0);
        setY(clampedY);
    }
    m_previous_contentY = m_flickable->contentY();
    if (!m_moving) {
        bool move = m_exposed ? y() != 0.0 : y() != -height();
        if (move) {
            m_moving = true;
            Q_EMIT movingChanged();
        }
    }
    if (!m_flickable->isMoving()) {
        // m_flickable.contentY was set directly, so no user flicking.
        _q_flickableMovementEnded();
    }
}

void UCHeader::_q_flickableMovementEnded() {
    Q_ASSERT(!m_flickable.isNull());
    if ((m_flickable->contentY() < 0)
            || (y() > -height()/2.0)) {
        show(true);
    } else {
        hide(true);
    }
}

void UCHeader::_q_contentHeightChanged() {
    Q_ASSERT(!m_flickable.isNull());
    if (m_flickable->height() >= m_flickable->contentHeight()) {
        // The user cannot scroll down to expose the header, so ensure
        //  that it is visible.
        show(true);
    }
}

void UCHeader::_q_flickableInteractiveChanged() {
    Q_ASSERT(!m_flickable.isNull());
    if (!m_flickable->isInteractive()) {
        // The user cannot scroll down to expose the header, so ensure
        //  that it is visible.
        show(true);
    }
}

UT_NAMESPACE_END
