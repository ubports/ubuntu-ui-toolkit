/*
 * Copyright (C) 2012-2014 Canonical, Ltd.
 *
 * Authors:
 *   Michal Hruby <michal.hruby@canonical.com>
 *   Christian Dywan <christian.dywan@canonical.com>
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

#include "sortfiltermodel_p.h"

UT_NAMESPACE_BEGIN

/*!
 * \qmltype SortFilterModel
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief SortFilterModel sorts and filters rows from an existing model.
 *
 * The SortFilterModel takes an existing model such as a ListModel or
 * any QAbstractItemModel implementation. The original rows and role names
 * show up in the SortFilterModel with two basic differences. For one if
 * \l sort.property is set all rows will be sorted. Further more if
 * \l filter.property is set only rows matching the filter will be in the model.
 *
 * Example usage:
 * \qml
 * import QtQuick 2.9
 * import Ubuntu.Components 1.3
 * 
 * MainView {
 *     width: units.gu(80)
 *     height: units.gu(40)
 * 
 *     ListModel {
 *         id: movies
 *         ListElement {
 *             title: "Esign"
 *             producer: "Chris Larkee"
 *         }
 *         ListElement {
 *             title: "Elephants Dream"
 *             producer: "Blender"
 *         }
 *         ListElement {
 *             title: "Big Buck Bunny"
 *             producer: "blender"
 *         }
 *     }
 * 
 *     SortFilterModel {
 *         id: sortedMovies
 *         model: movies
 *         sort {
 *             property: "title"
 *             order: Qt.DescendingOrder
 *         }
 *         filter {
 *             property: "producer"
 *             //Add i for case insensitive
 *             pattern: /Blender/i
 *         }
 *     }
 * 
 *     ListView {
 *         model: sortedMovies
 *         anchors.fill: parent
 *         delegate: ListItemLayout {
 *             title.text: model.title
 *             subtitle.text: model.producer
 *         }
 * 
 *         section {
 *             property: "title"
 *             criteria: ViewSection.FirstCharacter
 *             delegate: Text {
 *                 text: i18n.tr(section)
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * Pay attention to the differences between the original model and the result:
 * \list
 *     \li Big Buck Bunny will be the first row, because it's sorted by title
 *     \li Esign won't be visible, because it's from the wrong producer
 * \endlist
 */


QSortFilterProxyModelQML::QSortFilterProxyModelQML(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // This is virtually always what you want in QML
    setDynamicSortFilter(true);
    connect(this, SIGNAL(modelReset()), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), SIGNAL(countChanged()));
    connect(&m_sortBehavior, &SortBehavior::propertyChanged, this, &QSortFilterProxyModelQML::sortChangedInternal);
    connect(&m_sortBehavior, &SortBehavior::orderChanged, this, &QSortFilterProxyModelQML::sortChangedInternal);
    connect(&m_filterBehavior, &FilterBehavior::propertyChanged, this, &QSortFilterProxyModelQML::filterChangedInternal);
    connect(&m_filterBehavior, &FilterBehavior::patternChanged, this, &QSortFilterProxyModelQML::filterChangedInternal);
}

int
QSortFilterProxyModelQML::roleByName(const QString& roleName) const
{
    const QByteArray roleNameByteArray = roleName.toUtf8();
    const QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> i(roles);
    while (i.hasNext()) {
        i.next();
        if (i.value() == roleNameByteArray)
            return i.key();
    }
    return 0;
}

/*!
 * \qmlproperty string SortFilterModel::sort.property
 *
 * If set to a valid role name, all rows will be sorted according to \l sort.order.
 */

/*!
 * \qmlproperty string SortFilterModel::sort.order
 *
 * The order, if \l sort.property is set.
 * Qt::AscendingOrder sorts results from A to Z or 0 to 9.
 * Qt::DescendingOrder sorts results from Z to A or 9 to 0.
 */

SortBehavior*
QSortFilterProxyModelQML::sortBehavior()
{
    return &m_sortBehavior;
}

/*!
 * \qmlproperty string SortFilterModel::filter.pattern
 *
 * The pattern all rows must match, if \l filter.property is set.
 *
 * Some examples:
 * \list
 *     \li /possible/ matches anywhere in a word, so both "impossible" and "possible".
 *     \li /^sign/ matches "sign". But not "assignment" because ^ means start.
 *     \li /vest$/ matches "safety vest" and "vest" but not "vested".
 *     \li /bar/i matches "bar", "Bar" or "BAR" regardless of case.
 * \endlist
 *
 * For more advanced uses it's recommended to read up on Javascript regular expressions.
 */

/*!
 * \qmlproperty string SortFilterModel::filter.property
 *
 * If set to a valid role name, only rows matching \l filter.pattern will be in the model.
 */

FilterBehavior*
QSortFilterProxyModelQML::filterBehavior()
{
    return &m_filterBehavior;
}

void
QSortFilterProxyModelQML::sortChangedInternal()
{
    setSortRole(roleByName(m_sortBehavior.property()));
    sort(sortColumn() != -1 ? sortColumn() : 0, m_sortBehavior.order());
    Q_EMIT sortChanged();
}

void
QSortFilterProxyModelQML::filterChangedInternal()
{
    setFilterRole(roleByName(m_filterBehavior.property()));
    setFilterRegExp(m_filterBehavior.pattern());
    Q_EMIT filterChanged();
}

QHash<int, QByteArray> QSortFilterProxyModelQML::roleNames() const
{
    return sourceModel() ? sourceModel()->roleNames() : QHash<int, QByteArray>();
}

/*!
 * \qmlproperty QAbstractItemModel SortFilterModel::model
 *
 * The source model to sort and/ or filter.
 */
void
QSortFilterProxyModelQML::setModel(QAbstractItemModel *itemModel)
{
    if (itemModel == NULL) {
        return;
    }

    if (itemModel != sourceModel()) {
        if (sourceModel() != NULL) {
            sourceModel()->disconnect(this);
        }

        setSourceModel(itemModel);
        // Roles mapping to role names may change
        setSortRole(roleByName(m_sortBehavior.property()));
        setFilterRole(roleByName(m_filterBehavior.property()));
        Q_EMIT modelChanged();
    }
}

QVariantMap
QSortFilterProxyModelQML::get(int row)
{
    QVariantMap res;
    const QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> i(roles);
    while (i.hasNext()) {
        i.next();
        res.insert(QString::fromUtf8(i.value()), index(row, 0).data(i.key()));
    }
    return res;
}

int
QSortFilterProxyModelQML::count()
{
    return rowCount();
}

bool
QSortFilterProxyModelQML::filterAcceptsRow(int sourceRow,
                                           const QModelIndex &sourceParent) const
{
    if (filterRegExp().isEmpty()) {
        return true;
    }

    bool result = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    return result;
}

UT_NAMESPACE_END
