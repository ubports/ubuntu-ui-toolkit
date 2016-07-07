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
#include "ucaction.h"

namespace UbuntuToolkit {

class UCBottomEdgeStyle;
class UBUNTUTOOLKIT_EXPORT UCBottomEdgePrivate : public UCStyledItemBasePrivate, protected QQuickItemChangeListener
{
    Q_DECLARE_PUBLIC(UCBottomEdge)

public:
    UCBottomEdgePrivate();

    static UCBottomEdgePrivate *get(UCBottomEdge *item)
    {
        return item->d_func();
    }
    void init();

    void completeComponentInitialization() override;

    // data property
    QQmlListProperty<QObject> data();
    static void overload_data_append(QQmlListProperty<QObject> *, QObject *);
    static void overload_data_clear(QQmlListProperty<QObject> *);

    // range funcs
    void appendRegion(UCBottomEdgeRegion *range);
    void clearRegions(bool destroy);
    void validateRegion(UCBottomEdgeRegion *region, int regionsSize = -1);

    // page header manipulation
    void patchContentItemHeader();
    void updateProgressionStates(qreal distance);
    bool setActiveRegion(UCBottomEdgeRegion *range);
    void detectDirection(qreal currentDistance);
    void setDragDirection(UCBottomEdge::DragDirection direction);
    void onDragEnded();
    void commit(qreal to);

    // panel positioning
    void setDragProgress(qreal position);
    // internal setters
    void setStatus(UCBottomEdge::Status status);

    // from UCStyledItemBase
    bool loadStyleItem(bool animated = true) override;
    // from QQuickItemChangeListener
    void itemChildAdded(QQuickItem *item, QQuickItem *child) override;
    void itemChildRemoved(QQuickItem *item, QQuickItem *child) override;

    void setCurrentContent();
    void resetCurrentContent(QQuickItem *newItem);
    // members
    QList<UCBottomEdgeRegion*> regions;
    QPointer<QQuickItem> currentContentItem;
    UCBottomEdgeRegion *defaultRegion;
    UCBottomEdgeRegion *activeRegion;
    UCBottomEdgeHint *hint;
    UCBottomEdgeStyle *bottomPanel;

    qreal previousDistance;
    qreal dragProgress;
    UCBottomEdge::Status status;

    enum OperationStatus {
        Idle,
        CommitToTop,
        CommitToRegion,
        Collapsing
    };
    OperationStatus operationStatus;
    UCBottomEdge::DragDirection dragDirection;

    bool defaultRegionsReset:1;
    bool mousePressed:1;
    bool preloadContent:1;

    // status management
    void setOperationStatus(OperationStatus s);
    bool isLocked()
    {
        return operationStatus > Idle;
    }
};

class UBUNTUTOOLKIT_EXPORT UCCollapseAction : public UCAction
{
    Q_OBJECT
public:
    UCCollapseAction(QObject *parent = 0);
    void activate();
};

UT_NAMESPACE_END

#endif // UCBOTTOMEDGE_P_H
