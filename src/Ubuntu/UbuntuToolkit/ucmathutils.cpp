/*
 * Copyright 2015 Canonical Ltd.
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
 */

#include "ucmathutils.h"
#include <QDebug>

UT_NAMESPACE_BEGIN

/*!
  \qmltype mathUtils
  \inqmlmodule Ubuntu.Components
  \ingroup ubuntu
  \brief Various mathematical utility functions.
 */

UCMathUtils::UCMathUtils(QObject *parent) : QObject(parent)
{

}

/*!
  \qmlmethod mathUtils::clamp(x, min, max)
  Ensure the value x is between min and max
 */
double UCMathUtils::clamp(double x, double min, double max)
{
    if (!(min > max)) {
        return qBound(min, x, max);
    } else {
        // swap min/max if min > max
        return qBound(max, x, min);
    }
}

/*!
  \qmlmethod mathUtils::lerp(delta, from, to)
  Get the linear interpolation
 */
double UCMathUtils::lerp(double delta, double from, double to)
{
    return ((1.0 - delta) * from) + (delta * to);
}

/*!
  \qmlmethod mathUtils::projectValue(x, xmin, xmax, ymin, ymax)
  Linearly project a value x from [xmin, xmax] into [ymin, ymax]
 */
double UCMathUtils::projectValue(double x, double xmin, double xmax, double ymin, double ymax)
{
    return ((x - xmin) * ymax - (x - xmax) * ymin) / (xmax - xmin);
}

/*!
  \qmlmethod mathUtils::clampAndProject(x, xmin, xmax, ymin, ymax)
  Linearly project a value x, but in addition to projectValue it's clamped to xmin/xmax first
 */
double UCMathUtils::clampAndProject(double x, double xmin, double xmax, double ymin, double ymax)
{
    return projectValue(clamp(x, xmin, xmax), xmin, xmax, ymin, ymax);
}

UT_NAMESPACE_END
