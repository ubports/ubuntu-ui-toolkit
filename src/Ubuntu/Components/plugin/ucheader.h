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
 */

#ifndef UCHEADER_H
#define UCHEADER_H

#include "ucstyleditembase.h"
#include <QtCore/QPointer>
class QQuickFlickable;

class UCHeader : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(QQuickFlickable* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
    //    Q_PROPERTY(bool visible READ visible WRITE setVisible)

public:
    explicit UCHeader(QQuickItem *parent = 0);

    QQuickFlickable* flickable();
    void setFlickable(QQuickFlickable* newFlickable);

Q_SIGNALS:
    void flickableChanged();

private:
    bool m_visible;
    QPointer<QQuickFlickable> m_flickable;
};

#endif // UCHEADER_H
