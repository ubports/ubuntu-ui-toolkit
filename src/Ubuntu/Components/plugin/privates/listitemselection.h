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

#ifndef LISTITEMSELECTION_H
#define LISTITEMSELECTION_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

class UCViewItemsAttached;
class UCListItem;
class ListItemSelection : public QObject
{
    Q_OBJECT
public:
    explicit ListItemSelection(QObject *parent = 0);

    void init(UCViewItemsAttached * viewItems);
    void syncToViewItems();

    bool inSelectMode() const;
    void setSelectMode(bool mode);

    bool isSelected() const;
    void setSelected(bool selected);

    Q_SLOT void updateSelectMode();
    Q_SLOT void updateSelected(const QList<int> &indices);

private:
    QPointer<UCViewItemsAttached> viewItems;
    UCListItem *hostItem;
    bool selectMode:1;
    bool selected:1;
};

#endif // LISTITEMSELECTION_H
