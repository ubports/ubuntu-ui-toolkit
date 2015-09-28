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
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

#ifndef UCSLOTSLAYOUTPRIVATE_H
#define UCSLOTSLAYOUTPRIVATE_H

#include <private/qquickitem_p.h>

#include "ucslotslayout.h"

#define IMPLICIT_SLOTSLAYOUT_WIDTH_GU                40
#define IMPLICIT_SLOTSLAYOUT_HEIGHT_GU               7
#define SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU             1
#define SLOTSLAYOUT_SLOTS_TOPBOTTOMMARGINS_GU        0
#define SLOTSLAYOUT_LEFTMARGIN_GU                    1
#define SLOTSLAYOUT_RIGHTMARGIN_GU                   1

//we want a different top/bottom margin if any of the slot is taller than this
#define SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU 4
#define SLOTSLAYOUT_TOPMARGIN1_GU                    1
#define SLOTSLAYOUT_TOPMARGIN2_GU                    2
#define SLOTSLAYOUT_BOTTOMMARGIN1_GU                 1
#define SLOTSLAYOUT_BOTTOMMARGIN2_GU                 2

class UCSlotsLayoutPrivate : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(UCSlotsLayout)
public:
    UCSlotsLayoutPrivate();
    virtual ~UCSlotsLayoutPrivate();
    void init();
    void updateTopBottomPaddingIfNeeded();

    //returns true if we want to ignore "slot" during the layout process
    bool skipSlot(QQuickItem *slot);

    //utility method that scans a list and adds the slot after all the slots which
    //have same position
    void insertSlotIntoSortedList(QQuickItem *slot,
                                  UCSlotsLayout::UCSlotPosition desiredSlotPosition,
                                  QList<QQuickItem *> &slotsList);

    //add or remove a slot from the internal data structures
    void addSlot(QQuickItem *slot);
    void removeSlot(QQuickItem *slot);

    //layout "items" in a row, optionally anchoring the row to a sibling with margin siblingAnchorMargin
    //The optional anchoring behaviour can be disable by passing QQuickAnchorLine()
    void layoutInRow(qreal siblingAnchorMargin, QQuickAnchorLine siblingAnchor, QList<QQuickItem *> &items);

    //this method sets up vertical anchors and paddings for a slot ("item").
    //Attached properties are taken from "attached", if not null, otherwise
    //qml engine is queried.
    void setupSlotsVerticalPositioning(QQuickItem *item, UCSlotsAttached* attached = Q_NULLPTR);

    //We have two vertical positioning modes according to the visual design rules:
    //- RETURN VALUE CenterVertically --> All items have to be vertically centered
    //- RETURN VALUE AlignToTop --> All items have to anchor to the top of the listitem (using a top margin as well)
    //This is mainly used by the layout method
    enum UCSlotPositioningMode { AlignToTop, CenterVertically };
    UCSlotsLayoutPrivate::UCSlotPositioningMode getVerticalPositioningMode() const;

    //connect/disconnect item's UCSlotsAttached properties changes to/from the right slot
    void handleAttachedPropertySignals(QQuickItem *item, bool connect);

    static inline UCSlotsLayoutPrivate *get(UCSlotsLayout *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    void _q_onGuValueChanged();
    void _q_updateCachedHeight();
    void _q_updateProgressionStatus();
    void _q_updateGuValues();
    void _q_updateCachedMainSlotHeight();
    void _q_updateSlotsBBoxHeight();
    void _q_updateSize();
    void _q_onSlotWidthChanged();
    void _q_onSlotOverrideVerticalPositioningChanged();
    void _q_onSlotPositionChanged();
    void _q_relayout();

    UCSlotsLayoutPadding padding;

    //These lists are just to split the children() between leading
    //and trailing slots. Some of the slots in this lists may be
    //ignored during relayout, for example if they're not visible or
    //similar conditions. (see relayout implementation to make sure
    //what conditions we check before ignoring a slot)
    QList<QQuickItem *> leadingSlots;
    QList<QQuickItem *> trailingSlots;

    QQuickItem* mainSlot;

    //We cache the current parent so that we can disconnect from the signals when the
    //parent changes. We need this because itemChange(..) only provides the new parent
    QQuickItem *m_parentItem;

    qreal mainSlotHeight;
    //max slots height ignoring the main slot
    qreal maxSlotsHeight;
    //we cache the height so that we only relayout when it goes
    //from 0 to non-0 and not viceversa
    qreal _q_cachedHeight;

    //currently fixed, but we may allow changing this in the future
    qint32 maxNumberOfLeadingSlots;
    qint32 maxNumberOfTrailingSlots;

    //Show the chevron, name taken from old ListItem API to minimize changes
    bool progression : 1;
};

class UCSlotsAttachedPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UCSlotsAttached)
public:
    UCSlotsAttachedPrivate();

    static inline UCSlotsAttachedPrivate *get(UCSlotsAttached *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    void _q_onGuValueChanged();

    UCSlotsLayoutPadding padding;
    UCSlotsLayout::UCSlotPosition position;
    bool overrideVerticalPositioning : 1;
};

#endif // UCSLOTSLAYOUTPRIVATE_H
