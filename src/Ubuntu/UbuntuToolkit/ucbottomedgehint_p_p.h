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
 *
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCBOTTOMEDGEHINT_P
#define UCBOTTOMEDGEHINT_P

#include "ucbottomedgehint_p.h"
#include "ucactionitem_p_p.h"
#include <ubuntugesturesglobal.h>

UG_FORWARD_DECLARE_CLASS(UCSwipeArea)

UT_NAMESPACE_BEGIN

class PropertyChange;
class UCBottomEdgeHintPrivate : public UCActionItemPrivate
{
    Q_DECLARE_PUBLIC(UCBottomEdgeHint)
public:
    static UCBottomEdgeHintPrivate *get(UCBottomEdgeHint *item)
    {
        return item->d_func();
    }
    UCBottomEdgeHintPrivate();
    void init();

    QBasicTimer deactivationTimer;
    UG_PREPEND_NAMESPACE(UCSwipeArea) *swipeArea;
    QQuickFlickable *flickable;
    PropertyChange *flickableBottomMargin;
    int deactivateTimeout;
    UCBottomEdgeHint::Status status;
    bool pressed:1;
};

UT_NAMESPACE_END

#endif // UCBOTTOMEDGEHINT_P
