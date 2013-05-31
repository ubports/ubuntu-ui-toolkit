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
 * ActionList
 */
QmlProperty::QmlProperty(QQuickItem *item, const QString &name)
    : property(item, name, qmlContext(item))
    , binding(QQmlPropertyPrivate::binding(property))
{
}

QmlProperty::QmlProperty(const QmlProperty &other)
    : property(other.property)
    , binding(other.binding)
{
}

void QmlProperty::reset()
{
    property.reset();
    if (binding) {
        QQmlPropertyPrivate::setBinding(property, 0);
    }
}

bool QmlProperty::revert()
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
 * ActionList
 */

ActionList::~ActionList()
{
    clear();
}

void ActionList::apply()
{
    for (int priority = PropertyAction::High; priority < PropertyAction::MaxPriority; priority++) {
        for (int change = 0; change < changes[priority].count(); change++) {
            changes[priority][change]->execute();
        }
    }
}

void ActionList::revert()
{
    // reverse order of apply()
    for (int priority = PropertyAction::Low; priority >= PropertyAction::High; priority--) {
        for (int change = changes[priority].count() - 1; change >= 0; --change) {
            changes[priority][change]->revert();
        }
    }
}

void ActionList::clear()
{
    for (int priority = PropertyAction::High; priority < PropertyAction::MaxPriority; priority++) {
        for (int change = 0; change < changes[priority].count(); change++) {
            delete changes[priority][change];
        }
        changes[priority].clear();
    }
}

ActionList &ActionList::operator<<(PropertyAction *action)
{
    if (action && (action->priority() < PropertyAction::MaxPriority)) {
        action->saveState();
        changes[action->priority()] << action;
    }

    return *this;
}


/******************************************************************************
 * PropertyAction
 */
PropertyAction::PropertyAction(QQuickItem *target, const QString &property, const QVariant &value, Priority priority)
    : actionPriority(priority)
    , originalBinding(0)
    , targetBinding(0)
    , targetProperty(target, property, qmlContext(target))
    , originalValue(targetProperty.read())
    , targetValue(value)
{
}

void PropertyAction::saveState()
{
    if (targetProperty.isValid()) {
        originalBinding = QQmlPropertyPrivate::binding(targetProperty);
    }
}

void PropertyAction::execute()
{
    if (!targetProperty.write(targetValue)) {
        qmlInfo(targetProperty.object()) << "Updating property \"" << targetProperty.name()
                                         << "\" failed.";
    }
}

void PropertyAction::revert()
{
    restoreProperty(targetProperty, originalBinding, originalValue);
}

void PropertyAction::saveProperty(const QQmlProperty &property, QQmlAbstractBinding **binding, QVariant &value)
{
    (*binding) = QQmlPropertyPrivate::binding(property);
    if (!(*binding)) {
        value = property.read();
    }
}
void PropertyAction::restoreProperty(const QQmlProperty &property, QQmlAbstractBinding *binding, const QVariant &value)
{
    if (!restoreBinding(property, binding)) {
        property.write(value);
    }
}
bool PropertyAction::restoreBinding(const QQmlProperty &property, QQmlAbstractBinding *binding)
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
 * ParentAction
 * TODO: Save parent, x, y, width, height, scale and rotation
 */
ParentAction::ParentAction(QQuickItem *target, QQuickItem *source)
    : PropertyAction(target, "parent", QVariant(), Normal)
    , sourceProperty(source, "parent", qmlContext(source))
{
}

void ParentAction::saveState()
{
    targetValue = sourceProperty.read();
    PropertyAction::saveState();
    if (sourceProperty.isValid()) {
        targetBinding = QQmlPropertyPrivate::binding(sourceProperty);
    }
}

void ParentAction::execute()
{
    if (targetBinding) {
        QQmlPropertyPrivate::setBinding(targetProperty, targetBinding, QQmlPropertyPrivate::DontRemoveBinding);
        return;
    }
    // this is most likely not reached
    PropertyAction::execute();
}


