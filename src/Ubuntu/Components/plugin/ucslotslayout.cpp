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

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlInfo>

#include "ucunits.h"
#include "ucslotslayout.h"
#include "ucslotslayout_p.h"
#include "ucfontutils.h"
#include "uctheme.h"
#include "unitythemeiconprovider.h"

/******************************************************************************
 * UCSlotsLayoutPrivate
 */
UCSlotsLayoutPrivate::UCSlotsLayoutPrivate()
    : QQuickItemPrivate()
    , mainSlot(Q_NULLPTR)
    , m_parentItem(Q_NULLPTR)
    , maxSlotsHeight(0)
    , _q_cachedHeight(-1)
    , maxNumberOfLeadingSlots(1)
    , maxNumberOfTrailingSlots(2)
{
}

UCSlotsLayoutPrivate::~UCSlotsLayoutPrivate()
{
}

void UCSlotsLayoutPrivate::init()
{
    Q_Q(UCSlotsLayout);

    _q_updateGuValues();

    // FIXME: This is not yet possible from C++ since we're not a StyledItem
    //QObject::connect(q, SIGNAL(themeChanged()),
    //                 q, SLOT(_q_onThemeChanged()), Qt::DirectConnection);

    QObject::connect(&padding, SIGNAL(leadingChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&padding, SIGNAL(trailingChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&padding, SIGNAL(topChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
    QObject::connect(&padding, SIGNAL(bottomChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));

    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_onGuValueChanged()));

    //FIXME (if possible): this will cause relayout to be called 4-5 times when the layout has "anchors.fill: parent"
    //defined on QML side
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(_q_relayout()));

    //we connect height changes to a different function, because height changes only cause a relayout
    //in some cases (for instance when height goes from 0 to non 0)
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(_q_updateCachedHeight()));

    QObject::connect(q, SIGNAL(visibleChanged()), q, SLOT(_q_relayout()));
}

UCSlotsLayoutPrivate::UCSlotPositioningMode UCSlotsLayoutPrivate::getVerticalPositioningMode() const
{
    return (mainSlotHeight > maxSlotsHeight)
            ? UCSlotPositioningMode::AlignToTop
            : UCSlotPositioningMode::CenterVertically;
}

void UCSlotsLayoutPrivate::updateTopBottomPaddingIfNeeded()
{
    if (!padding.topWasSetFromQml) {
        padding.setTop((getVerticalPositioningMode() == UCSlotPositioningMode::CenterVertically
                        && maxSlotsHeight > UCUnits::instance().gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU))
                       ? UCUnits::instance().gu(SLOTSLAYOUT_TOPMARGIN1_GU)
                       : UCUnits::instance().gu(SLOTSLAYOUT_TOPMARGIN2_GU));
    }

    if (!padding.bottomWasSetFromQml) {
        padding.setBottom((getVerticalPositioningMode() == UCSlotPositioningMode::CenterVertically
                           && maxSlotsHeight > UCUnits::instance().gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU))
                          ? UCUnits::instance().gu(SLOTSLAYOUT_BOTTOMMARGIN1_GU)
                          : UCUnits::instance().gu(SLOTSLAYOUT_BOTTOMMARGIN2_GU));
    }

    return;
}

bool UCSlotsLayoutPrivate::skipSlot(QQuickItem *slot)
{
    if (slot == Q_NULLPTR) {
        qDebug() << "skipSlot: NULL POINTER!";
        return true;
    }

    //this are the same conditions Row uses, we don't want
    //to check opacity because that may be used for animations,
    //We don't want to relayout if the opacity reaches 0 during an
    //animation, for instance.
    return slot->height() <= 0 || slot->width() <= 0
            || !slot->isVisible();
}

void UCSlotsLayoutPrivate::insertSlotIntoSortedList(QQuickItem *slot,
                                                    UCSlotsLayout::UCSlotPosition desiredSlotPosition,
                                                    QList<QQuickItem *> &slotsList)
{
    if (slot == Q_NULLPTR) {
        qDebug() << "insertSlotIntoSortedList: INVALID POINTER!";
        return;
    }

    //linear search, as these lists will be very short
    int i = 0;
    const int size = slotsList.length();
    for (i = 0; i < size; ++i) {
        UCSlotsAttached *attachedProperty =
                qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(slotsList.at(i)));

        if (!attachedProperty) {
            Q_Q(UCSlotsLayout);
            qmlInfo(q) << "Invalid attached property!";
            return;
        }

        if (attachedProperty->position() > desiredSlotPosition) {
            break;
        }
    }

    slotsList.insert(i, slot);
}

void UCSlotsLayoutPrivate::addSlot(QQuickItem *slot)
{
    if (slot == Q_NULLPTR) {
        qDebug() << "addSlot: INVALID POINTER!";
        return;
    }

    Q_Q(UCSlotsLayout);
    UCSlotsAttached *attachedProperty =
            qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(slot));
    if (!attachedProperty) {
        qmlInfo(q) << "Invalid attached property!";
        return;
    }

    if (attachedProperty->position() <= 0) {
        insertSlotIntoSortedList(slot, attachedProperty->position(), leadingSlots);
    } else {
        insertSlotIntoSortedList(slot, attachedProperty->position(), trailingSlots);
    }
}

void UCSlotsLayoutPrivate::removeSlot(QQuickItem *slot)
{
    if (slot == Q_NULLPTR) {
        qDebug() << "addSlot: INVALID POINTER!";
        return;
    }

    Q_Q(UCSlotsLayout);
    UCSlotsAttached *attachedProperty =
            qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(slot));
    if (!attachedProperty) {
        qmlInfo(q) << "Invalid attached property!";
        return;
    }

    if (attachedProperty->position() <= 0) {
        leadingSlots.removeAll(slot);
    } else {
        trailingSlots.removeAll(slot);
    }
}

void UCSlotsLayoutPrivate::_q_onGuValueChanged()
{
    _q_updateCachedMainSlotHeight();
    _q_updateSlotsBBoxHeight();
    _q_updateGuValues();
}

