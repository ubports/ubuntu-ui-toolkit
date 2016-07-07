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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCACTIONITEM_P
#define UCACTIONITEM_P

#include "ucactionitem.h"
#include "ucstyleditembase_p.h"

namespace UbuntuToolkit {

class UCActionItemPrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCActionItem)
public:
    static UCActionItemPrivate* get(UCActionItem *item)
    {
        return item->d_func();
    }

    UCActionItemPrivate();
    void init();

    bool hasBindingOnProperty(const QString &name);
    void updateProperties();
    void attachAction(bool attach);

    // private slots
    void _q_visibleBinding();
    void _q_enabledBinding();
    void _q_invokeActionTrigger(const QVariant &value);

    enum {
        CustomText = 0x01,
        CustomIconSource = 0x02,
        CustomIconName = 0x04,
        CustomVisible = 0x40,
        CustomEnabled = 0x80
    };
    QString text;
    QString iconName;
    QUrl iconSource;
    UCAction *action;
    quint8 flags;
};

UT_NAMESPACE_END

#endif // UCACTIONITEM_P

