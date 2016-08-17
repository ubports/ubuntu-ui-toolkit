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
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UCViewItemsAttached;
class UCListItem;
class ListItemSelection : public QObject
{
    Q_OBJECT
public:
    explicit ListItemSelection(UCListItem *parent = 0);

    void attachToViewItems(UCViewItemsAttached * newViewItems);

    bool inSelectMode() const;
    void setSelectMode(bool mode);

    bool isSelected() const;
    void setSelected(bool selected);

    void onSelectModeChanged();
    void onSelectedIndicesChanged(const QList<int> &indices);

private:
    QPointer<UCViewItemsAttached> viewItems;
    UCListItem *hostItem;
    enum {
        SelectModeDirty = 0x01,
        SelectedDirty = 0x02
    };
    quint8 dirtyFlags;
    bool selectMode:1;
    bool selected:1;

    void syncWithViewItems();
};

UT_NAMESPACE_END

#endif // LISTITEMSELECTION_H
