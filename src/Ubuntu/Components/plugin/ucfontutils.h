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

#ifndef UCFONTUTILS_H
#define UCFONTUTILS_H

#include <QtCore/QObject>

class UCFontUtils : public QObject
{
    Q_OBJECT

public:
    static constexpr float fontUnits = 14.0f;
    static constexpr float xxSmallScale = 0.606f;
    static constexpr float xSmallScale = 0.707f;
    static constexpr float smallScale = 0.857f;
    static constexpr float mediumScale = 1.0f;
    static constexpr float largeScale = 1.414f;
    static constexpr float xLargeScale = 1.905f;

    static UCFontUtils& instance()
    {
        static UCFontUtils instance;
        return instance;
    }

    explicit UCFontUtils(QObject *parent = 0) : QObject(parent) {}

    Q_INVOKABLE qreal sizeToPixels(const QString &size);
    Q_INVOKABLE qreal modularScale(const QString &size);
};

#define SCALE_CODE(size)    reinterpret_cast<int*>(size.toLatin1().data())[0]
#define SCALE_MEDIUM        0x6964656d // "medi"
#define SCALE_LARGE         0x6772616c // "larg"
#define SCALE_SMALL         0x6c616d73 // "smal"
#define SCALE_XLARGE        0x616c2d78 // "x-la"
#define SCALE_XSMALL        0x6d732d78 // "x-sm"
#define SCALE_XXSMALL       0x732d7878 // "xx-s"

#endif // UCFONTUTILS_H
