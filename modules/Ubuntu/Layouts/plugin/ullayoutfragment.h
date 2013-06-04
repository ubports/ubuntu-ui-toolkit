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

#ifndef ULFRAGMENT_H
#define ULFRAGMENT_H

#include <QtQuick/QQuickItem>

class ULLayoutFragment : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(ULLayoutFragment)

    Q_PROPERTY(QString item READ item WRITE setItem)

public:
    explicit ULLayoutFragment(QQuickItem *parent = 0);
    ~ULLayoutFragment();

    QString item() const;
    void setItem(const QString &value);

    QStringList changedProperties() const;

private Q_SLOTS:
    void markPropertyChange();

private:
    QString m_itemName;
    QSet<QString> m_changedProperties;
};

#endif // ULFRAGMENT_H
