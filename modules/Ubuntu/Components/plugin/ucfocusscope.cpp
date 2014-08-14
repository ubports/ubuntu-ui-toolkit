/*
 * Copyright 2014 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucfocusscope.h"

UCFocusScope::UCFocusScope(QQuickItem *parent)
    : QQuickItem(parent)
    , m_focusable(false)
{
    setFlag(ItemIsFocusScope);
}


SIMPLE_PROPERTY(UCFocusScope, bool, focusable, setFocusableOnChildren(m_focusable))

void UCFocusScope::focusInEvent(QFocusEvent *event)
{
    QQuickItem::focusInEvent(event);
    if (!m_focusable) {
        setFocus(false, Qt::OtherFocusReason);
    }
}

void UCFocusScope::setFocusableOnChildren(bool focus)
{
    set_focusable(focus);
    QList<UCFocusScope*> children = findChildren<UCFocusScope*>();
    Q_FOREACH(UCFocusScope *item, children) {
        item->set_focusable(focus);
    }
}
