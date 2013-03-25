/*
 * Copyright 2012 Canonical Ltd.
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

#include "ucstyle.h"
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlInfo>
#include <private/qqmlproperty_p.h>
#include <private/qqmlabstractbinding_p.h>

/*!
 * \qmltype Style
 * \instantiates UCStyle
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup theming
 * \brief The Style component is the base component of types defining styling
 * properties.
 *
 * The component provides automatic styling of styled item and delegate properties.
 */

UCStyle::UCStyle(QObject *parent) :
    QObject(parent)
{
}

UCStyle::~UCStyle()
{
    // we shouldn't have bindings by this time, but just in case...
    QHashIterator<int, Binding> i(m_bindings);
    while (i.hasNext()) {
        i.next();
        unbind(i.key());
    }
}

/*!
 * \internal
 * The method creates the property bindings between the styled item properties
 * and the style. The propertyMap contains the styled item properties that can
 * still be bound.
 */
void UCStyle::bindStyledItem(QQuickItem *item, StyledPropertyMap &propertyMap)
{
    // the styled item must be the parent of the style object
    if (!item || (parent() != item))
        return;
    QString omit(OMIT_STYLED_PROPERTIES);
    const QMetaObject *styleMo = metaObject();
    const QMetaObject *itemMo = item->metaObject();

    // loop through styled item properties as some new properties in the derivates may
    // depend on previous properties (i.e. Label's fontSize depends on font)
    for (int i = 0; i < itemMo->propertyCount(); i++) {
        const QMetaProperty itemProperty = itemMo->property(i);
        if (omit.contains(itemProperty.name()))
            continue;
        int styleIndex = styleMo->indexOfProperty(itemProperty.name());
        if (styleIndex == -1)
            continue;
        const QMetaProperty styleProperty = styleMo->property(styleIndex);
        if (!styleProperty.hasNotifySignal())
            continue;

        // check if the property has equivalent in the attachee and if we can style it
        // this means that the equivalent property index in attachee is present in stylableProperties
        QQmlProperty qmlProperty(item, itemProperty.name(), qmlContext(item));
        if (QQmlPropertyPrivate::binding(qmlProperty)) {
            // mark as not stylable
            propertyMap.insert(i, false);
            continue;
        }
        // if not bound, check if we can still style it
        if (propertyMap.value(i) && !m_bindings.contains(styleIndex)) {

            // bind
            bind(styleIndex, item, qmlProperty);
        }
    }
}

/*!
 * \internal
 * The method creates the property bindings between the delegate properties
 * and the style. The propertyMap contains the styled item properties that can
 * still be bound. The method uses this parameter to detect which delegate
 * property can be styled which has an equivalent in styled item, and the
 * styled item property is not banned from styling.
 * Example: The style ditem has a color property and the delegate has also a
 * color property. If styled item's color property gets banned from styling (got
 * bound or got assigned a value), delegate's colo rproperty should also be
 * excluded from styling.
 */
void UCStyle::bindDelegate(QQuickItem *item, StyledPropertyMap &propertyMap)
{
    QQuickItem *styledItem = qobject_cast<QQuickItem*>(parent());
    if (!item || !styledItem)
        return;

    // omit different amoutn of properties for delegates
    QString omit(OMIT_DELEGATE_PROPERTIES);
    const QMetaObject *styleMo = metaObject();
    const QMetaObject *delegateMo = item->metaObject();

    for (int i = 0; i < delegateMo->propertyCount(); i++) {
        const QMetaProperty delegateProperty = delegateMo->property(i);
        if (!delegateProperty.hasNotifySignal() || omit.contains(delegateProperty.name()))
            continue;

        // find out whether the property was already connected to attachee or the
        // property is not in the style
        int styleIndex = styleMo->indexOfProperty(delegateProperty.name());
        if ((styleIndex == -1) || m_bindings.contains(styleIndex))
            continue;
        int styledItemIndex = styledItem->metaObject()->indexOfProperty(delegateProperty.name());
        if ((styledItemIndex != -1) && propertyMap.contains(styledItemIndex) && !propertyMap.value(styledItemIndex))
            continue;

        // write and memorize
        QQmlProperty qmlProperty(item, delegateProperty.name(), qmlContext(item));
        bind(styleIndex, item, qmlProperty);
    }
}

