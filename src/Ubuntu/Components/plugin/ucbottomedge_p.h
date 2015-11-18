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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCBOTTOMEDGE_P_H
#define UCBOTTOMEDGE_P_H

#include "ucbottomedge.h"
#include "ucstyleditembase_p.h"

class UCBottomEdgeStyle;
class UCBottomEdgePrivate : public UCStyledItemBasePrivate, protected QQuickItemChangeListener
{
    Q_DECLARE_PUBLIC(UCBottomEdge)

public:
    UCBottomEdgePrivate();

    static UCBottomEdgePrivate *get(UCBottomEdge *item)
    {
        return item->d_func();
    }
    void init();

    // data property
    QQmlListProperty<QObject> data();
    static void overload_data_append(QQmlListProperty<QObject> *, QObject *);
    static void overload_data_clear(QQmlListProperty<QObject> *);

    // range funcs
    void appendRange(UCBottomEdgeRange *range);
    void clearRanges(bool destroy);

    // page header manipulation
    void patchContentItemHeader();
    void createDefaultRanges();
    void updateProgressionStates();
    bool setActiveRange(UCBottomEdgeRange *range);
    void detectDirection(qreal currentPanelY);
    void setDragDirection(UCBottomEdge::DragDirection direction);

    // panel positioning
    void positionPanel(qreal position);
    // internal setters
    void setState(UCBottomEdge::State state);

    // from UCStyledItemBase
    bool loadStyleItem(bool animated = true) override;
    void _q_styleResized() override {}
    // from QQuickItemChangeListener
    void itemChildAdded(QQuickItem *item, QQuickItem *child);
    void itemChildRemoved(QQuickItem *item, QQuickItem *child);

    // members
    QUrl contentUrl;
    QList<UCBottomEdgeRange*> ranges;
    UCBottomEdgeRange *activeRange;
    UCBottomEdgeHint *hint;
    QQmlComponent *contentComponent;
    UCBottomEdgeStyle *bottomPanel;

    qreal previousPanelY;
    UCBottomEdge::State state;

    enum OperationStatus {
        Idle,
        CommitToTop,
        CommitToRange,
        Collapsing
    };
    OperationStatus operationStatus;
    UCBottomEdge::DragDirection dragDirection;

    bool defaultRangesReset:1;

    // status management
    void setOperationStatus(OperationStatus s)
    {
        operationStatus = s;
    }
    bool isLocked()
    {
        return operationStatus > Idle;
    }
};

#endif // UCBOTTOMEDGE_P_H
