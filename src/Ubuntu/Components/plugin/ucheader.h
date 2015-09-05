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

#ifndef UCHEADER_H
#define UCHEADER_H

#include "ucstyleditembase.h"
//class QQuickBehavior;

class UCHeader : public UCStyledItemBase
{
    Q_OBJECT
//    Q_PROPERTY(bool visible READ visible WRITE setVisible)

public:
    // TODO TIM: why is this explicit??
    explicit UCHeader(QQuickItem *parent = 0);

//protected:
////    void classBegin();
////    void componentComplete();

private:
    bool m_visible;
//    QQuickBehavior *m_behavior_on_y;
};

#endif // UCHEADER_H