void UCSlotsLayoutPrivate::_q_updateCachedHeight()
{
    Q_Q(UCSlotsLayout);
    if (_q_cachedHeight != q->height()) {
        if (qIsNull(_q_cachedHeight)) {
            _q_relayout();
        }
        _q_cachedHeight = q->height();
    }
}

void UCSlotsLayoutPrivate::_q_updateGuValues()
{
    Q_Q(UCSlotsLayout);

    if (!padding.leadingWasSetFromQml) {
        padding.setLeading(UCUnits::instance().gu(SLOTSLAYOUT_LEFTMARGIN_GU));
    }

    if (!padding.trailingWasSetFromQml) {
        padding.setTrailing(UCUnits::instance().gu(SLOTSLAYOUT_RIGHTMARGIN_GU));
    }

    updateTopBottomPaddingIfNeeded();

    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateCachedMainSlotHeight()
{
    //if the component is not ready the QML properties may not have been evaluated yet,
    //it's not worth doing anything if that's the case
    if (!componentComplete)
        return;

    Q_Q(UCSlotsLayout);

    if (mainSlot) {
        UCSlotsAttached *attachedProperty =
                qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(mainSlot));

        if (!attachedProperty) {
            qmlInfo(q) << "Invalid attached property!";
            mainSlotHeight = 0;
            return;
        }
        mainSlotHeight = mainSlot->height() + attachedProperty->padding()->top()
                + attachedProperty->padding()->bottom();
    } else {
        mainSlotHeight = 0;
    }

    updateTopBottomPaddingIfNeeded();
    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateSlotsBBoxHeight()
{
    if (!componentComplete)
        return;

    Q_Q(UCSlotsLayout);

    qreal maxSlotsHeightTmp = 0;
    const int numOfLeading = leadingSlots.count();
    const int numOfTrailing = trailingSlots.count();
    int numOfLeadingToLayout = 0;
    int numOfTrailingToLayout = 0;
    //instead of having 2 cycles we only make one which iterates over both lists
    for (int i = 0; i < numOfLeading + numOfTrailing; i++) {
        QQuickItem *child = i < numOfLeading
                ? leadingSlots.at(i)
                : trailingSlots.at(i - numOfLeading);

        bool skipSlotFlag = skipSlot(child);
        if (i < numOfLeading) {
            if (numOfLeadingToLayout >= maxNumberOfLeadingSlots) {
                skipSlotFlag = true;
            } else {
                if (!skipSlotFlag)
                    numOfLeadingToLayout++;
            }
        } else {
            if (numOfTrailingToLayout >= maxNumberOfTrailingSlots) {
                skipSlotFlag = true;
            } else {
                if (!skipSlotFlag)
                    numOfTrailingToLayout++;
            }
        }
        if (!skipSlotFlag) {
            UCSlotsAttached *attachedProperty =
                    qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(child));

            if (!attachedProperty) {
                qmlInfo(q) << "Invalid attached property!";
                continue;
            }

            //ignore children which have custom vertical positioning
            if (!attachedProperty->overrideVerticalPositioning())
                maxSlotsHeightTmp = qMax<qreal>(maxSlotsHeightTmp, child->height()
                                                + attachedProperty->padding()->top()
                                                + attachedProperty->padding()->bottom());
        }
    }
    maxSlotsHeight = maxSlotsHeightTmp;

    updateTopBottomPaddingIfNeeded();
    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateSize()
{
    if (!componentComplete)
        return;

    Q_Q(UCSlotsLayout);
    q->setImplicitWidth(parentItem ? parentItem->width() : UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_WIDTH_GU));
    q->setImplicitHeight(qMax<qreal>(maxSlotsHeight, mainSlotHeight)
                         + padding.top() + padding.bottom());

    _q_relayout();
}

void UCSlotsLayoutPrivate::_q_onSlotWidthChanged()
{
    Q_Q(UCSlotsLayout);
    QQuickItem *slot = qobject_cast<QQuickItem*>(q->sender());
    if (slot == Q_NULLPTR) {
        qDebug() << "onSlotWidthChanged: NULL SLOT";
        return;
    }

    //if the width is 0, we should also update the max slots height
    //as this item is now considered as if it were !visible
    if (!slot->width()) {
        _q_updateSlotsBBoxHeight();
    } else {
        _q_relayout();
    }
}

void UCSlotsLayoutPrivate::_q_onSlotOverrideVerticalPositioningChanged()
{
    Q_Q(UCSlotsLayout);
    UCSlotsAttached *sender = qobject_cast<UCSlotsAttached *>(q->sender());
    if (sender == Q_NULLPTR) {
        qDebug() << "onSlotOverrideVerticalPositioningChanged: NULL SENDER";
        return;
    }

    QQuickItem *slot = qobject_cast<QQuickItem*>(sender->parent());
    if (slot == Q_NULLPTR) {
        qDebug() << "onSlotOverrideVerticalPositioningChanged: NULL SLOT";
        return;
    }

    QQuickAnchors *slotAnchors = QQuickItemPrivate::get(slot)->anchors();
    slotAnchors->resetTop();
    slotAnchors->resetTopMargin();
    slotAnchors->resetBottom();
    slotAnchors->resetBottomMargin();
    slotAnchors->resetVerticalCenter();
    slotAnchors->setVerticalCenterOffset(0);
    slotAnchors->resetFill();
    slotAnchors->resetCenterIn();

    _q_updateSlotsBBoxHeight();
}

void UCSlotsLayoutPrivate::_q_onSlotPositionChanged()
{
    Q_Q(UCSlotsLayout);
    UCSlotsAttached *sender = qobject_cast<UCSlotsAttached *>(q->sender());
    if (sender == Q_NULLPTR) {
        qDebug() << "onSlotPositionChanged: NULL SENDER";
        return;
    }

    QQuickItem *slot = qobject_cast<QQuickItem*>(sender->parent());
    if (slot == Q_NULLPTR) {
        qDebug() << "onSlotPositionChanged: NULL SLOT";
        return;
    }

    //The slot may have changed position within the same group of slots
    //(i.e. it is still a leading or still a trailing slot) or it may
    //have switched from one group to the other. In any case,
    //remove the slot and add it back in the correct place.
    //We remove it from both the arrays as we don't know what its old
    //position was
    leadingSlots.removeAll(slot);
    trailingSlots.removeAll(slot);
    addSlot(slot);

    _q_relayout();
}

