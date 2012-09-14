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
    activeStyle(0)
{
}

void StyledItemPrivate::_q_updateCurrentStyle(const QString &state)
{
    // do not do anything till the component gets complete?
    //if (!componentCompleted) return;

    Style *tmp = 0;
    if (privateStyle) {
        tmp = activeStyle;
    } else {
        Q_Q(StyledItem);
        tmp = ThemeEngine::lookupStyle(q, state);
    }
    if ((tmp != activeStyle) && tmp) {
        Q_Q(StyledItem);
        activeStyle = tmp;
        emit q->styleChanged();
    }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
StyledItem::StyledItem(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    d_ptr(new StyledItemPrivate(this))
{
    QObject::connect(this, SIGNAL(stateChanged(const QString &)), this, SLOT(_q_updateCurrentStyle(const QString &)));
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

    // set the default style
    d->_q_updateCurrentStyle("");
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
        if (ThemeEngine::registerInstanceId(this, instanceId)) {
            d->instanceId = instanceId;
            ThemeEngine::styledItemPropertiesToSelector(this);
            d->_q_updateCurrentStyle(QString());
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
        d->_q_updateCurrentStyle(QString());
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
        d->_q_updateCurrentStyle(QString());
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
        d->_q_updateCurrentStyle(QString());
        emit styleChanged();
    }
}


#include "moc_styleditem.cpp"
