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

#include "appheaderbase.h"
//#include "ucheader.h"
#include <QDebug>
//#include <QtQuick/private/qquickflickable_p.h>
//#include <QtQuick/private/qquickanchors_p.h>
//#include <QtQuick/private/qquickanimation_p.h>
//#include "ucubuntuanimation.h"
//#include "ucunits.h"
//#include "propertychange_p.h"


UCAppHeaderBase::UCAppHeaderBase(QQuickItem *parent)
    : UCHeader(parent)
//    , m_animate(true)
{
    qDebug()<<"Create AppHeaderBase.";
}

//void AppHeaderBase::show(bool animate) {
//    if (m_exposed && !m_moving && y() == 0.0) return;
//    if (!m_exposed) {
//        m_exposed = true;
//        Q_EMIT exposedChanged();
//        if (m_showHideAnimation->isRunning()) {
//            // The header was in the process of hiding.
//            m_showHideAnimation->stop();
//        }
//    }

//    if (animate && isComponentComplete()) {
//        m_showHideAnimation->setFrom(y());
//        m_showHideAnimation->setTo(0.0);
//        m_showHideAnimation->start();
//    } else {
//        // If a previous animation was showing the header, stop it.
//        m_showHideAnimation->stop();
//        this->setY(0.0);
//        if (m_moving) {
//            m_moving = false;
//            Q_EMIT movingChanged();
//        }
//    }
//}

//void AppHeaderBase::hide(bool animate) {
//    if (!m_exposed && !m_moving && y() == -1.0*height()) return;
//    if (m_exposed) {
//        m_exposed = false;
//        Q_EMIT exposedChanged();
//        if (m_showHideAnimation->isRunning()) {
//            // The header was in the process of showing.
//            m_showHideAnimation->stop();
//        }
//    }

//    if (animate && isComponentComplete()) {
//        m_showHideAnimation->setFrom(y());
//        m_showHideAnimation->setTo(-1.0*height());
//        m_showHideAnimation->start();
//    } else {
//        // If a previous animation was hiding the header, stop it.
//        m_showHideAnimation->stop();
//        this->setY(-1.0*height());
//        if (m_moving) {
//            m_moving = false;
//            Q_EMIT movingChanged();
//        }
//    }
//}

/*!
 * \qmlproperty bool Header::animate
 * Animate the y-value when the value of \l exposed is set or unset.
 * If the header is already animating while the value is set to false,
 * the current animation is not cancelled.
 * Default value: true.
 */
//void UCAppHeaderBase::setAnimate(bool animate) {
//    if (animate != m_animate) {
//        m_animate = animate;
//        Q_EMIT animateChanged();
//    }
//}