void UCSlotsLayoutPrivate::setupSlotsVerticalPositioning(QQuickItem *slot, UCSlotsAttached* attached)
{
    if (slot == Q_NULLPTR)
        return;

    UCSlotsAttached* attachedProps = attached;
    if (attached == Q_NULLPTR) {
        attachedProps = qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(slot));

        if (attachedProps == Q_NULLPTR) {
            Q_Q(UCSlotsLayout);
            qmlInfo(q) << "Invalid attached property!";
            return;
        }
    }

    QQuickAnchors *slotAnchors = QQuickItemPrivate::get(slot)->anchors();
    if (getVerticalPositioningMode() == UCSlotPositioningMode::AlignToTop) {
        //reset the vertical anchor as we might be transitioning from the configuration
        //where all items are vertically centered to the one where they're anchored to top
        slotAnchors->resetVerticalCenter();
        slotAnchors->setVerticalCenterOffset(0);

        slotAnchors->setTop(top());
        slotAnchors->setTopMargin(padding.top() + attachedProps->padding()->top());
    } else {
        slotAnchors->resetTop();

        slotAnchors->setVerticalCenter(verticalCenter());
        //bottom and top offsets could have different values
        qreal offset = (padding.top() - padding.bottom()
                        + attachedProps->padding()->top()
                        - attachedProps->padding()->bottom()) / 2.0;
        slotAnchors->setVerticalCenterOffset(offset);
    }
}

void UCSlotsLayoutPrivate::layoutInRow(qreal siblingAnchorMargin, QQuickAnchorLine siblingAnchor, QList<QQuickItem *> &items)
{
    Q_Q(UCSlotsLayout);

    const int size = items.length();
    for (int i = 0; i < size; i++) {
        QQuickItem *item = items.at(i);
        QQuickAnchors *itemAnchors = QQuickItemPrivate::get(item)->anchors();

        UCSlotsAttached *attached =
                qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(item));

        if (!attached) {
            qmlInfo(q) << "Invalid attached property!";
            continue;
        }

        if (!attached->overrideVerticalPositioning()) {
            setupSlotsVerticalPositioning(item, attached);
        }

        if (i == 0) {
            //skip anchoring if the anchor given as input is invalid
            if (siblingAnchor.item == Q_NULLPTR)
                continue;

            itemAnchors->setLeft(siblingAnchor);
            itemAnchors->setLeftMargin(attached->padding()->leading() + siblingAnchorMargin);
        } else {
            UCSlotsAttached *attachedPreviousItem =
                    qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(items.at(i - 1)));

            if (!attachedPreviousItem) {
                qmlInfo(q) << "Invalid attached property!";
            } else {
                itemAnchors->setLeft(QQuickItemPrivate::get(items.at(i - 1))->right());
                itemAnchors->setLeftMargin(attachedPreviousItem->padding()->trailing() + attached->padding()->leading());
            }
        }
    }
}

void UCSlotsLayoutPrivate::_q_relayout()
{
    Q_Q(UCSlotsLayout);

    //only relayout after the component has been initialized
    if (!componentComplete)
        return;

    if (q->width() <= 0 || q->height() <= 0
            || !q->isVisible() || !q->opacity()) {
        return;
    }

    //total width of the slots that we're going to do the layout of.
    //this excludes mainSlot and the slots which have been skipped because
    //they're !visible or similar conditions
    qreal totalSlotsWidth = 0;

    //let's check the current visibility of our children and skip the
    //invisible slots
    QList<QQuickItem *> itemsToLayout;
    const int numOfLeading = leadingSlots.count();
    const int numOfTrailing = trailingSlots.count();
    int numOfLeadingToLayout = 0;
    int numOfTrailingToLayout = 0;
    //instead of having 2 cycles we only make one which iterates over both lists
    for (int i = 0; i < numOfLeading + numOfTrailing; i++) {
        QQuickItem *child = i < numOfLeading
                ? leadingSlots.at(i)
                : trailingSlots.at(i - numOfLeading);

        bool skipSlotFlag = skipSlot(child);
        if (i < numOfLeading) {
            if (numOfLeadingToLayout >= maxNumberOfLeadingSlots) {
                skipSlotFlag = true;
                qmlInfo(q) << "This layout only allows up to " << maxNumberOfLeadingSlots
                           << " leading slots. Please remove any additional leading slot.";
            } else {
                if (!skipSlotFlag)
                    numOfLeadingToLayout++;
            }
        } else {
            if (numOfTrailingToLayout >= maxNumberOfTrailingSlots) {
                skipSlotFlag = true;
                qmlInfo(q) << "This layout only allows up to " << maxNumberOfTrailingSlots
                           << " trailing slots. Please remove any additional trailing slot.";

            } else {
                if (!skipSlotFlag)
                    numOfTrailingToLayout++;
            }
        }
        if (!skipSlotFlag) {
            itemsToLayout.append(child);
            UCSlotsAttached *attached =
                    qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(child));

            if (!attached) {
                qmlInfo(q) << "Invalid attached property!";
                continue;
            }
            totalSlotsWidth += child->width() + attached->padding()->leading()
                    + attached->padding()->trailing();
        }
    }

    if (mainSlot) {
        //insert between leading and trailing
        itemsToLayout.insert(numOfLeadingToLayout, mainSlot);

        UCSlotsAttached *attachedProps =
                qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(mainSlot));

        if (!attachedProps) {
            qmlInfo(q) << "Invalid attached property!";
            return;
        }
        mainSlot->setImplicitWidth(q->width() - totalSlotsWidth
                                   - attachedProps->padding()->leading()
                                   - attachedProps->padding()->trailing()
                                   - padding.leading() - padding.trailing());
    } else {
        Q_UNUSED(totalSlotsWidth);
    }

    layoutInRow(padding.leading(), left(), itemsToLayout);
}

