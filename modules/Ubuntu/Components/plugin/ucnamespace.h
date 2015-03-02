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

#ifndef UCNAMESPACE_H
#define UCNAMESPACE_H

#include <QObject>

class UCNamespace : public QObject
{
    Q_OBJECT
    Q_ENUMS(CaptionsStyle)
public:
    enum CaptionsStyle {
        TitleCaptionStyle = 5000,
        SummaryCaptionStyle
    };
    explicit UCNamespace(QObject *parent = 0);

};

#endif // UCNAMESPACE_H