/*!
 * \internal
 * The method removes all the style bindings between the item and the style.
 */
void UCStyle::unbindItem(QQuickItem *item)
{
    if (!item)
        return;
    QHashIterator<int, Binding> i(m_bindings);
    while (i.hasNext()) {
        i.next();
        Binding binding = i.value();
        if (binding.target == item) {
            unbind(i.key());
        }
    }
}

/*!
 * \internal
 * The method removes style binding on a given property.
 */
void UCStyle::unbindProperty(const QString &property)
{
    int index = metaObject()->indexOfProperty(property.toLatin1());
    if ((index == -1) || !m_bindings.contains(index))
        return;
    unbind(index);
}

/*!
 * \internal
 * Returns whether the updated styled property is the one given as parameter.
 */
bool UCStyle::isUpdating(const QString &property)
{
    return property == m_propertyUpdated;
}


/*!
 * \internal
 * Slot called when any of the style property changes. This assures the styled
 * properties are updated properly, and updates the binding object on the property.
 */
void UCStyle::updateStyledItem()
{
    QMetaMethod signal = metaObject()->method(senderSignalIndex());
    if (!signal.isValid()) {
        qmlInfo(this) << "Unknown sender signal detected";
        return;
    }

    m_propertyUpdated = QString(signal.name()).remove("Changed");

    QQmlProperty styleProperty(this, m_propertyUpdated, qmlContext(this));
    int index = styleProperty.index();
    if (m_bindings.contains(index)) {
        Binding binding = m_bindings.value(styleProperty.index());
        QVariant::Type targetType = binding.styledProperty.property().type();

        // update binding

        QQmlAbstractBinding *tmp = QQmlPropertyPrivate::setBinding(binding.styledProperty, 0);
        if (tmp && binding.prevBind)
            tmp->destroy();
        else if (!binding.prevBind) {
            binding.prevBind = tmp;
            m_bindings.insert(index, binding);
        }

        // update value
        if (targetType == QVariant::Color) {
            // color requires special attention, when transformed to variant then back to color
            // the alpha value is lost, meaning the destination will get value of 255
            // therefore we need to convert the variant to color and set the color falue
            binding.styledProperty.write(styleProperty.read().value<QColor>());
        } else
            binding.styledProperty.write(styleProperty.read());
    }

    m_propertyUpdated.clear();
}

/*!
 * \internal
 * Creates the first binding between the style and the target (styled item or
 * delegate). The binding consist of creating the QML binding, writing the data
 * and connecting the style properties' notify signal to the updater slot.
 */
void UCStyle::bind(int index, QQuickItem *target, const QQmlProperty &property)
{
    // bind
    Binding binding;
    binding.styledProperty = property;
    binding.prevBind = QQmlPropertyPrivate::setBinding(binding.styledProperty, 0);
    binding.target = target;
    m_bindings.insert(index, binding);

    // then write
    const QMetaProperty styleProperty = metaObject()->property(index);
    m_propertyUpdated = property.name();
    binding.styledProperty.write(styleProperty.read(this));
    m_propertyUpdated.clear();

    // finally connect to update styled property
    QQmlProperty qmlProperty(this, property.name(), qmlContext(this));
    qmlProperty.connectNotifySignal(this, SLOT(updateStyledItem()));
}

/*!
 * \internal
 * Removes the binding.
 */
void UCStyle::unbind(int index)
{
    Binding binding = m_bindings.value(index);
    if (binding.prevBind) {
        // unbind
        QQmlAbstractBinding *tmp = QQmlPropertyPrivate::setBinding(binding.styledProperty, binding.prevBind);
        if (tmp)
            tmp->destroy();

        // disconnect from update
        QMetaMethod metaSignal = metaObject()->property(index).notifySignal();
        // QML signals are preseeded with a '2' character, see qqmlproperty.cpp, connectNotifySignal
        QByteArray signal('2' + metaSignal.methodSignature());
        QObject::disconnect(this, signal.constData(), this, SLOT(updateStyledItem()));

        // finally remove from bindings
        m_bindings.remove(index);
    }
}

