#include "ucmathutils.h"

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
    if (min <= max) {
        return qMax(min, qMin(x, max));
    } else {
        // swap min/max if min > max
        return clamp(x, max, min);
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
  \qmlmethod mathUtils::getFlickableChild(item)
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

QObject *UCMathUtils::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new UCMathUtils;
}

