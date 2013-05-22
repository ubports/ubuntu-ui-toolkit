/*
 * Copyright 2013 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef ULLAYOUTS_H
#define ULLAYOUTS_H

#include <QtQuick/QQuickItem>
#include "ulconditionallayout.h"

class ULLayoutsPrivate;
class ULLayouts : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(ULLayouts)

    Q_PROPERTY(QString currentLayout READ currentLayout NOTIFY currentLayoutChanged DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<ULConditionalLayout> layouts READ layouts DESIGNABLE false)
    
public:
    explicit ULLayouts(QQuickItem *parent = 0);
    ~ULLayouts();

Q_SIGNALS:
    void currentLayoutChanged();

private:
    QString currentLayout() const;
    QQmlListProperty<ULConditionalLayout> layouts();

private:
    Q_DECLARE_PRIVATE(ULLayouts)
    QScopedPointer<ULLayoutsPrivate> d_ptr;
};

QML_DECLARE_TYPE(ULLayouts)

#endif // ULLAYOUTS_H

