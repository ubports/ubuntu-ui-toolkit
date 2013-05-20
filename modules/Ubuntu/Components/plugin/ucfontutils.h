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
 */

#ifndef UCFONTUTILS_H
#define UCFONTUTILS_H

#include <QtCore/QObject>
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>

class UCFontUtils : public QObject
{
    Q_OBJECT
public:

    static UCFontUtils& instance()
    {
        static UCFontUtils instance;
        return instance;
    }

    explicit UCFontUtils(QObject *parent = 0) : QObject(parent) {}

    Q_INVOKABLE qreal sizeToPixels(const QString &size);
    Q_INVOKABLE qreal modularScale(const QString &size);
};

#endif // UCFONTUTILS_H
