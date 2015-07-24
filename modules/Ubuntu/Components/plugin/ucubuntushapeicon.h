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
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

#ifndef UCUBUNTUSHAPEICON_H
#define UCUBUNTUSHAPEICON_H

#include "ucubuntushape.h"

class UCUbuntuShapeIcon : public UCUbuntuShape
{
    Q_OBJECT

public:
    UCUbuntuShapeIcon(QQuickItem* parent=0);

private Q_SLOTS:
    void updateWidth();
    void updateHeight();

private:
    Q_DISABLE_COPY(UCUbuntuShapeIcon)
};

QML_DECLARE_TYPE(UCUbuntuShapeIcon)

#endif  // UCUBUNTUSHAPEICON_H