void UCSlotsLayoutPrivate::handleAttachedPropertySignals(QQuickItem *item, bool connect)
{
    if (item == Q_NULLPTR) {
        qDebug() << "handleAttachedPropertySignals: INVALID POINTER!";
        return;
    }

    Q_Q(UCSlotsLayout);
    UCSlotsAttached *attachedSlot =
            qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(item));
    if (!attachedSlot) {
        qmlInfo(q) << "Invalid attached property!";
        return;
    }

    if (connect) {
        QObject::connect(attachedSlot->padding(), SIGNAL(leadingChanged()), q, SLOT(_q_relayout()));
        QObject::connect(attachedSlot->padding(), SIGNAL(trailingChanged()), q, SLOT(_q_relayout()));
        if (item != mainSlot) {
            QObject::connect(attachedSlot, SIGNAL(positionChanged()), q, SLOT(_q_onSlotPositionChanged()));
            QObject::connect(attachedSlot->padding(), SIGNAL(topChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
            QObject::connect(attachedSlot->padding(), SIGNAL(bottomChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
            QObject::connect(attachedSlot, SIGNAL(overrideVerticalPositioningChanged()), q, SLOT(_q_onSlotOverrideVerticalPositioningChanged()));
        } else {
            QObject::connect(attachedSlot->padding(), SIGNAL(topChanged()), q, SLOT(_q_updateCachedMainSlotHeight()));
            QObject::connect(attachedSlot->padding(), SIGNAL(bottomChanged()), q, SLOT(_q_updateCachedMainSlotHeight()));
            //we ignore changes in overrideVerticalPositioning and position for the main slot
            //QObject::connect(attachedSlot, SIGNAL(overrideVerticalPositioningChanged()), q, SLOT(_q_updateCachedMainSlotHeight()));
            //QObject::disconnect(attachedSlot, SIGNAL(positionChanged()), q, SLOT(_q_onSlotPositionChanged()));
        }
    } else {
        QObject::disconnect(attachedSlot->padding(), SIGNAL(leadingChanged()), q, SLOT(_q_relayout()));
        QObject::disconnect(attachedSlot->padding(), SIGNAL(trailingChanged()), q, SLOT(_q_relayout()));
        if (item != mainSlot) {
            QObject::disconnect(attachedSlot, SIGNAL(positionChanged()), q, SLOT(_q_onSlotPositionChanged()));
            QObject::disconnect(attachedSlot->padding(), SIGNAL(topChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
            QObject::disconnect(attachedSlot->padding(), SIGNAL(bottomChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
            QObject::disconnect(attachedSlot, SIGNAL(overrideVerticalPositioningChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
        } else {
            QObject::disconnect(attachedSlot->padding(), SIGNAL(topChanged()), q, SLOT(_q_updateCachedMainSlotHeight()));
            QObject::disconnect(attachedSlot->padding(), SIGNAL(bottomChanged()), q, SLOT(_q_updateCachedMainSlotHeight()));
            //we ignore changes in overrideVerticalPositioning and positiong for the main slot
            //QObject::disconnect(attachedSlot, SIGNAL(overrideVerticalPositioningChanged()), q, SLOT(_q_updateCachedMainSlotHeight()));
            //QObject::disconnect(attachedSlot, SIGNAL(positionChanged()), q, SLOT(_q_onSlotPositionChanged()));
        }
    }
}


/*!
   \qmltype SlotsLayout
   \instantiates UCSlotsLayout
   \inqmlmodule Ubuntu.Components 1.3
   \ingroup ubuntu-slotslayout
   \inherits Item
   \since Ubuntu.Components 1.3
   \brief The SlotsLayout component provides an easy way to layout a list
   of user-interface elements horizontally following Ubuntu design standards.
   We call the elements which we want to layout "slots". Slot is just another
   name for SlotsLayout's visual children.

   SlotsLayout will layout its visual children according to Ubuntu's visual design
   rules, providing automatic spacing and positioning (both horizontal and
   vertical, unless \l overrideVerticalPositioning is set) for each of them.

   There are three conceptual types of slots:
   \list
   \li * The \b {leading slots}, which are the ones positioned at the
            beginning of the layout (i.e. they are the leftmost elements
            in left-to-right locales).
   \li * The \b {trailing slots}, which are positioned after \l mainSlot,
            if any, or after the leading slots otherwise.
   \li * The \b {main slot}, which drives the positioning of
            the vertical dimension of each slot as described in section
            \l {Automatic vertical positioning of slots}. This slot sits
            between leading and trailing slots, and can be set using the
            property \l {mainSlot}.
   \endlist

   In order to comply with Ubuntu design standards, this component hosts
   a maximum of 1 leading and 2 trailing slots. The layout algorithm will
   ignore any additional item.

   SlotsLayout also provides a series of properties which are attached to
   each slot. However not all properties are valid in all the circumstances.
   These properties allow an easy tweaking of slot's positioning within
   the layout.
   \l {SlotsLayout::position} can be used to set whether a slot should a
   leading or trailing one.
   \l {SlotsLayout::padding} can be used to tweak the padding around a
   slot.
   \l {SlotsLayout::overrideVerticalPositioning} allows to disable the
   automatic positioning of the vertical coordinate of a slot as described
   in \l {Automatic vertical positioning of slots}, in case
   a custom behaviour is needed.

   The following example shows a very simple SlotsLayout with an \l Icon as
   leading slot, and a \l CheckBox as trailing slot. There is no need to specify
   any size or margin, everything is handled automatically by the layout
   component.

   \qml
        SlotsLayout {
            mainSlot: Label {
                text: "Hello developers!"
            }

            CheckBox { SlotsLayout.position: SlotsLayout.Trailing }

            Icon {
                name: "message"
                SlotsLayout.position: SlotsLayout.Leading;
                width: units.gu(2)
            }
        }
   \endqml

   \b {For list items, the recommendation is to use} \l {ListItemLayout},
   which is a SlotsLayout featuring a predefined \l mainSlot which
   has been optimized to provide maximum performance while still covering
   most of the usecases.
   \sa ListItemLayout

   The positioning of each
   slot should only be tweaked using its attached properties. Just like
   when using QtQuick's Row, a child item within the layout (i.e. a slot) should
   not set its \b x or \b anchors affecting the horizontal positioning
   (left, right, horizontalCenter, centerIn, fill). If you need to perform
   these actions, consider positioning the items without the use of a
   SlotsLayout.

   The vertical \b anchors of a slot can be modified, \b {provided
   that} \l {SlotsLayout::overrideVerticalPositioning} for that slot is set.
   More about this in the \l {Advanced layout tweaks} section.


   \section1 Resizing the layout

   SlotsLayout's implicit width is by default set to the width of the parent
   (usually a \l {ListItem}).
   Changing the width of the layout is possible but should not be needed in
   most of the usecases: keeping the width in sync with the width of its parent
   allows the user interface to be scalable across devices with varying resolution
   and form factors.

   The implicit height is not fixed either. In order not to clip any of the slots,
   SlotsLayout watches its children for height and padding changes  and makes sure to
   grow its implicit height to accomodate the tallest slot plus the padding around
   the slot and the one around the layout.

   Manually resizing SlotsLayout (or \l ListItemLayout) is usually not needed and
   we recommend to avoid it, as the component will already take care of doing the
   job for you. The following example shows what a standard SlotsLayout should
   look like:

    \qml
        SlotsLayout {
            mainSlot: Label {
                text: "Hello developers!"
            }
            //just an example of something inside SlotsLayout
            CheckBox { SlotsLayout.position: SlotsLayout.Trailing }
        }
   \endqml

   \section1 Automatic vertical positioning of slots
   In order to provide a nice looking and consistent layout across the whole
   platform, SlotsLayout automatically handles the vertical positioning of its
   slots so that they comply with the following rules:
   \list
    \li * if there's any slot which is taller or as tall as \l {mainSlot} or if
        no \l {mainSlot} is defined, all slots will be vertically centered within the
        layout (still taking \l {SlotsLayout::padding.top} and \l {SlotsLayout::padding.bottom}
        into account).
    \li * Otherwise, all the slots (including \l {mainSlot}) will be aligned to
        the top of the layout with a padding of \l {SlotsLayout::padding.top} plus
        the top padding of the slot, defined in its attached properties.
   \endlist

   Even though it is \b {not recommended}, it is still possible to override
   this behaviour on a slot-by-slot basis by setting
   \l {SlotsLayout::overrideVerticalPositioning} to true. This is described in
   \l {Advanced layout tweaks}.

   \section1 Advanced layout tweaks
   The automatic layout provided by SlotsLayout is designed to cover most
   of the usecases. There could be times, however, where we might want to
   tweak the positioning of one particular slot.

   A slot can set its attached properties \l SlotsLayout::padding and
   \l {SlotsLayout::overrideVerticalPositioning} to reach the desired
   position.

   When a slot enables \l {SlotsLayout::overrideVerticalPositioning}, it
   gains control over its vertical anchors (top, bottom, verticalCenter).

   Moreover, SlotsLayout will not consider the height of that slot when
   computing its implicitHeight anymore (see \l {Resizing the layout}).
   Care must be to avoid pushing the slot outside of the layout perimeter,
   to avoid getting it clipped.

   The following example shows a SlotsLayout which has both custom padding
   and a slot which uses custom vertical positioning:

   \qml
        SlotsLayout {
            id: layout
            mainSlot: Label { id: label; text: "Hello developers!" }
            padding {
                top: units.gu(3)
                bottom: units.gu(3)
            }
            CheckBox {
                anchors.top: label.top
                SlotsLayout.position: SlotsLayout.Trailing
                SlotsLayout.overrideVerticalPositioning: true
            }
        }
   \endqml
*/

UCSlotsLayout::UCSlotsLayout(QQuickItem *parent) :
    QQuickItem(*(new UCSlotsLayoutPrivate), parent)
{
    setFlag(ItemHasContents);
    setFlag(ItemIsFocusScope);
    Q_D(UCSlotsLayout);
    d->init();
}

void UCSlotsLayout::componentComplete()
{
    Q_D(UCSlotsLayout);
    QQuickItem::componentComplete();

    d->_q_updateCachedMainSlotHeight();
    d->_q_updateSlotsBBoxHeight();
}

void UCSlotsLayout::itemChange(ItemChange change, const ItemChangeData &data)
{
    Q_D(UCSlotsLayout);

    //declare vars outside switch to prevent "crosses initialization of" compile error
    QQuickItem *newParent = Q_NULLPTR;
    switch (change) {
    case ItemChildAddedChange:
        if (data.item) {
            d->handleAttachedPropertySignals(data.item, true);

            //An item disappearing/reappearing could change the maximum height of the slots
            //_q_updateSlotsBBoxHeight will trigger relayout
            QObject::connect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));

            //we relayout because we have to update the width of the main slot
            //TODO: do this in a separate function? do were really have to do the whole relayout?
            if (data.item != d->mainSlot) {
                d->addSlot(data.item);
                QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_onSlotWidthChanged()));
                QObject::connect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
                d->_q_updateSlotsBBoxHeight();
            } else {
                QObject::connect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateCachedMainSlotHeight()));
                d->_q_updateCachedMainSlotHeight();
            }
        }
        break;
    case ItemChildRemovedChange:
        if (data.item) {
            d->handleAttachedPropertySignals(data.item, false);

            //This wouldn't be needed if the child is destroyed, but we can't know what, we just know
            //that it's changing parent, so we still disconnect from all the signals manually
            QObject::disconnect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));

            if (data.item != d->mainSlot) {
                d->removeSlot(data.item);
                QObject::disconnect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_onSlotWidthChanged()));
                QObject::disconnect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
                d->_q_updateSlotsBBoxHeight();
            } else {
                QObject::disconnect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateCachedMainSlotHeight()));
                d->_q_updateCachedMainSlotHeight();
            }
        }

        break;
    case ItemParentHasChanged:
        newParent = data.item;
        if (newParent) {
            if (d->m_parentItem) {
                QObject::disconnect(d->m_parentItem, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
            }

            d->m_parentItem = newParent;
            QObject::connect(newParent, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()), Qt::DirectConnection);
            d->_q_updateSize();
        }
        break;
    default:
        break;
    }

    QQuickItem::itemChange(change, data);
}

