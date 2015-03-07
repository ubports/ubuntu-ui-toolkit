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

#ifndef UCSTYLEDITEMBASE_H
#define UCSTYLEDITEMBASE_H

#include <QtQuick/QQuickItem>

class UCStyledItemBasePrivate;
class UCStyleSet;
class UCStyledItemBase : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool activeFocusOnPress
               READ activefocusOnPress WRITE setActiveFocusOnPress
               NOTIFY activeFocusOnPressChanged REVISION 1)
    Q_PROPERTY(UCStyleSet *styleSet READ styleSet WRITE setStyleSet RESET resetStyleSet NOTIFY styleSetChanged REVISION 2)
public:
    explicit UCStyledItemBase(QQuickItem *parent = 0);

    bool activefocusOnPress() const;
    void setActiveFocusOnPress(bool value);

    UCStyleSet *styleSet() const;
    void setStyleSet(UCStyleSet *styleSet);
    void resetStyleSet();

public Q_SLOTS:
    Q_REVISION(1) bool requestFocus(Qt::FocusReason reason = Qt::OtherFocusReason);

Q_SIGNALS:
    void activeFocusOnPressChanged();
    Q_REVISION(2) void styleSetChanged();

protected:
    UCStyledItemBase(UCStyledItemBasePrivate &, QQuickItem *parent);

    void mousePressEvent(QMouseEvent *event);
    bool childMouseEventFilter(QQuickItem *child, QEvent *event);
    void itemChange(ItemChange, const ItemChangeData &);

private:
    Q_DECLARE_PRIVATE(UCStyledItemBase)
    Q_PRIVATE_SLOT(d_func(), void _q_ascendantChanged(QQuickItem*))
    Q_PRIVATE_SLOT(d_func(), void _q_parentStyleChanged())
};

#endif // UCSTYLEDITEMBASE_H
