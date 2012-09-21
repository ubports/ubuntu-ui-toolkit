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
    activeStyle(0),
    styleItem(0),
    componentContext(0),
    visualsItem(0)
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
        Style *tmp = ThemeEngine::lookupStyle(q);
        if (activeStyle != tmp) {
            styleItem = 0;
            activeStyle = tmp;
            styleChanged = true;
        }
    }
    if (styleChanged && activeStyle) {
        //styleItem = activeStyle->style()->create(componentContext);
        // check if we have the context
        if (!componentContext) {
            componentContext = new QDeclarativeContext(QDeclarativeEngine::contextForObject(q));
            componentContext->setContextProperty("control", q);
        }

        if (!styleItem && activeStyle->style()) {
            styleItem = activeStyle->style()->create(componentContext);
        }

        // do not mandate yet the existence of visuals
        if (!visualsItem) {
            QDeclarativeComponent *visuals = activeStyle->visuals();
            if (!visuals) {
                // use meta class name to search for the visuals selector
                qDebug() << "YUMM";
            }
            if (visuals) {
                // create visuals component
                visualsItem = qobject_cast<QDeclarativeItem*>(visuals->create(componentContext));
                if (visualsItem) {
                    visualsItem->setParentItem(q);

                    // If style item contains a property "contentItem" that points
                    // to an item, reparent all children into it:
                    QVariant contentVariant = visualsItem->property("contentItem");
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
    activeStyle = 0;
    styleItem = 0;
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

Style *StyledItem::activeStyle() const
{
    Q_D(const StyledItem);
    return d->activeStyle;
}

void StyledItem::setActiveStyle(Style *style)
{
    Q_D(StyledItem);
    if (d->activeStyle != style) {

        d->privateStyle = (style != 0);

        d->activeStyle = style;
        d->updateCurrentStyle(true);
    }
}

QObject *StyledItem::styleItem() const
{
    Q_D(const StyledItem);
    return d->styleItem;
}

QDeclarativeItem *StyledItem::visualsItem() const
{
    Q_D(const StyledItem);
    return d->visualsItem;
}


#include "moc_styleditem.cpp"