/*!
 * \qmlproperty Item SlotsLayout::mainSlot
 * This property represents the main slot of the layout. The main slot is the
 * one that defines the vertical positioning of the other slots. More details
 * can be found in the section \l {Automatic vertical positioning of slots}.
 */
QQuickItem *UCSlotsLayout::mainSlot() const
{
    Q_D(const UCSlotsLayout);
    return d->mainSlot;
}

void UCSlotsLayout::setMainSlot(QQuickItem *item)
{
    Q_D(UCSlotsLayout);
    if (d->mainSlot != item) {
        d->mainSlot = item;
        d->mainSlot->setParent(this);
        d->mainSlot->setParentItem(this);
        Q_EMIT mainSlotChanged();
    }
}

/*!
    \qmlpropertygroup ::SlotsLayout::padding
    \qmlproperty real SlotsLayout::padding.top
    \qmlproperty real SlotsLayout::padding.bottom
    \qmlproperty real SlotsLayout::padding.leading
    \qmlproperty real SlotsLayout::padding.trailing

    This property defines the padding around the bounding box which
    holds all the slots.
*/
UCSlotsLayoutPadding* UCSlotsLayout::padding() {
    Q_D(UCSlotsLayout);
    return &(d->padding);
}

/******************************************************************************
 * UCSlotsAttachedPrivate
 */
