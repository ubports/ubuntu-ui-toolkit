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
//#include <QtQuick/private/qquickbehavior_p.h>

/*!
    \qmltype Header
    \instantiates UCHeader
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief TODO
*/

UCHeader::UCHeader(QQuickItem *parent)
    : UCStyledItemBase(parent)
//    , m_behavior_on_y(new QQuickBehavior)
{
    // TODO
    qDebug() << "Header created!";
    // TODO TIM: Don't use Behavior here. Use an Animation directly
    //  when show/hide is called.
//    m_behavior_on_y = new QQuickBehavior(this);
//    m_behavior_on_y->setParent(this);
//    m_behavior_on_y->setTarget(this->y());
}

//void UCHeader::classBegin()
//{
//    UCStyledItemBase::classBegin();
//}

//void UCHeader::componentComplete()
//{
//    UCStyledItemBase::componentComplete();
//}
