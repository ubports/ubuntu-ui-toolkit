/*
 * Copyright 2013 Canonical Ltd.
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

#define foreach Q_FOREACH
#include <private/qqmlbinding_p.h>
#undef foreach

#include "ulconditionallayout.h"
#include "ulconditionallayout_p.h"
#include "ullayouts.h"
#include "ullayouts_p.h"

ULConditionalLayoutPrivate::ULConditionalLayoutPrivate(ULConditionalLayout *qq) :
    q_ptr(qq),
    when(0),
    component(0)
{
}

ULLayouts *ULConditionalLayoutPrivate::layouts()
{
    Q_Q(ULConditionalLayout);
    return qobject_cast<ULLayouts*>(q->parent());
}

/*!
 * \qmltype ConditionalLayout
 * \instantiates ULConditionalLayout
 * \inqmlmodule Ubuntu.Layouts 0.1
 * \ingroup ubuntu-layouts
 * \brief ConditionalLayout defines the layout of a given form factor.
 *
 * ConditionalLayout is a non-visual element defining the actual layout to be applied
 * upon the given condition evaluates to true. The condition can be any valid boolean
 * JavaScript expression.
 *
 * As Layouts can be nested, use the closest embedding Layouts object's size when defining
 * the condition. Other elements like Screen orientation can also be used to define
 * the layout activation condition.
 *
 * There can be a single component defined in the ConditionalLayout which will be
 * instantiated upon activation. This component then will be filling the entire
 * Layouts area, so no additional anchoring is required in those components.
 *
 * Each conditional layout must be named with a string, which identifies the layout
 * when activated.
 *
 * \qml
 * Layouts {
 *     id: layouts
 *     width: units.gu(40)
 *     height: units.gu(40)
 *     layouts: [
 *         ConditionalLayout {
 *             name: "flow"
 *             when: layouts.width > units.gu(60)
 *             Flow {
 *                 spacing: units.dp(3)
 *                 flow: Flow.LeftToRight
 *                 ItemLayout {
 *                     item: "item1"
 *                     width: units.gu(30)
 *                     height: units.gu(20)
 *                 }
 *                 ItemLayout {
 *                     item: "item2"
 *                     width: units.gu(30)
 *                     height: units.gu(20)
 *                 }
 *                 ItemLayout {
 *                     item: "item3"
 *                     width: units.gu(30)
 *                     height: units.gu(20)
 *                 }
 *             }
 *         }
 *     ]
 *
 *     Column {
 *         spacing: units.dp(2)
 *         Button {
 *             text: "Button #1"
 *             Layouts.item: "item1"
 *         }
 *         Button {
 *             text: "Button #2"
 *             Layouts.item: "item2"
 *         }
 *         Button {
 *             text: "Button #3"
 *             Layouts.item: "item3"
 *         }
 *     }
 * }
 * \endqml
 *
 * The items subject of layout must be having the Layouts.item attached property,
 * which will be laid out by the ItemLayout components.
 *
 * \sa {ItemLayout}
 */

ULConditionalLayout::ULConditionalLayout(QObject *parent) :
    QObject(parent),
    d_ptr(new ULConditionalLayoutPrivate(this))
{
}

ULConditionalLayout::~ULConditionalLayout()
{
}

/*!
 * \qmlproperty string ConditionalLayout::name
 * The property defines the name of the layout. Each layout should have a unique
 * name within its Layouts item, and identifies the conditional layout itself.
 * \sa Layouts
 */
QString ULConditionalLayout::layoutName() const
{
    Q_D(const ULConditionalLayout);
    return d->name;
}
void ULConditionalLayout::setLayoutName(const QString &name)
{
    Q_D(ULConditionalLayout);
    if (name.isEmpty() || (name == d->name)) {
        return;
    }
    d->name = name;
}

/*!
 * \qmlproperty bool ConditionalLayout::when
 * The property defines the condition when the layout should be activated.
 */
QQmlBinding *ULConditionalLayout::when() const
{
    Q_D(const ULConditionalLayout);
    return d->when;
}
void ULConditionalLayout::setWhen(QQmlBinding *when)
{
    Q_D(ULConditionalLayout);
    d->when = when;

    // re-layout
    ULLayouts *layouts = qobject_cast<ULLayouts*>(parent());
    if (layouts) {
        layouts->d_ptr->updateLayout();
    }
}

/*!
 * \qmlproperty Component ConditionalLayout::layout
 * \default
 * Default property holding the definition component of the layout. The component
 * will be instantiated once the condition evaluates to true.
 */
QQmlComponent *ULConditionalLayout::layout() const
{
    Q_D(const ULConditionalLayout);
    return d->component;
}
void ULConditionalLayout::setLayout(QQmlComponent *component)
{
    Q_D(ULConditionalLayout);
    d->component = component;
}