UCSlotsAttachedPrivate::UCSlotsAttachedPrivate()
    : QObjectPrivate()
    , position(UCSlotsLayout::Trailing)
    , overrideVerticalPositioning(false)
{
}

void UCSlotsAttachedPrivate::_q_onGuValueChanged()
{
    Q_Q(UCSlotsAttached);
    if (!padding.leadingWasSetFromQml)
        padding.setLeading(UCUnits::instance().gu(SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU));

    if (!padding.trailingWasSetFromQml)
        padding.setTrailing(UCUnits::instance().gu(SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU));

    if (!padding.topWasSetFromQml)
        padding.setTop(UCUnits::instance().gu(SLOTSLAYOUT_SLOTS_TOPBOTTOMMARGINS_GU));

    if (!padding.bottomWasSetFromQml)
        padding.setBottom(UCUnits::instance().gu(SLOTSLAYOUT_SLOTS_TOPBOTTOMMARGINS_GU));
}

/******************************************************************************
 * UCSlotsAttached
 */
UCSlotsAttached::UCSlotsAttached(QObject *object)
    : QObject(*(new UCSlotsAttachedPrivate), object)
{
    Q_D(UCSlotsAttached);
    d->_q_onGuValueChanged();
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(_q_onGuValueChanged()));
}

/*!
    \qmlattachedproperty enumeration SlotsLayout::position
    This attached property defines the relative position of
    the slot inside the layout. It is attached to each slot.
    The default value is SlotsLayout.Trailing.

    Valid values for \l position are:
    \list
    \li * SlotsLayout.First: the slot will be positioned at the
        beginning of the layout
    \li * SlotsLayout.Leading: the slot will be positioned in
        the leading slots
    \li * SlotsLayout.Trailing: the slot will be positioned in
        the trailing slots, i.e. the one towards the end of the
        layout.
    \li * SlotsLayout.End: the slot will be positioned at
        the end of the layout.
    \endlist

    Whenever there are more slots with the same \l {SlotsLayout::position},
    they will be positioned following their stacking order.

    TODO: INSERT CODE EXAMPLE

    TODO: INSERT RELATIVE POSITIONING EXPLANATION
    (SlotsLayout.Leading+1, etc)
*/
UCSlotsLayout::UCSlotPosition UCSlotsAttached::position() const
{
    Q_D(const UCSlotsAttached);
    return d->position;
}

void UCSlotsAttached::setPosition(UCSlotsLayout::UCSlotPosition pos)
{
    Q_D(UCSlotsAttached);
    if (d->position != pos) {
        d->position = pos;
        Q_EMIT positionChanged();
    }
}

/*!
    \qmlattachedproperty QtObject ::SlotsLayout::padding

    While SlotsLayout's padding property defines the padding around the whole
    layout, this attached property defines the padding around the slot it is
    attached to.

    It is a grouped property that exposes the properties padding.top,
    padding.bottom, padding.leading, padding.trailing.

    Please note that \b top and \b bottom paddings are only used when
    \l {SlotsLayout::overrideVerticalPositioning} is set to false. More about
    this in \l {Automatic vertical positioning of slots}.

*/
UCSlotsLayoutPadding *UCSlotsAttached::padding()
{
    Q_D(UCSlotsAttached);
    return &d->padding;
}

/*!
 * \qmlattachedproperty bool SlotsLayout::overrideVerticalPositioning
 *
 * This attached property holds whether the layout should take care of
 * the vertical positioning of a slot. It is attached to every slot.
 * The default value is false.
 *
 * If this property is set to true, the layout will ignore the
 * corresponding slot during the computation of the \b implicitHeight of
 * the whole layout (see \l {Resizing the layout}) and it will not touch
 * its vertical position during the layout process. As a consequence, it
 * is possible, in that case, to set the vertical anchors of that slot
 * (verticalCenter, top, bottom) or even its \b y property.
 *
 * Care must be taken to avoid getting the slot clipped by positioning it
 * partly or completely outside of the SlotsLayout that holds it.
 *
 */

bool UCSlotsAttached::overrideVerticalPositioning() const
{
    Q_D(const UCSlotsAttached);
    return d->overrideVerticalPositioning;
}

void UCSlotsAttached::setOverrideVerticalPositioning(bool val)
{
    Q_D(UCSlotsAttached);
    if (d->overrideVerticalPositioning != val) {
        d->overrideVerticalPositioning = val;
        Q_EMIT overrideVerticalPositioningChanged();
    }
}

UCSlotsAttached *UCSlotsLayout::qmlAttachedProperties(QObject *object)
{
    return new UCSlotsAttached(object);
}

