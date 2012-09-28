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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "style.h"
#include "style_p.h"
#include "themeengine.h"
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeProperty>

/*!
  \class StyleRule
  \brief StyleRule component is used to define styling elements for the UITK.

*/

StyleRulePrivate::StyleRulePrivate(StyleRule *qq) :
    q_ptr(qq),
    style(0),
    delegate(0)
{
}

/*=============================================================================
=============================================================================*/

StyleRule::StyleRule(QObject *parent) :
    QObject(parent),
    d_ptr(new StyleRulePrivate(this))
{}

StyleRule::~StyleRule()
{}


void StyleRule::classBegin()
{

}

void StyleRule::componentComplete()
{
    // emit changed signal; this is needed when developer defines a private StyleRule
    // for a control
    emit ruleChanged();
}

QString StyleRule::selector() const
{
    Q_D(const StyleRule);
    return d->selector;
}
void StyleRule::setSelector(const QString &selector)
{
    Q_D(StyleRule);
    d->selector = selector;
}

QDeclarativeComponent *StyleRule::style()
{
    Q_D(const StyleRule);
    return d->style;
}
void StyleRule::setStyle(QDeclarativeComponent *style)
{
    Q_D(StyleRule);
    d->style = style;
}

QDeclarativeComponent *StyleRule::delegate()
{
    Q_D(const StyleRule);
    return d->delegate;
}

void StyleRule::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(StyleRule);
    d->delegate = delegate;
}

#include "moc_style.cpp"
