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

#include "propertychanges_p.h"
#include "ullayouts_p.h"
#include "ullayouts.h"
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlInfo>

#include <QtQml/private/qqmlcontext_p.h>
#include <QtQuick/private/qquickanchors_p.h>

/******************************************************************************
 * PropertyAction
 * Saves property state, applies target values or bindings and restores property
 * saved values. Distinguishes Binding and Value types, which is relevant when
 * the property is restored to its defaults.
 */
PropertyAction::PropertyAction()
    : type(Value)
    , fromBinding(0)
    , toValueSet(false)
    , deleteFromBinding(false)
    , deleteToBinding(false)
{
}

PropertyAction::PropertyAction(const PropertyAction &other)
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

PropertyAction::PropertyAction(QObject *item, const QString &name, Type type)
    : type(type)
    , property(item, name, qmlContext(item))
    , fromBinding(QQmlPropertyPrivate::binding(property))
    , fromValue(property.read())
    , toValueSet(false)
    , deleteFromBinding(false)
    , deleteToBinding(false)
{
}

PropertyAction::PropertyAction(QObject *item, const QString &name, QQmlContext *context, const QVariant &value, Type type)
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

/*
 * Set target value.
 */
void PropertyAction::setValue(const QVariant &value)
{
    toValue = value;
    toValueSet = true;
}

/*
 * Set target binding and whether it can be deleted upon reset.
 */
void PropertyAction::setTargetBinding(QQmlAbstractBinding *binding, bool deletable)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    toBinding = binding;
#else
    toBinding = QQmlAbstractBinding::getPointer(binding);
#endif
    deleteToBinding = deletable;
}


/*
 * Apply property action by setting the target binding (toBinding) or by setting the
 * target value if the value is set.
 */
void PropertyAction::apply()
{
    if (toBinding) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
        QQmlAbstractBinding::Ptr binding(QQmlPropertyPrivate::binding(property));
        QQmlPropertyPrivate::setBinding(property, toBinding.data());
#else
        QQmlAbstractBinding *binding = QQmlPropertyPrivate::setBinding(property, toBinding.data());
#endif
        if (binding != fromBinding || (binding == fromBinding && deleteFromBinding)) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            if (binding == fromBinding) {
                fromBinding.reset();
                fromBinding=0;
            }
            binding->removeFromObject();
#else
            binding->destroy();
            if (binding == fromBinding) {
                fromBinding = 0;
            }
#endif
        }
    } else if (toValueSet) {
        if (!property.object()->setProperty(property.name().toLocal8Bit(), toValue)) {
            qmlInfo(property.object()) << "Layouts: updating property \""
                                      << property.name()
                                      << "\" failed.";
        }
    }
}

/*
 * Resets the hosted property and removes its bindings.
 */
void PropertyAction::reset()
{
    property.reset();
    if (fromBinding) {
        QQmlPropertyPrivate::setBinding(property, 0);
        if (deleteFromBinding) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            fromBinding->removeFromObject();
            fromBinding.reset();
#else
            fromBinding->destroy();
            fromBinding = 0;
#endif
            deleteFromBinding = false;
        }
    }
}

/*
 * Reverts the property to its defaults. Restores original bindings and in case
 * of Value types restores their original values.
 */
void PropertyAction::revert(bool reset)
{
    if (reset) {
        property.reset();
    }
    if (fromBinding) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
        QQmlAbstractBinding::Ptr revertedBinding(QQmlPropertyPrivate::binding(property));
        QQmlPropertyPrivate::setBinding(property, fromBinding.data());
#else
        QQmlAbstractBinding *revertedBinding = QQmlPropertyPrivate::setBinding(property, fromBinding);
#endif

        if (revertedBinding && (revertedBinding != fromBinding) && ((revertedBinding != toBinding.data()) || (revertedBinding == toBinding.data() && deleteToBinding))) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            revertedBinding->removeFromObject();
            revertedBinding.reset();
#else
            revertedBinding->destroy();
#endif
        }
    } else if (toBinding && QQmlPropertyPrivate::binding(property) == toBinding.data()) {
        QQmlPropertyPrivate::setBinding(property, 0);
        if (deleteToBinding) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            toBinding->removeFromObject();
            toBinding.reset();
#else
            toBinding.data()->destroy();
            toBinding.clear();
#endif
            deleteToBinding = false;
        }
    } else if (property.isValid() && fromValue.isValid() && (type == Value)) {
        property.write(fromValue);
    }
}

