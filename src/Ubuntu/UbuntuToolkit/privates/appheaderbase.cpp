/*
 * Copyright 2016 Canonical Ltd.
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

#include "appheaderbase.h"
#include "ucstyleditembase_p.h"

UT_NAMESPACE_BEGIN

/*!
    \qmltype AppHeaderBase
    \instantiates UCAppHeaderBase
    \inherits UCHeader
    \internal
    \inqmlmodule Ubuntu.Components.Private 1.3
    \ingroup ubuntu
    \since Ubuntu.Components.Private 1.3
    \brief Private parent of AppHeader, to disable animations until pages are
        loaded and initial visibility of the header is set.
*/
UCAppHeaderBase::UCAppHeaderBase(QQuickItem *parent)
    : UCHeader(parent)
    , m_animate(true)
{
    connect(this, &UCHeader::themeChanged, this, &UCAppHeaderBase::themeChanged2);
    UCStyledItemBasePrivate::get(this)->styleDocument = "PageHeadStyle";
}

UCTheme* UCAppHeaderBase::theme2()
{
    return getTheme();
}

void UCAppHeaderBase::setTheme2(UCTheme* value)
{
    setTheme(value);
}

void UCAppHeaderBase::show(bool animate) {
    UCHeader::show(m_animate && animate);
}

void UCAppHeaderBase::hide(bool animate) {
    UCHeader::hide(m_animate && animate);
}

/*!
 * \qmlproperty bool AppHeaderBase::animate
 * Animate the y-value when the value of \l Header::exposed is set or unset.
 * If the header is already animating while the value is set to false,
 * the current animation is not cancelled.
 * Default value: true.
 */
void UCAppHeaderBase::setAnimate(bool animate) {
    if (animate != m_animate) {
        m_animate = animate;
        Q_EMIT animateChanged();
    }
}

UT_NAMESPACE_END
