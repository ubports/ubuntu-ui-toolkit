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

#include "layoutaction_p.h"
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlInfo>

#include <QtQml/private/qqmlcontext_p.h>
#include <QtQuick/private/qquickanchors_p.h>

#include <QtQuick/private/qquickstateoperations_p.h>
#include <QtQuick/private/qquickstatechangescript_p.h>
#include <QtQuick/private/qquickpropertychanges_p.h>
#include <QtQuick/private/qquickstate_p.h>


/******************************************************************************
 * LayoutAction
 */
LayoutAction::LayoutAction()
    : type(Value)
    , fromBinding(0)
    , toValueSet(false)
    , deleteFromBinding(false)
    , deleteToBinding(false)
{}

LayoutAction::LayoutAction(const LayoutAction &other)
    : type(other.type)
    , property(other.property)
    , fromBinding(other.fromBinding)
    , toBinding(other.toBinding)
    , fromValue(other.fromValue)
    , toValue(other.toValue)
    , toValueSet(other.toValueSet)
    , deleteFromBinding(other.deleteFromBinding)
    , deleteToBinding(other.deleteToBinding)
{
}

LayoutAction::LayoutAction(QObject *item, const QString &name, Type type)
    : type(type)
    , property(item, name, qmlEngine(item))
    , fromBinding(QQmlPropertyPrivate::binding(property))
    , fromValue(property.read())
    , toValueSet(false)
    , deleteFromBinding(false)
    , deleteToBinding(false)
{
}

LayoutAction::LayoutAction(QObject *item, const QString &name, QQmlContext *context, const QVariant &value, Type type)
    : type(type)
    , property(item, name, context)
    , fromBinding(QQmlPropertyPrivate::binding(property))
    , fromValue(property.read())
    , toValue(value)
    , toValueSet(value.isValid())
    , deleteFromBinding(false)
    , deleteToBinding(false)
{
}

void LayoutAction::setValue(const QVariant &value)
{
    toValue = value;
    toValueSet = true;
}

void LayoutAction::apply()
{
    if (toBinding) {
        QQmlAbstractBinding *binding = QQmlPropertyPrivate::setBinding(property, toBinding.data());
        if (binding != fromBinding || (binding == fromBinding && deleteFromBinding)) {
            binding->destroy();
            if (binding == fromBinding) {
                fromBinding = 0;
            }
        }
    } else if (toValueSet) {
        qDebug() << "set" << property.name() << toValue;
        if (!property.object()->setProperty(property.name().toLocal8Bit(), toValue)) {
            // FIXME: why does the QQmlProperty::write() give binding loop?
//        if (!property.write(toValue)) {
            qmlInfo(property.object()) << "Layouts: updating property \""
                                      << property.name()
                                      << "\" failed.";
        }
    }
}

void LayoutAction::reset()
{
    property.reset();
    if (fromBinding) {
        QQmlPropertyPrivate::setBinding(property, 0);
        if (deleteFromBinding) {
            fromBinding->destroy();
            fromBinding = 0;
            deleteFromBinding = false;
        }
    }
}

void LayoutAction::revert(bool reset)
{
    if (reset) {
        property.reset();
    }
    if (fromBinding) {
        QQmlAbstractBinding *revertedBinding = QQmlPropertyPrivate::setBinding(property, fromBinding);
        if (revertedBinding && ((revertedBinding != toBinding.data()) || (revertedBinding == toBinding.data() && deleteToBinding))) {
            revertedBinding->destroy();
        }
    } else if (!toBinding.isNull() && QQmlPropertyPrivate::binding(property) == toBinding.data()) {
        QQmlPropertyPrivate::setBinding(property, 0);
        if (deleteToBinding) {
            toBinding.data()->destroy();
            toBinding.clear();
            deleteToBinding = false;
        }
    } else if (property.isValid() && fromValue.isValid() && (type == Value)) {
        property.write(fromValue);
    }
}

