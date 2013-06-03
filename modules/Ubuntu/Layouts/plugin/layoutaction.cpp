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
 * ChangeList
 */
LayoutAction::LayoutAction(QQuickItem *item, const QString &name)
    : property(item, name, qmlContext(item))
    , binding(QQmlPropertyPrivate::binding(property))
{
}

LayoutAction::LayoutAction(const LayoutAction &other)
    : property(other.property)
    , binding(other.binding)
{
}

void LayoutAction::reset()
{
    property.reset();
    if (binding) {
        QQmlPropertyPrivate::setBinding(property, 0);
    }
}

bool LayoutAction::revert()
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
    , xProperty(target, "x", qmlContext(target))
    , yProperty(target, "y", qmlContext(target))
    , widthProperty(target, "width", qmlContext(target))
    , heightProperty(target, "height", qmlContext(target))
    , scaleProperty(target, "scale", qmlContext(target))
    , rotationProperty(target, "rotation", qmlContext(target))
    , originalStackBefore(0)
    , xBinding(0), yBinding(0), widthBinding(0), heightBinding(0), scaleBinding(0), rotationBinding(0)
    , originalX(0), originalY(0), originalWidth(0), originalHeight(0), originalScale(0), originalRotation(0)
{
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

    // save bindings for x, y, width, height, scale and rotation
    saveProperty(xProperty, &xBinding, originalX);
    saveProperty(yProperty, &yBinding, originalY);
    saveProperty(widthProperty, &widthBinding, originalWidth);
    saveProperty(heightProperty, &heightBinding, originalHeight);
    saveProperty(scaleProperty, &scaleBinding, originalScale);
    saveProperty(rotationProperty, &rotationBinding, originalRotation);
}

void ParentChange::execute()
{
    PropertyChange::execute();
    // need to reset x, y coordinates
    QQuickItem *target = static_cast<QQuickItem*>(targetProperty.object());
    target->setX(0.0);
    target->setY(0.0);
}

void ParentChange::revert()
{
    // need to reset x, y coordinates
    QQuickItem *target = static_cast<QQuickItem*>(targetProperty.object());
    restoreProperty(xProperty, xBinding, originalX);
    restoreProperty(yProperty, yBinding, originalY);
    restoreProperty(widthProperty, widthBinding, originalWidth);
    restoreProperty(heightProperty, heightBinding, originalHeight);
    restoreProperty(scaleProperty, scaleBinding, originalScale);
    restoreProperty(rotationProperty, rotationBinding, originalRotation);
    PropertyChange::revert();
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
    , fill(target, "anchors.fill")
    , centerIn(target, "anchors.centerIn")
{
    anchors[Left] = LayoutAction(target, "anchors.left");
    anchors[Right] = LayoutAction(target, "anchors.right");
    anchors[Top] = LayoutAction(target, "anchors.top");
    anchors[Bottom] = LayoutAction(target, "anchors.bottom");
    anchors[HCenter] = LayoutAction(target, "anchors.horizontalCenter");
    anchors[VCenter] = LayoutAction(target, "anchors.verticalCenter");
    anchors[Baseline] = LayoutAction(target, "anchors.baseline");

    margins[Margins] = LayoutAction(target, "anchors.margins");
    margins[Left] = LayoutAction(target, "anchors.leftMargin");
    margins[Right] = LayoutAction(target, "anchors.rightMargin");
    margins[Top] = LayoutAction(target, "anchors.topMargin");
    margins[Bottom] = LayoutAction(target, "anchors.bottomMargin");
    margins[HCenterOffset] = LayoutAction(target, "anchors.horizontalCenterOffset");
    margins[VCenterOffset] = LayoutAction(target, "anchors.verticalCenterOffset");
    margins[BaselineOffset] = LayoutAction(target, "anchors.baselineOffset");
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

    fill.reset();
    centerIn.reset();
    margins[Margins].reset();

    for (int i = Left; i < MaxAnchor; i++) {
        anchors[i].reset();
        margins[i + 1].reset();
    }
}

void AnchorChange::revert()
{
    // revert all anchors
    if (!used) {
        return;
    }

    fill.revert();
    centerIn.revert();
    margins[Margins].revert();

    for (int i = Left; i < MaxAnchor; i++) {
        anchors[i].revert();
        margins[i + 1].revert();
    }
}
