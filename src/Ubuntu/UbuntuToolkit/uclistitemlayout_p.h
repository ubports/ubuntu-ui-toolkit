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

#ifndef UCLISTITEMLAYOUT_H
#define UCLISTITEMLAYOUT_H

#include "ucslotslayout_p.h"
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UCLabel;
class UBUNTUTOOLKIT_EXPORT UCListItemLayout : public UCSlotsLayout
{
    Q_OBJECT
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCLabel) *title READ title CONSTANT FINAL)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCLabel) *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCLabel) *summary READ summary CONSTANT FINAL)
#else
    Q_PROPERTY(UCLabel *title READ title CONSTANT FINAL)
    Q_PROPERTY(UCLabel *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(UCLabel *summary READ summary CONSTANT FINAL)
#endif

public:
    explicit UCListItemLayout(QQuickItem *parent = 0);

    QQuickItem *mainSlot() override;
    //we don't allow changing the main slot on ListItemLayout because otherwise
    //accessing title/subtitle/summary would lead to a crash
    void setMainSlot(QQuickItem *slot, bool fireSignal = true) override;

    UCLabel *title();
    UCLabel *subtitle();
    UCLabel *summary();
};

UT_NAMESPACE_END

#endif // UCLISTITEMLAYOUT_H
