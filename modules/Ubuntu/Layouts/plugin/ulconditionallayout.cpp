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
 * \inqmlmodule Ubuntu.Layouts 1.0
 * \ingroup ubuntu-layouts
 * \brief ConditionalLayout defines the layout of a given form factor.
 *
 * ConditionalLayout is a non-visual element defining the actual layout to be applied
 * when a given condition evaluates to true. The condition can be any valid boolean
 * JavaScript expression.
 *
 * As Layouts can be nested, it is recommended to choose properties of the Layouts item,
 * often width and height, when defining the condition. Other elements like Screen
 * orientation can also be used to define the layout activation condition.
 *
 * There must be only a single component defined in the ConditionalLayout which will be
 * instantiated upon activation.
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
 *                 anchors.fill: parent
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
 * All Items to be managed by the Layouts engine must identify themselves by setting
 * the Layouts.item attached property to a unique name (string). These unique names
 * are then used by the ItemLayout component, so can apply the new layout to the
 * correct Item.
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
 * This property defines the name of the layout. Within the definition of Layouts,
 * these names should be unique for each layout.
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
    if (name == d->name) {
        return;
    }
    d->name = name;
}

/*!
 * \qmlproperty bool ConditionalLayout::when
 * This property defines the condition that when evaluating to true, chooses this
 * ConditionalLayout to become the active layout.
 * When two ConditionalLayouts \b when condition is evaluated to true, the first
 * one declared in the layouts list is chosen.
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
