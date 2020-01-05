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

#ifndef COLORUTILS_P_H
#define COLORUTILS_P_H

#include <QtCore/QObject>

#include <UbuntuToolkit/ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT ColorUtils : public QObject
{
    Q_OBJECT
public:
    explicit ColorUtils(QObject *parent = 0);
    Q_INVOKABLE static qreal luminance(const QColor &color);
    Q_INVOKABLE static qreal contrast(const QColor &color);
    Q_INVOKABLE static qreal contrastRatio(const QColor &color1, const QColor &color2);
};

UT_NAMESPACE_END

#endif // COLORUTILS_P_H