UCSlotsLayoutPadding::UCSlotsLayoutPadding(QObject *parent)
    : QObject(parent)
    , leadingWasSetFromQml(false)
    , trailingWasSetFromQml(false)
    , topWasSetFromQml(false)
    , bottomWasSetFromQml(false)
    , m_leading(0)
    , m_trailing(0)
    , m_top(0)
    , m_bottom(0)
{
}

qreal UCSlotsLayoutPadding::leading() const
{
    return m_leading;
}

void UCSlotsLayoutPadding::setLeading(qreal val)
{
    if (m_leading != val) {
        m_leading = val;
        Q_EMIT leadingChanged();
    }
}

void UCSlotsLayoutPadding::setLeadingQml(qreal val)
{
    leadingWasSetFromQml = true;
    setLeading(val);
}

qreal UCSlotsLayoutPadding::trailing() const
{
    return m_trailing;
}

void UCSlotsLayoutPadding::setTrailing(qreal val)
{
    if (m_trailing != val) {
        m_trailing = val;
        Q_EMIT trailingChanged();
    }
}

void UCSlotsLayoutPadding::setTrailingQml(qreal val)
{
    trailingWasSetFromQml = true;
    setTrailing(val);
}

qreal UCSlotsLayoutPadding::top() const
{
    return m_top;
}

void UCSlotsLayoutPadding::setTop(qreal val)
{
    if (m_top != val) {
        m_top = val;
        Q_EMIT topChanged();
    }
}

void UCSlotsLayoutPadding::setTopQml(qreal val)
{
    topWasSetFromQml = true;
    setTop(val);
}

qreal UCSlotsLayoutPadding::bottom() const
{
    return m_bottom;
}

void UCSlotsLayoutPadding::setBottom(qreal val)
{
    if (m_bottom != val) {
        m_bottom = val;
        Q_EMIT bottomChanged();
    }
}

void UCSlotsLayoutPadding::setBottomQml(qreal val)
{
    bottomWasSetFromQml = true;
    setBottom(val);
}


UCThreeLabelsSlotPrivate::UCThreeLabelsSlotPrivate()
    : QQuickItemPrivate()
    , cachedTheme(Q_NULLPTR)
    , m_title(Q_NULLPTR)
    , m_subtitle(Q_NULLPTR)
    , m_summary(Q_NULLPTR)
{
}

void UCThreeLabelsSlotPrivate::init()
{
    Q_Q(UCThreeLabelsSlot);

    //TODO FIXME: Add connect to themeChanged() when that signal will be available

    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_onGuValueChanged()));
}

void UCThreeLabelsSlotPrivate::setTitleProperties()
{
    Q_Q(UCThreeLabelsSlot);
    if (m_title != Q_NULLPTR) {
        m_title->setWrapMode(QQuickText::WordWrap);
        m_title->setElideMode(QQuickText::ElideRight);
        m_title->setMaximumLineCount(1);
        QFont titleFont = m_title->font();
        titleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("medium"));
        titleFont.setWeight(QFont::Light);
        m_title->setFont(titleFont);
    }

    //We set the theme-dependent properties (such as the colour) later
    //as it requires qmlContext(q) to be initialized
}

void UCThreeLabelsSlotPrivate::setSubtitleProperties()
{
    Q_Q(UCThreeLabelsSlot);
    if (m_subtitle != Q_NULLPTR) {
        m_subtitle->setWrapMode(QQuickText::WordWrap);
        m_subtitle->setElideMode(QQuickText::ElideRight);
        m_subtitle->setMaximumLineCount(1);
        QFont subtitleFont = m_subtitle->font();
        subtitleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));
        subtitleFont.setWeight(QFont::Light);
        m_subtitle->setFont(subtitleFont);
    }

    //We set the theme-dependent properties (such as the colour) later
    //as it requires qmlContext(q) to be initialized
}

void UCThreeLabelsSlotPrivate::updateLabelsColors()
{
    if (!componentComplete)
        return;

    if (cachedTheme) {
        if (m_title != Q_NULLPTR) {
            m_title->setColor(labelsColor);
        }
        if (m_subtitle != Q_NULLPTR) {
            m_subtitle->setColor(labelsColor);
        }
        if (m_summary != Q_NULLPTR) {
            m_summary->setColor(labelsColor);
        }
    }
}

void UCThreeLabelsSlotPrivate::setSummaryProperties()
{
    Q_Q(UCThreeLabelsSlot);
    if (m_summary != Q_NULLPTR) {
        m_summary->setWrapMode(QQuickText::WordWrap);
        m_summary->setElideMode(QQuickText::ElideRight);
        m_summary->setMaximumLineCount(2);
        QFont summaryFont = m_summary->font();
        summaryFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));
        summaryFont.setWeight(QFont::Light);
        m_summary->setFont(summaryFont);
    }

    //We set the theme-dependent properties (such as the colour) later
    //as it requires qmlContext(q) to be initialized
}

void UCThreeLabelsSlotPrivate::_q_onThemeChanged()
{
    if (!componentComplete)
        return;

    Q_Q(UCThreeLabelsSlot);
    cachedTheme = qmlContext(q)->contextProperty("theme").value<UCTheme*>();
    labelsColor = cachedTheme->getPaletteColor("selected", "backgroundText");

    updateLabelsColors();
}

void UCThreeLabelsSlotPrivate::_q_onGuValueChanged()
{
    if (m_title != Q_NULLPTR) {
        QFont titleFont = m_title->font();
        titleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("medium"));
        m_title->setFont(titleFont);
    }

    if (m_subtitle != Q_NULLPTR) {
        QFont subtitleFont = m_subtitle->font();
        subtitleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));
        m_subtitle->setFont(subtitleFont);
    }

    if (m_summary != Q_NULLPTR) {
        QFont summaryFont = m_summary->font();
        summaryFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));
        m_summary->setFont(summaryFont);
    }

    if (m_title != Q_NULLPTR
            || m_subtitle != Q_NULLPTR
            || m_summary != Q_NULLPTR) {
        _q_updateLabelsAnchorsAndBBoxHeight();
    }
}

