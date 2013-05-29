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

#include <QtQml/private/qqmlcontext_p.h>
#include <QtQuick/private/qquickstateoperations_p.h>
#include <QtQuick/private/qquickstatechangescript_p.h>
#include <QtQuick/private/qquickstate_p.h>

LayoutAction::LayoutAction(ActionType type)
    : actionType(type)
    , originalBinding(0)
    , targetBinding(0)
{
}

void LayoutAction::initialize()
{
    if (targetProperty.isValid()) {
        originalBinding = QQmlPropertyPrivate::binding(targetProperty);
        if (!originalBinding) {
            originalValue = targetProperty.read();
        }
    }
}

void LayoutAction::execute()
{
    qDebug() << "write to" << targetProperty.name() << targetProperty.write(targetValue);
}

void LayoutAction::revert()
{
    if (originalBinding) {
        QQmlPropertyPrivate::setBinding(targetProperty, originalBinding, QQmlPropertyPrivate::DontRemoveBinding);
    } else {
        targetProperty.write(originalValue);
    }
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
    // first apply breakers
    for (int i = 0; i < breakerChanges.count(); i++) {
        breakerChanges[i]->execute();
    }
    // then updaters
    for (int i = 0; i < updaterChanges.count(); i++) {
        updaterChanges[i]->execute();
    }
}

void ActionList::revert()
{
    // reverse order of apply()
    for (int i = updaterChanges.count() - 1; i >= 0; --i) {
        updaterChanges[i]->revert();
    }
    for (int i = breakerChanges.count() - 1; i >= 0; --i) {
        breakerChanges[i]->revert();
    }
}

void ActionList::clear()
{
    for (int i = 0; i < breakerChanges.count(); i++) {
        delete breakerChanges[i];
    }
    for (int i = 0; i < updaterChanges.count(); i++) {
        delete updaterChanges[i];
    }
    breakerChanges.clear();
    updaterChanges.clear();
}

ActionList &ActionList::operator<<(LayoutAction *action)
{
    if (action) {
        switch (action->type()) {
        case LayoutAction::Breaker:
            breakerChanges << action;
            break;
        case LayoutAction::Updater:
            updaterChanges << action;
            break;
        default:
            break;
        }
    }

    return *this;
}


/******************************************************************************
 * PropertyAction
 */
PropertyAction::PropertyAction(QQuickItem *target, const QString &property, const QVariant &value)
    : LayoutAction(Updater)
{
    targetProperty = QQmlProperty(target, property, qmlContext(target));
    targetValue = value;
    initialize();
}

/******************************************************************************
 * ParentAction
 * TODO: Save parent, x, y, width, height, scale and rotation
 */
ParentAction::ParentAction(QQuickItem *target, const QString &property, QQuickItem *source)
    : LayoutAction(Updater)
    , sourceProperty(source, property, qmlContext(source))
{
    targetProperty = QQmlProperty(target, property, qmlContext(target));
    targetValue = sourceProperty.read();
    initialize();
}

void ParentAction::execute()
{
    if (sourceProperty.isValid()) {
        targetBinding = QQmlPropertyPrivate::binding(sourceProperty);
        if (targetBinding) {
            QQmlPropertyPrivate::setBinding(targetProperty, targetBinding, QQmlPropertyPrivate::DontRemoveBinding);
            return;
        }
    }
    // this is most likely not reached
    LayoutAction::execute();
}


/******************************************************************************
 * AnchorAction
 */
AnchorAction::AnchorAction(QQuickItem *target, const QString &property)
    : LayoutAction(Breaker)
{
    targetProperty = QQmlProperty(target, "anchors." + property, qmlContext(target));
    //targetValue = qVariantFromValue(QQuickAnchorLine());
    initialize();
}

void AnchorAction::execute()
{
    qDebug() << "resetting" << targetProperty.name() << targetProperty.reset();
    // remove the binding
    QQmlPropertyPrivate::setBinding(targetProperty, 0);
}


/******************************************************************************
 * AnchorResetAction
 */
AnchorResetAction::AnchorResetAction(QQuickItem *target)
    : LayoutAction(Breaker)
    , anchors(0)
{
    originalValue = target->property("anchors");
    anchors = originalValue.value<QQuickAnchors*>();
//    QQuickAnchors::Anchors used = anchors->usedAnchors();
//    if (used & QQuickAnchors::LeftAnchor) {
//        anchorLines.insert(QQuickAnchors::LeftAnchor, target->property("anchors.left"));
//    }
//    if (used & QQuickAnchors::RightAnchor) {
//        anchorLines.insert(QQuickAnchors::RightAnchor, target->property("anchors.right"));
//    }
//    if (used & QQuickAnchors::TopAnchor) {
//        anchorLines.insert(QQuickAnchors::TopAnchor, target->property("anchors.top"));
//    }
//    if (used & QQuickAnchors::BottomAnchor) {
//        anchorLines.insert(QQuickAnchors::BottomAnchor, target->property("anchors.bottom"));
//    }
}

void AnchorResetAction::execute()
{
//    if (targetProperty.isValid()) {
//        // save anchor lines
//        QQuickAnchors::Anchors used = anchors->usedAnchors();
//        if (used & QQuickAnchors::LeftAnchor) {
//            anchors->resetLeft();
//        }
//        if (used & QQuickAnchors::RightAnchor) {
//            anchors->resetRight();
//        }
//        if (used & QQuickAnchors::TopAnchor) {
//            anchors->resetTop();
//        }
//        if (used & QQuickAnchors::BottomAnchor) {
//            anchors->resetBottom();
//        }
//    }
}

void AnchorResetAction::revert()
{
//    if (targetProperty.isValid() && anchors) {
//        QHashIterator<QQuickAnchors::Anchor, QQuickAnchorLine> i(anchorLines);
//        while (i.hasNext()) {
//            i.next();
//            if (i.key() == QQuickAnchors::LeftAnchor) {
//                anchors->setLeft(QQuickAnchorLine(i.value().item, i.value().anchorLine));
//            }
//            if (i.key() == QQuickAnchors::RightAnchor) {
//                anchors->setRight(QQuickAnchorLine(i.value().item, i.value().anchorLine));
//            }
//            if (i.key() == QQuickAnchors::TopAnchor) {
//                anchors->setTop(QQuickAnchorLine(i.value().item, i.value().anchorLine));
//            }
//            if (i.key() == QQuickAnchors::BottomAnchor) {
//                anchors->setBottom(QQuickAnchorLine(i.value().item, i.value().anchorLine));
//            }
//        }
//    }
}
