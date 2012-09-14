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
 */

#include "style.h"
#include "style_p.h"
#include "themeengine.h"


/*!
  \class Style
  \brief Style class is the root component of every styling component in UITK.
  It is a helper

*/

StylePrivate::StylePrivate(Style *qq) :
    q_ptr(qq),
    target(0),
    delegate(0)
{
}

/*=============================================================================
=============================================================================*/

Style::Style(QObject *parent) :
    QObject(parent),
    d_ptr(new StylePrivate(this))
{
}
Style::~Style()
{}

void Style::classBegin()
{

}

void Style::componentComplete()
{
    // update target if set
    Q_D(Style);
    d->target = ThemeEngine::instance()->lookupTarget(d->instanceId);
}

QString Style::styleClass() const
{
    Q_D(const Style);
    return d->styleClass;
}
void Style::setStyleClass(const QString &styleClass)
{
    Q_D(Style);
    if (styleClass != d->styleClass) {
        d->styleClass = styleClass;
        emit styleClassChanged();
    }
}

QString Style::instanceId() const
{
    Q_D(const Style);
    return d->instanceId;
}
void Style::setInstanceId(const QString &instanceId)
{
    Q_D(Style);
    if (instanceId != d->instanceId) {
        d->instanceId = instanceId;
        d->target = ThemeEngine::instance()->lookupTarget(d->instanceId);
        emit targetChanged();
    }
}

QDeclarativeItem* Style::target() const
{
    Q_D(const Style);
    return d->target;
}

QDeclarativeComponent *Style::delegate() const
{
    Q_D(const Style);
    return d->delegate;
}
void Style::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(Style);
    if (d->delegate != delegate) {
        d->delegate = delegate;
        emit delegateChanged();
    }
}

QDeclarativeListProperty<QObject> Style::data()
{
    Q_D(Style);
    return QDeclarativeListProperty<QObject>(this, d->data);
}

QStringList Style::states() const
{
    Q_D(const Style);
    return d->states;
}
void Style::setStates(const QStringList &states)
{
    Q_D(Style);
    if (d->states != states) {
        d->states = states;
        emit statesChanged();
    }
}

QDeclarativeListProperty<QDeclarativeState> Style::styleStates()
{
    Q_D(Style);
    return QDeclarativeListProperty<QDeclarativeState>(this, d->styleStates);
}

#include "moc_style.cpp"