/******************************************************************************
 * PropertyChange
 */
PropertyChange::PropertyChange(QQuickItem *target, const QString &property, const QVariant &value, Priority priority)
    : actionPriority(priority)
    , originalBinding(0)
    , targetBinding(0)
    , targetProperty(target, property, qmlContext(target))
    , originalValue(targetProperty.read())
    , targetValue(value)
{
}

void PropertyChange::saveState()
{
    if (targetProperty.isValid()) {
        originalBinding = QQmlPropertyPrivate::binding(targetProperty);
    }
}

void PropertyChange::execute()
{
    if (!targetProperty.write(targetValue)) {
        qmlInfo(targetProperty.object()) << "Updating property \"" << targetProperty.name()
                                         << "\" failed.";
    }
}

void PropertyChange::revert()
{
    restoreProperty(targetProperty, originalBinding, originalValue);
}

void PropertyChange::saveProperty(const QQmlProperty &property, QQmlAbstractBinding **binding, QVariant &value)
{
    (*binding) = QQmlPropertyPrivate::binding(property);
    if (!(*binding)) {
        value = property.read();
    }
}
void PropertyChange::restoreProperty(const QQmlProperty &property, QQmlAbstractBinding *binding, const QVariant &value)
{
    if (!restoreBinding(property, binding)) {
        property.write(value);
    }
}
bool PropertyChange::restoreBinding(const QQmlProperty &property, QQmlAbstractBinding *binding)
{
    if (binding) {
        QQmlAbstractBinding *revertedBinding = QQmlPropertyPrivate::setBinding(property, binding);
        if (revertedBinding && (revertedBinding != binding)) {
            revertedBinding->destroy();
        }
    }
    return (binding != 0);
}



/******************************************************************************
 * ReparentChange
 * TODO: Save parent, x, y, width, height, scale and rotation
 */
ReparentChange::ReparentChange(QQuickItem *target, QQuickItem *source)
    : PropertyChange(target, "parent", QVariant(), Normal)
    , sourceProperty(source, "parent", qmlContext(source))
{
}

void ReparentChange::saveState()
{
    targetValue = sourceProperty.read();
    PropertyChange::saveState();
    if (sourceProperty.isValid()) {
        targetBinding = QQmlPropertyPrivate::binding(sourceProperty);
    }
}

void ReparentChange::execute()
{
    if (targetBinding) {
        QQmlPropertyPrivate::setBinding(targetProperty, targetBinding, QQmlPropertyPrivate::DontRemoveBinding);
        return;
    }
    // this is most likely not reached
    PropertyChange::execute();
}


/******************************************************************************
 * ParentChange
 */
ParentChange::ParentChange(QQuickItem *target, QQuickItem *targetParent)
    : PropertyChange(target, "parent", qVariantFromValue(targetParent), Normal)
    , originalStackBefore(0)
    , container(targetParent)
{
    actions << LayoutAction(target, "x", LayoutAction::Value)
            << LayoutAction(target, "y", LayoutAction::Value)
            << LayoutAction(target, "width", LayoutAction::Value)
            << LayoutAction(target, "height", LayoutAction::Value)
            << LayoutAction(target, "scale", LayoutAction::Value)
            << LayoutAction(target, "rotation", LayoutAction::Value);
}

void ParentChange::saveState()
{
    PropertyChange::saveState();

    QQuickItem *target = static_cast<QQuickItem*>(targetProperty.object());
    QQuickItem *rewindParent = target->parentItem();
    // save original stack position
    QList<QQuickItem*> children = rewindParent->childItems();
    for (int ii = 0; ii < children.count() - 1; ++ii) {
        if (children.at(ii) == target) {
            originalStackBefore = children.at(ii + 1);
            break;
        }
    }

    if (container->inherits("QQuickColumn")) {
        actions[0].setValue(0.0);
    } else if (container->inherits("QQuickRow")) {
        actions[1].setValue(0.0);
    } else if (container->inherits("QQuickFlow") || container->inherits("QQuickGrid")) {
        qDebug() << "other positioner";
        actions[0].setValue(0.0);
        actions[1].setValue(0.0);
    }
}

