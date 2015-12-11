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

#include "ucbottomedgehint.h"
#include "ucactionitem_p.h"

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
    UCSwipeArea *swipeArea;
    QQuickFlickable *flickable = nullptr;
    PropertyChange *flickableBottomMargin = nullptr;
    int deactivateTimeout = 800;
    UCBottomEdgeHint::Status status;
    bool pressed:1;

    void completeComponentInitialization() override;
};

#endif // UCBOTTOMEDGEHINT_P

