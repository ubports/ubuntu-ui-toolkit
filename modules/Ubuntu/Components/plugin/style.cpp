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
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeProperty>

/*!
  \class Style
  \brief Style class is the root component of every styling component in UITK.
  It is a helper

*/

StylePrivate::StylePrivate(Style *qq) :
    q_ptr(qq),
    style(0),
    visuals(0)
{
}

/*=============================================================================
=============================================================================*/

Style::Style(QObject *parent) :
    QObject(parent),
    d_ptr(new StylePrivate(this))
{}

Style::~Style()
{}


void Style::classBegin()
{

}

void Style::componentComplete()
{
}

Style::StyleTypes Style::styleType()
{
    Q_D(Style);
    StyleTypes ret = 0;
    if (d->style)
        ret |= ConfigurationStyle;
    if (d->visuals)
        ret |= VisualStyle;
    return ret;
}


QString Style::selector() const
{
    Q_D(const Style);
    return d->selector;
}
void Style::setSelector(const QString &selector)
{
    Q_D(Style);
    if (d->selector != selector) {
        d->selector = selector;
        emit styleChanged();
    }
}

QDeclarativeComponent *Style::style()
{
    Q_D(const Style);
    return d->style;
}
void Style::setStyle(QDeclarativeComponent *style)
{
    Q_D(Style);
    if (d->style != style) {
        d->style = style;
        emit styleChanged();
    }
}

QDeclarativeComponent *Style::visuals()
{
    Q_D(const Style);
    return d->visuals;
}

void Style::setVisuals(QDeclarativeComponent *visuals)
{
    Q_D(Style);
    if (d->visuals != visuals) {
        d->visuals = visuals;
        emit styleChanged();
    }
}

QDeclarativeListProperty<QObject> Style::data()
{
    Q_D(Style);
    return QDeclarativeListProperty<QObject>(this, d->data);
}

#include "moc_style.cpp"
