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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include "styleditem.h"
#include "styleditem_p.h"
#include "style.h"
#include "style_p.h"
#include "themeengine.h"
#include <QDeclarativeProperty>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>

/*!
  \preliminary
  StyledItem class is a base class for all UI controls that use styling in their layout.
  The style to be used is selected based on the following principles:
  - hierarchy
  - classification
  - instance identification
  - property binding

  By default, styled elements use the style definition from a theme that is attached to
  them. This means that
*/

StyledItemPrivate::StyledItemPrivate(StyledItem *qq):
    q_ptr(qq),
    privateStyle(false),
    componentCompleted(false),
    styleRule(0),
    styleObject(0),
    componentContext(0),
    delegateItem(0)
{
}

void StyledItemPrivate::updateCurrentStyle(bool forceUpdate)
{
    Q_Q(StyledItem);
    // do not do anything till the component gets complete?
    //if (!componentCompleted) return;

    bool styleChanged = forceUpdate;

    // in case of private style is in use, no need to change anything
    if (!privateStyle) {
        // check whether we have different style for the state
        StyleRule *tmp = ThemeEngine::lookupStyleRule(q);
        if (styleRule != tmp) {
            styleObject = 0;
            styleRule = tmp;
            styleChanged = true;
        }
    }

    // reset delegate if the style is updated and the new style has visuals
    if (styleChanged && styleRule && styleRule->delegate())
        delegateItem = 0;

    if (styleChanged && styleRule) {
        // check if we have the context
        if (!componentContext) {
            componentContext = new QDeclarativeContext(QDeclarativeEngine::contextForObject(q));
            componentContext->setContextProperty("control", q);
        }

        if (!styleObject && styleRule->style()) {
            styleObject = styleRule->style()->create(componentContext);
            if (styleObject) {
                styleObject->setParent(q);
            }
        }

        // do not mandate yet the existence of visuals
        if (!delegateItem) {
            QDeclarativeComponent *visuals = styleRule->delegate();
            if (!visuals) {
                // reset
                StyleRule *delegateStyle = ThemeEngine::lookupStyleRule(q, true);
                if (delegateStyle)
                    visuals = delegateStyle->delegate();
            }
            if (visuals) {
                // create visuals component
                delegateItem = qobject_cast<QDeclarativeItem*>(visuals->create(componentContext));
                if (delegateItem) {
                    delegateItem->setParentItem(q);

                    // If style item contains a property "contentItem" that points
                    // to an item, reparent all children into it:
                    QVariant contentVariant = delegateItem->property("contentItem");
                    QDeclarativeItem *contentItem = qvariant_cast<QDeclarativeItem *>(contentVariant);
                    if (contentItem) {
                        foreach (QObject *child, q->children()) {
                            QDeclarativeItem *childItem = qobject_cast<QDeclarativeItem *>(child);
                            if (childItem)
                                childItem->setParentItem(contentItem);
                        }
                    }
                }
            }
        }
    }

    if (styleChanged)
        emit q->styleChanged();
}

void StyledItemPrivate::_q_reloadTheme()
{
    if (privateStyle)
        return;
    // update style if theme is used
    styleRule = 0;
    styleObject = 0;
    updateCurrentStyle();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
StyledItem::StyledItem(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    d_ptr(new StyledItemPrivate(this))
{
    QObject::connect(ThemeEngine::instance(), SIGNAL(themeChanged()), this, SLOT(_q_reloadTheme()));
}

StyledItem::~StyledItem()
{}

void StyledItem::componentComplete()
{
    QDeclarativeItem::componentComplete();
    Q_D(StyledItem);
    d->componentCompleted = true;

    // activate style
    d->updateCurrentStyle();
}

QString StyledItem::instanceId() const
{
    Q_D(const StyledItem);
    return d->instanceId;
}
void StyledItem::setInstanceId(const QString &instanceId)
{
    Q_D(StyledItem);
    if (instanceId != d->instanceId) {
        // this might not be necessary... let's see
        if (ThemeEngine::registerInstanceId(this, instanceId)) {
            d->instanceId = instanceId;
            d->updateCurrentStyle();
        } else {
            qWarning() << "instance" << instanceId << "already registered!";
        }
    }
}

QString StyledItem::styleClass() const
{
    Q_D(const StyledItem);
    return d->styleClass;
}
void StyledItem::setStyleClass(const QString &styleClass)
{
    Q_D(StyledItem);
    if (d->styleClass != styleClass) {
        d->styleClass = styleClass;
        d->updateCurrentStyle();
    }
}

StyleRule *StyledItem::styleRule() const
{
    Q_D(const StyledItem);
    return d->styleRule;
}

void StyledItem::setStyleRule(StyleRule *styleRule)
{
    Q_D(StyledItem);
    if (d->styleRule != styleRule) {

        d->privateStyle = (styleRule != 0);

        d->styleRule = styleRule;
        d->updateCurrentStyle(true);
    }
}

QObject *StyledItem::styleObject() const
{
    Q_D(const StyledItem);
    return d->styleObject;
}

QDeclarativeItem *StyledItem::delegateItem() const
{
    Q_D(const StyledItem);
    return d->delegateItem;
}


#include "moc_styleditem.cpp"
