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

#include "colorutils_p.h"

#include <QtGui/QColor>

#include <math.h>

UT_NAMESPACE_BEGIN

ColorUtils::ColorUtils(QObject *parent) : QObject(parent)
{}

qreal ColorUtils::luminance(const QColor &color)
{
    return color.lightnessF();
}

/*
 * Contrast calculation
 * https://www.w3.org/TR/WCAG20-TECHS/G17.html#G17-procedure
 *
 * Based on the code by kirilloid
 * https://stackoverflow.com/questions/9733288/how-to-programmatically-calculate-the-contrast-ratio-between-two-colors
 */

qreal ColorUtils::contrast(const QColor &color)
{
    qreal redLight, greenLight, blueLight, colorLight;

    if (color.redF() <= 0.03928) {
        redLight = color.redF() / 12.92;
    } else {
        redLight = pow((color.redF() + 0.055) / 1.055, 2.4);
    }

    if (color.greenF() <= 0.03928) {
        greenLight = color.greenF() / 12.92;
    } else {
        greenLight = pow((color.greenF() + 0.055) / 1.055, 2.4);
    }

    if (color.blueF() <= 0.03928) {
        blueLight = color.blueF() / 12.92;
    } else {
        blueLight = pow((color.blueF() + 0.055) / 1.055, 2.4);
    }

    colorLight = redLight * 0.2126 + greenLight * 0.7152 + blueLight * 0.0722;

    return colorLight;
}

qreal ColorUtils::contrastRatio(const QColor &color1, const QColor &color2)
{
    return (contrast(color1) + 0.05) / (contrast(color2) + 0.05);
}

UT_NAMESPACE_END