/******************************************************************************
 * PropertyChange
 * Default low priority change.
 */
PropertyChange::PropertyChange(Priority priority)
    : actionPriority(priority)
    , resetOnRevert(true)
{
}

PropertyChange::PropertyChange(QQuickItem *target, const QString &property, const QVariant &value, Priority priority)
    : actionPriority(priority)
    , resetOnRevert(true)
    , action(target, property, PropertyAction::Value)
{
    if (value.isValid()) {
        action.setValue(value);
    }
}

PropertyChange::PropertyChange(QQuickItem *target, const QString &property, const QQmlScriptString &script, QQmlContext *scriptContext, Priority priority)
    : actionPriority(priority)
    , resetOnRevert(true)
    , action(target, property, PropertyAction::Value)
{
    if (!script.isEmpty()) {
        bool ok = false;
        qreal value = script.numberLiteral(&ok);
        if (ok) {
            action.setValue(value);
        } else {
            QQmlBinding *binding = new QQmlBinding(script, target, scriptContext);
            binding->setTarget(action.property);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            action.toBinding = binding;
#else
            action.toBinding = QQmlAbstractBinding::getPointer(binding);
#endif
            action.deleteToBinding = true;
        }
    }
}

void PropertyChange::saveState()
{
}

void PropertyChange::apply()
{
    action.apply();
}

void PropertyChange::revert()
{
    action.revert(resetOnRevert);
}


/******************************************************************************
 * PropertyBackup
 */
PropertyBackup::PropertyBackup(QQuickItem *target, const QString &property)
    : PropertyChange(target, property, QVariant(), High)
{
}


/******************************************************************************
 * ParentChange
 * Normal priority change, a PropertyChange reparenting an item to the target.
 */
ParentChange::ParentChange(QQuickItem *item, QQuickItem *targetParent, bool topmostChild)
    : PropertyChange(item, "parent", qVariantFromValue(targetParent), Normal)
    , newParent(targetParent)
    , topmostChild(topmostChild)
{
}

void ParentChange::apply()
{
    // get child items before reparenting
    QList<QQuickItem*> items = newParent->childItems();

    PropertyChange::apply();

    if (topmostChild && (items.count() > 0)) {
        // move the hosted item as topmost child
        QQuickItem *item = static_cast<QQuickItem*>(action.property.object());
        item->stackBefore(items[0]);
    }
}

/******************************************************************************
 * AnchorChange
 * Low priority change for anchoring
 */
AnchorChange::AnchorChange(QQuickItem *item, const QString &anchor, QQuickItem *target, const QString &targetAnchor)
    : PropertyChange(item, "anchors." + anchor, QVariant())
    , active(false)
{
    QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
    // check the special cases, like fill, centerIn
    if (anchor != "fill" || (anchor == "fill" && !anchors->fill())) {
        active = true;
        if (targetAnchor.isEmpty()) {
            action.setValue(qVariantFromValue(target));
        } else {
            action.setValue(target->property(QString("anchors." + targetAnchor).toLocal8Bit()));
        }
    }
}

void AnchorChange::apply()
{
    if (!active)
        return;
    PropertyChange::apply();
}

void AnchorChange::revert()
{
    if (!active)
        return;
    PropertyChange::revert();
}

/******************************************************************************
 * ItemStackBackup
 * High priority change backing up the item's stack position.
 */
ItemStackBackup::ItemStackBackup(QQuickItem *item)
    : PropertyChange(High)
    , target(item)
    , prevItem(0)
{
}

void ItemStackBackup::saveState()
{
    QQuickItem *rewindParent = target->parentItem();
    if (!rewindParent) {
        return;
    }
    // save original stack position, but detect layout objects!
    QList<QQuickItem*> children = rewindParent->childItems();
    int index = children.indexOf(target);
    if (index > 0) {
        prevItem = children.at(index - 1);
    }
}