void UCThreeLabelsSlotPrivate::_q_updateLabelsAnchorsAndBBoxHeight()
{
    //if the component is not ready the QML properties may not have been evaluated yet,
    //it's not worth doing anything if that's the case
    if (!componentComplete) {
        return;
    }

    Q_Q(UCThreeLabelsSlot);
    bool skipTitle = m_title == Q_NULLPTR || m_title->text().isEmpty() || !m_title->isVisible();
    bool skipSubtitle = m_subtitle == Q_NULLPTR || m_subtitle->text().isEmpty() || !m_subtitle->isVisible();
    bool skipSummary = m_summary == Q_NULLPTR || m_summary->text().isEmpty() || !m_summary->isVisible();

    if (!skipTitle) {
        QQuickAnchors *titleAnchors = QQuickItemPrivate::get(m_title)->anchors();
        titleAnchors->setTop(top());
    }

    //even if a QQuickText is empty it will have height as if it had one character, so we can't rely
    //on just anchoring to bottom of the text above us (title in this case) because that will lead
    //to wrong positioning when title is empty
    if (!skipSubtitle) {
        QQuickAnchors *subtitleAnchors = QQuickItemPrivate::get(m_subtitle)->anchors();
        subtitleAnchors->setTop(skipTitle
                                ? top()
                                : QQuickItemPrivate::get(m_title)->baseline());
        subtitleAnchors->setTopMargin(skipTitle
                                      ? 0
                                      : UCUnits::instance().gu(LABELSBLOCK_SPACING_GU));
    }

    if (!skipSummary) {
        QQuickAnchors *summaryAnchors = QQuickItemPrivate::get(m_summary)->anchors();
        summaryAnchors->setTop(skipSubtitle
                               ? (skipTitle ? top() : QQuickItemPrivate::get(m_title)->baseline())
                               : QQuickItemPrivate::get(m_subtitle)->baseline());
        summaryAnchors->setTopMargin(skipSubtitle && skipTitle
                                     ? 0
                                     : UCUnits::instance().gu(LABELSBLOCK_SPACING_GU));

    }
    //Update height of the labels box
    //NOTE (FIXME? it's stuff in Qt): height is not 0 when the string is empty, its default value is "fontHeight"!
    qreal labelsBoundingBoxHeight = 0;

    if (!skipTitle) {
        if (skipSubtitle && skipSummary) {
            labelsBoundingBoxHeight += m_title->height();
        } else {
            labelsBoundingBoxHeight += m_title->baselineOffset() + UCUnits::instance().gu(LABELSBLOCK_SPACING_GU);
        }
    }

    if (skipSubtitle) {
        if (!skipSummary) {
            labelsBoundingBoxHeight += m_summary->height();
        }
    } else {
        if (skipSummary) {
            labelsBoundingBoxHeight += m_subtitle->height();
        } else {
            labelsBoundingBoxHeight += m_subtitle->baselineOffset()
                    + UCUnits::instance().gu(LABELSBLOCK_SPACING_GU)
                    + m_summary->height();
        }
    }

    q->setImplicitHeight(labelsBoundingBoxHeight);
}

UCThreeLabelsSlot::UCThreeLabelsSlot(QQuickItem *parent)
    : QQuickItem(*(new UCThreeLabelsSlotPrivate), parent)
{
    setFlag(ItemHasContents);
    Q_D(UCThreeLabelsSlot);
    d->init();
}

void UCThreeLabelsSlot::componentComplete()
{
    Q_D(UCThreeLabelsSlot);
    QQuickItem::componentComplete();

    d->_q_onGuValueChanged();

    //only at this point the context property is available
    d->_q_onThemeChanged();
}

QQuickText *UCThreeLabelsSlot::title()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_title == Q_NULLPTR) {
        d->m_title = new QQuickText(this);

        QQuickAnchors *titleAnchors = QQuickItemPrivate::get(d->m_title)->anchors();
        titleAnchors->setLeft(d->left());
        titleAnchors->setRight(d->right());

        //we need this to know when any of the labels is empty. In that case, we'll have to change the
        //anchors because even if a QQuickText has empty text, its height will not be 0 but "fontHeight",
        //so anchoring to text's bottom will result in the wrong outcome as a consequence.
        //TODO: updating anchors just because text changes is too much, we should probably just
        //have variables signal when a label becomes empty
        QObject::connect(d->m_title, SIGNAL(textChanged(QString)), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        //the height may change for many reasons, for instance:
        //- change of fontsize
        //- or resizing the layout until text wrapping is triggered
        //so we have to monitor height change as well
        QObject::connect(d->m_title, SIGNAL(heightChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_title, SIGNAL(visibleChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        d->setTitleProperties();
        d->updateLabelsColors();
        d->_q_updateLabelsAnchorsAndBBoxHeight();
    }
    return d->m_title;
}

QQuickText *UCThreeLabelsSlot::subtitle()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_subtitle == Q_NULLPTR) {
        d->m_subtitle = new QQuickText(this);

        QQuickAnchors *subtitleAnchors = QQuickItemPrivate::get(d->m_subtitle)->anchors();
        subtitleAnchors->setLeft(d->left());
        subtitleAnchors->setRight(d->right());

        QObject::connect(d->m_subtitle, SIGNAL(textChanged(QString)), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_subtitle, SIGNAL(heightChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_subtitle, SIGNAL(visibleChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        d->setSubtitleProperties();
        d->updateLabelsColors();
        d->_q_updateLabelsAnchorsAndBBoxHeight();
    }
    return d->m_subtitle;
}

QQuickText *UCThreeLabelsSlot::summary()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_summary == Q_NULLPTR) {
        d->m_summary = new QQuickText(this);

        QQuickAnchors *summaryAnchors = QQuickItemPrivate::get(d->m_summary)->anchors();
        summaryAnchors->setLeft(d->left());
        summaryAnchors->setRight(d->right());

        QObject::connect(d->m_summary, SIGNAL(textChanged(QString)), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_summary, SIGNAL(heightChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_summary, SIGNAL(visibleChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        d->setSummaryProperties();
        d->updateLabelsColors();
        d->_q_updateLabelsAnchorsAndBBoxHeight();
    }
    return d->m_summary;
}

#include "moc_ucslotslayout.cpp"
