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
    activeStyle(0),
    styleItem(0)
{
}

void StyledItemPrivate::updateCurrentStyle()
{
    Q_Q(StyledItem);
    // do not do anything till the component gets complete?
    //if (!componentCompleted) return;

    bool emitSignal = false;

    // in case of private style is in use, no need to change anything
    if (!privateStyle) {
        // check whether we have different style for the state
        Style *tmp = ThemeEngine::lookupStyle(q);
        if (activeStyle != tmp) {
            styleItem = 0;
            activeStyle = tmp;
            emitSignal = true;
        }
    }
    if (!styleItem && activeStyle) {
        QDeclarativeContext *context = new QDeclarativeContext(QDeclarativeEngine::contextForObject(q));
        context->setContextProperty("control", q);
        styleItem = activeStyle->style()->create(context);
    }

    if (emitSignal)
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

    if (!d->privateStyle) {
        // check if the selector is built, if not, build it!
        if (d->localSelector.isEmpty())
            ThemeEngine::styledItemPropertiesToSelector(this);
    }

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
            ThemeEngine::styledItemPropertiesToSelector(this);
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
        ThemeEngine::styledItemPropertiesToSelector(this);
        d->updateCurrentStyle();
    }
}

QString StyledItem::selector() const
{
    Q_D(const StyledItem);
    return d->localSelector;
}
void StyledItem::setSelector(const QString &selector)
{
    Q_D(StyledItem);
    if (d->localSelector != selector) {
        d->localSelector = selector;
        ThemeEngine::styledItemSelectorToProperties(this);
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
        d->updateCurrentStyle();
        emit styleChanged();
    }
}

QObject *StyledItem::styleItem() const
{
    Q_D(const StyledItem);
    qDebug() << d->styleClass << d->componentCompleted;
    return d->styleItem;
}

#include "moc_styleditem.cpp"