/******************************************************************************
 * ReparentAction
 */
ReparentAction::ReparentAction(QQuickItem *target, QQuickItem *targetParent)
    : PropertyAction(target, "parent", qVariantFromValue(targetParent), Normal)
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

void ReparentAction::saveState()
{
    PropertyAction::saveState();

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

void ReparentAction::execute()
{
    PropertyAction::execute();
    // need to reset x, y coordinates
    QQuickItem *target = static_cast<QQuickItem*>(targetProperty.object());
    target->setX(0.0);
    target->setY(0.0);
}

void ReparentAction::revert()
{
    // need to reset x, y coordinates
    QQuickItem *target = static_cast<QQuickItem*>(targetProperty.object());
    restoreProperty(xProperty, xBinding, originalX);
    restoreProperty(yProperty, yBinding, originalY);
    restoreProperty(widthProperty, widthBinding, originalWidth);
    restoreProperty(heightProperty, heightBinding, originalHeight);
    restoreProperty(scaleProperty, scaleBinding, originalScale);
    restoreProperty(rotationProperty, rotationBinding, originalRotation);
    PropertyAction::revert();
    if (originalStackBefore) {
        target->stackBefore(originalStackBefore);
    }
}

/******************************************************************************
 * AnchorAction
 */
AnchorPropertyAction::AnchorPropertyAction(QQuickItem *target, const QString &property, const QVariant &value)
    : PropertyAction(target, property, value, Low)
{
}

void AnchorPropertyAction::saveState()
{
    PropertyAction::saveState();
    QQuickAnchors *anchors = originalValue.value<QQuickAnchors*>();
    QQuickAnchors::Anchors used = anchors->usedAnchors();

}

void AnchorPropertyAction::execute()
{
    qDebug() << "resetting" << targetProperty.name() << targetProperty.reset();
    // remove the binding
    QQmlPropertyPrivate::setBinding(targetProperty, 0);
}

void AnchorPropertyAction::revert()
{
    PropertyAction::revert();
}

/******************************************************************************
 * AnchorResetAction
 */
AnchorBackupAction::AnchorBackupAction(QQuickItem *target)
    : PropertyAction(target, "anchors", QVariant(), High)
    , anchorsObject(originalValue.value<QQuickAnchors*>())
    , used(anchorsObject->usedAnchors())
    , fill(target, "anchors.fill")
    , centerIn(target, "anchors.centerIn")
{
    anchors[Left] = QmlProperty(target, "anchors.left");
    anchors[Right] = QmlProperty(target, "anchors.right");
    anchors[Top] = QmlProperty(target, "anchors.top");
    anchors[Bottom] = QmlProperty(target, "anchors.bottom");
    anchors[HCenter] = QmlProperty(target, "anchors.horizontalCenter");
    anchors[VCenter] = QmlProperty(target, "anchors.verticalCenter");
    anchors[Baseline] = QmlProperty(target, "anchors.baseline");

    margins[Margins] = QmlProperty(target, "anchors.margins");
    margins[Left] = QmlProperty(target, "anchors.leftMargin");
    margins[Right] = QmlProperty(target, "anchors.rightMargin");
    margins[Top] = QmlProperty(target, "anchors.topMargin");
    margins[Bottom] = QmlProperty(target, "anchors.bottomMargin");
    margins[HCenterOffset] = QmlProperty(target, "anchors.horizontalCenterOffset");
    margins[VCenterOffset] = QmlProperty(target, "anchors.verticalCenterOffset");
    margins[BaselineOffset] = QmlProperty(target, "anchors.baselineOffset");
}

void AnchorBackupAction::saveState()
{
    // no need to call superclass' saveState() as we don't touch the anchor property
    // only its properties
}

void AnchorBackupAction::execute()
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

void AnchorBackupAction::revert()
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