void ItemStackBackup::revert()
{
    if (prevItem) {
        target->stackAfter(prevItem);
    }
}

/******************************************************************************
 * AnchorBackup
 * High priority change backing up item anchors and margins.
 */
AnchorBackup::AnchorBackup(QQuickItem *item)
    : PropertyChange(item, "anchors", QVariant(), High)
    , anchorsObject(action.fromValue.value<QQuickAnchors*>())
    , used(anchorsObject->usedAnchors())
{
    if ((used & QQuickAnchors::LeftAnchor) == QQuickAnchors::LeftAnchor) {
        actions << PropertyAction(item, "anchors.left")
                << PropertyAction(item, "anchors.leftMargin", PropertyAction::Value);
    }
    if ((used & QQuickAnchors::RightAnchor) == QQuickAnchors::RightAnchor) {
        actions << PropertyAction(item, "anchors.right")
                << PropertyAction(item, "anchors.rightMargin", PropertyAction::Value);
    }
    if ((used & QQuickAnchors::TopAnchor) == QQuickAnchors::TopAnchor) {
        actions << PropertyAction(item, "anchors.top")
                << PropertyAction(item, "anchors.topMargin", PropertyAction::Value);
    }
    if ((used & QQuickAnchors::BottomAnchor) == QQuickAnchors::BottomAnchor) {
        actions << PropertyAction(item, "anchors.bottom")
                << PropertyAction(item, "anchors.bottomMargin", PropertyAction::Value);
    }
    if ((used & QQuickAnchors::HCenterAnchor) == QQuickAnchors::HCenterAnchor) {
        actions << PropertyAction(item, "anchors.horizontalCenter")
                << PropertyAction(item, "anchors.horizontalCenterOffset", PropertyAction::Value);
    }
    if ((used & QQuickAnchors::VCenterAnchor) == QQuickAnchors::VCenterAnchor) {
        actions << PropertyAction(item, "anchors.verticalCenter")
                << PropertyAction(item, "anchors.verticalCenterOffset", PropertyAction::Value);
    }
    if ((used & QQuickAnchors::BaselineAnchor) == QQuickAnchors::BaselineAnchor) {
        actions << PropertyAction(item, "anchors.baseline")
                << PropertyAction(item, "anchors.baselineOffset", PropertyAction::Value);
    }

    if (anchorsObject->fill()) {
        actions << PropertyAction(item, "anchors.fill")
                << PropertyAction(item, "anchors.margins", PropertyAction::Value);
    }
    if (anchorsObject->centerIn()) {
        actions << PropertyAction(item, "anchors.centerIn")
                << PropertyAction(item, "anchors.alignWhenCentered", PropertyAction::Value);
    }
}

void AnchorBackup::saveState()
{
    // no need to call superclass' saveState() as we don't touch the anchor property
    // only its properties
}

void AnchorBackup::apply()
{
    // reset all anchors
    if (!used) {
        return;
    }

    for (int i = 0; i < actions.count(); i++) {
        actions[i].reset();
    }
}

void AnchorBackup::revert()
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
    QList<PropertyChange*> list = unifiedChanges();
    for (int i = 0; i < list.count(); i++) {
        list[i]->apply();
    }
}

void ChangeList::revert()
{
    // reverse order of apply()
    QList<PropertyChange*> list = unifiedChanges();
    for (int i = list.count() - 1; i >= 0; i--) {
        list[i]->revert();
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

ChangeList &ChangeList::addChange(PropertyChange *change)
{
    if (change && (change->priority() < PropertyChange::MaxPriority)) {
        change->saveState();
        changes[change->priority()] << change;
    }
    return *this;
}

// creates two changes, one for reparenting and one for itemstack backup
ChangeList &ChangeList::addParentChange(QQuickItem *item, QQuickItem *newParent, bool topmostItem)
{
    return addChange(new ParentChange(item, newParent, topmostItem))
            .addChange(new ItemStackBackup(item));
}

QList<PropertyChange*> ChangeList::unifiedChanges()
{
    QList<PropertyChange*> list;
    for (int priority = PropertyChange::High; priority < PropertyChange::MaxPriority; priority++) {
        list << changes[priority];
    }
    return list;
}
