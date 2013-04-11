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

#define foreach Q_FOREACH
#include <private/qqmlbinding_p.h>
#undef foreach

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
    QHashIterator<QString, QQmlProperty> i(m_bindings);
    while (i.hasNext()) {
        i.next();
        unbind(i.key());
    }
}

/*!
 * \internal
 * The method creates the property bindings between the item properties and the
 * style. The propertyMap contains the item properties that can still be bound.
 * Returns the number of properties bound, -1 on error.
 */
int UCStyle::bindItem(QQuickItem *item, StyledPropertyMap &propertyMap, bool usePropertyMap)
{
    if (!item)
        return -1;

    const QMetaObject *styleMo = metaObject();
    const QMetaObject *targetMo = item->metaObject();
    int result = 0;

    for (int i = 0; i < targetMo->propertyCount(); i++) {
        const QMetaProperty targetProperty = targetMo->property(i);
        const char *name = targetProperty.name();

        // check if it should be omitted
        if (omitProperty(name))
            continue;

        // check if we have a corresponding style property
        // all style properties have notify signals; therefore no need to check those
        if (styleMo->indexOfProperty(name) == -1)
            continue;

        // check whether we have it already styled
        // this check is needed because shadowed or extended properties are listed twice
        // or because the styled item already has the property and the main item's property
        // is not banned from styling, in which case the delegate one shouldn't be styled
        if (m_bindings.contains(name))
            continue;
        // if the styled item property is banned, skip
        if (usePropertyMap && !propertyMap.isEnabled(i))
            continue;

        QQmlProperty qmlProperty(item, name, qmlContext(item));

        if (usePropertyMap && QLatin1String(name) != QLatin1String("font")) {
            // styled item specific: check if it has a QML binding and whether the binding
            // is the original one or a newer one
            // do not ban the front property from being styled even if it has a binding
            // which happens in Label: "font.pixelSize: FontUtils.sizeToPixels(fontSize)"
            QQmlAbstractBinding *binding = QQmlPropertyPrivate::binding(qmlProperty);
            if (binding) {
                // check if this binding is the original one
                if (binding == propertyMap.binding(i)) {
                    // we can destroy it and style
                    binding->destroy();
                } else {
                    // newer binding, mark as banned
                    propertyMap.mark(i, StyledPropertyMap::Banned);
                    continue;
                }
            }
        }

        // finally do the binding
        if (usePropertyMap)
            // mark it as styled
            propertyMap.mark(i, StyledPropertyMap::Styled);

        bind(qmlProperty);
        result++;
    }
    return result;
}

/*!
 * \internal
 * The method removes all the style bindings between the item and the style.
 * Returns true if the bindings were successfully removed.
 */
bool UCStyle::unbindItem(QQuickItem *item)
{
    if (!item)
        return false;

    bool result = false;
    QHashIterator<QString, QQmlProperty> i(m_bindings);
    while (i.hasNext()) {
        i.next();
        if (i.value().object() == item) {
            unbind(i.key());
            result = true;
        }
    }
    return result;
}

/*!
 * \internal
 * The method removes style binding on a given property. Returns true on
 * success.
 */
bool UCStyle::unbindProperty(const QString &property)
{
    if (!m_bindings.contains(property))
        return false;
    unbind(property);
    return true;
}

/*!
 * \internal
 * Returns whether the updated styled property is the one given as parameter.
 */
bool UCStyle::isUpdating(const QString &property) const
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

    QString property = QString(signal.name()).remove("Changed");
    // update value
    write(property, m_bindings.value(property));
}

/*!
 * \internal
 * Creates the first binding between the style and the target (styled item or
 * delegate). The binding consist of creating the QML binding, writing the data
 * and connecting the style properties' notify signal to the updater slot.
 */
void UCStyle::bind(const QQmlProperty &property)
{
    // store target property
    m_bindings.insert(property.name(), property);

    // connect the style property's notify signal so we can guard
    // styled item property changes when change occurs because of style changes
    const QMetaObject *mo = metaObject();
    QMetaMethod notifySignal = mo->property(mo->indexOfProperty(property.name().toLocal8Bit())).notifySignal();
    QMetaMethod updateSlot = mo->method(mo->indexOfSlot("updateStyledItem()"));
    QObject::connect(this, notifySignal, this, updateSlot);

    write(property.name(), property);
}

/*!
 * \internal
 * Removes the binding.
 */
void UCStyle::unbind(const QString &property)
{
    // disconnect from update
    const QMetaObject *mo = metaObject();
    QMetaMethod metaSignal = mo->property(mo->indexOfProperty(property.toLatin1())).notifySignal();
    QMetaMethod updateSlot = mo->method(mo->indexOfSlot("updateStyledItem()"));
    QObject::disconnect(this, metaSignal, this, updateSlot);

    // finally remove from bindings
    m_bindings.remove(property);
}

void UCStyle::write(const QString &source, const QQmlProperty &destination)
{
    if (!property(source.toLocal8Bit()).isValid()) {
        qmlInfo(this) << "invalid property: " << source;
        return;
    }
    m_propertyUpdated = destination.name();
    const QMetaProperty target = destination.property();
    if (target.type() == QMetaType::QColor) {
        // color requires special attention, when transformed to variant then back to color
        // the alpha value is lost, meaning the destination will get value of 255
        // therefore we need to convert the variant to color and set the color falue
        destination.write(property(source.toLatin1()).value<QColor>());
    } else if (target.type() == QMetaType::QFont) {
        // only set font subproperties that were not set before
        QFont sourceValue = property(source.toLatin1()).value<QFont>();
        QFont destinationValue = destination.read().value<QFont>();

        // keep binding if there were any
        QQmlAbstractBinding *originalBinding = QQmlPropertyPrivate::setBinding(destination, 0);
        QFont result = destinationValue.resolve(sourceValue); 
        destination.write(result);

        // restore binding if any
        if (originalBinding) {
            QQmlAbstractBinding *previousBinding = QQmlPropertyPrivate::setBinding(destination, originalBinding);
            if (previousBinding) {
                previousBinding->destroy();
            }
        }
    } else {
        destination.write(property(source.toLatin1()));
    }
    m_propertyUpdated.clear();
}