void ParentChange::execute()
{
    PropertyChange::execute();
    // need to reset x, y coordinates
    for (int i = 0; i < actions.count(); i++) {
        actions[i].apply();
    }
}

void ParentChange::revert()
{
    // need to reset x, y coordinates
    for (int i = 0; i < actions.count(); i++) {
        actions[i].revert(true);
    }

    PropertyChange::revert();

    QQuickItem *target = static_cast<QQuickItem*>(targetProperty.object());
    if (originalStackBefore) {
        target->stackBefore(originalStackBefore);
    }
}

/******************************************************************************
 * AnchorChange
 */
AnchorChange::AnchorChange(QQuickItem *target)
    : PropertyChange(target, "anchors", QVariant(), High)
    , anchorsObject(originalValue.value<QQuickAnchors*>())
    , used(anchorsObject->usedAnchors())
{
    actions << LayoutAction(target, "anchors.left")
            << LayoutAction(target, "anchors.right")
            << LayoutAction(target, "anchors.top")
            << LayoutAction(target, "anchors.bottom")
            << LayoutAction(target, "anchors.horizontalCenter")
            << LayoutAction(target, "anchors.verticalCenter")
            << LayoutAction(target, "anchors.baseline")

            << LayoutAction(target, "anchors.margins", LayoutAction::Value)
            << LayoutAction(target, "anchors.leftMargin", LayoutAction::Value)
            << LayoutAction(target, "anchors.rightMargin", LayoutAction::Value)
            << LayoutAction(target, "anchors.topMargin", LayoutAction::Value)
            << LayoutAction(target, "anchors.bottomMargin", LayoutAction::Value)
            << LayoutAction(target, "anchors.horizontalCenterOffset", LayoutAction::Value)
            << LayoutAction(target, "anchors.verticalCenterOffset", LayoutAction::Value)
            << LayoutAction(target, "anchors.baselineOffset", LayoutAction::Value);
}

void AnchorChange::saveState()
{
    // no need to call superclass' saveState() as we don't touch the anchor property
    // only its properties
}

void AnchorChange::execute()
{
    // reset all anchors
    if (!used) {
        return;
    }

    for (int i = 0; i < actions.count(); i++) {
        actions[i].reset();
    }
}

void AnchorChange::revert()
{
    // revert all anchors
    if (!used) {
        return;
    }

    for (int i = 0; i < actions.count(); i++) {
        actions[i].revert(true);
    }
}

/******************************************************************************
 * ChangeList
 */

ChangeList::~ChangeList()
{
    clear();
}

void ChangeList::apply()
{
    for (int priority = PropertyChange::High; priority < PropertyChange::MaxPriority; priority++) {
        for (int change = 0; change < changes[priority].count(); change++) {
            changes[priority][change]->execute();
        }
    }
}

void ChangeList::revert()
{
    // reverse order of apply()
    for (int priority = PropertyChange::Low; priority >= PropertyChange::High; priority--) {
        for (int change = changes[priority].count() - 1; change >= 0; --change) {
            changes[priority][change]->revert();
        }
    }
}

void ChangeList::clear()
{
    for (int priority = PropertyChange::High; priority < PropertyChange::MaxPriority; priority++) {
        for (int change = 0; change < changes[priority].count(); change++) {
            delete changes[priority][change];
        }
        changes[priority].clear();
    }
}

ChangeList &ChangeList::operator<<(PropertyChange *action)
{
    if (action && (action->priority() < PropertyChange::MaxPriority)) {
        action->saveState();
        changes[action->priority()] << action;
    }

    return *this;
}
