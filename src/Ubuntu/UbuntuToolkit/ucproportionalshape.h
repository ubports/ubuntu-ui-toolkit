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

#ifndef UCPROPORTIONALSHAPE_H
#define UCPROPORTIONALSHAPE_H

#include "ucubuntushape.h"

namespace UbuntuToolkit {

class UBUNTUTOOLKIT_EXPORT UCProportionalShape : public UCUbuntuShape
{
    Q_OBJECT

public:
    UCProportionalShape(QQuickItem* parent=0);

private Q_SLOTS:
    void _q_updateWidth();
    void _q_updateHeight();

private:
    void componentComplete() override;

    Q_DISABLE_COPY(UCProportionalShape)
};

UT_NAMESPACE_END

QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(UCProportionalShape))

#endif  // UCPROPORTIONALSHAPE_H
