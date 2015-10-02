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

#include "ucslotslayout.h"

class UCLabel;
class UCListItemLayout : public UCSlotsLayout
{
    Q_OBJECT
    Q_PROPERTY(UCLabel *title READ title CONSTANT FINAL)
    Q_PROPERTY(UCLabel *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(UCLabel *summary READ summary CONSTANT FINAL)

public:
    explicit UCListItemLayout(QQuickItem *parent = 0);

    //we don't allow changing the main slot on ListItemLayout because otherwise
    //accessing title/subtitle/summary would lead to a crash
    void setMainSlot(QQuickItem *slot) override;

    UCLabel *title();
    UCLabel *subtitle();
    UCLabel *summary();
};

#endif // UCLISTITEMLAYOUT_H
