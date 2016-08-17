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

#include "ucunits_p.h"
#include "ucslotslayout_p_p.h"

UT_NAMESPACE_BEGIN

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

    QObject::connect(&padding, SIGNAL(leadingChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&padding, SIGNAL(trailingChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&padding, SIGNAL(topChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&padding, SIGNAL(bottomChanged()), q, SLOT(_q_relayout()));

    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_onGuValueChanged()));

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
                        && maxSlotsHeight > UCUnits::instance()->gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU))
                       ? UCUnits::instance()->gu(SLOTSLAYOUT_TOPMARGIN1_GU)
                       : UCUnits::instance()->gu(SLOTSLAYOUT_TOPMARGIN2_GU));
    }

    if (!padding.bottomWasSetFromQml) {
        padding.setBottom((getVerticalPositioningMode() == UCSlotPositioningMode::CenterVertically
                           && maxSlotsHeight > UCUnits::instance()->gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU))
                          ? UCUnits::instance()->gu(SLOTSLAYOUT_BOTTOMMARGIN1_GU)
                          : UCUnits::instance()->gu(SLOTSLAYOUT_BOTTOMMARGIN2_GU));
    }

    return;
}

bool UCSlotsLayoutPrivate::skipSlot(QQuickItem *slot)
{
    if (slot == Q_NULLPTR) {
        qFatal("skipSlot: NULL POINTER!");
    }

    //these are the same conditions Row uses, we don't want
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
        qFatal("insertSlotIntoSortedList: INVALID POINTER!");
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
        qFatal("addSlot: INVALID POINTER!");
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
        qFatal("addSlot: INVALID POINTER!");
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
    if (!padding.leadingWasSetFromQml) {
        padding.setLeading(UCUnits::instance()->gu(SLOTSLAYOUT_LEFTMARGIN_GU));
    }

    if (!padding.trailingWasSetFromQml) {
        padding.setTrailing(UCUnits::instance()->gu(SLOTSLAYOUT_RIGHTMARGIN_GU));
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
    q->setImplicitWidth(parentItem ? parentItem->width() : UCUnits::instance()->gu(IMPLICIT_SLOTSLAYOUT_WIDTH_GU));
    q->setImplicitHeight(qMax<qreal>(maxSlotsHeight, mainSlotHeight)
                         + padding.top() + padding.bottom());

    _q_relayout();
}

void UCSlotsLayoutPrivate::_q_onSlotWidthChanged()
{
    Q_Q(UCSlotsLayout);
    QQuickItem *slot = qobject_cast<QQuickItem*>(q->sender());
    if (slot == Q_NULLPTR) {
        qFatal("onSlotWidthChanged: NULL SLOT");
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
        qFatal("onSlotOverrideVerticalPositioningChanged: NULL SENDER");
    }

    QQuickItem *slot = qobject_cast<QQuickItem*>(sender->parent());
    if (slot == Q_NULLPTR) {
        qFatal("onSlotOverrideVerticalPositioningChanged: NULL SLOT");
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

    //resetting anchors doesn't also reset the position
    slot->setY(0);

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
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu-slotslayout
    \inherits Item
    \since Ubuntu.Components 1.3
    \brief The SlotsLayout component provides an easy way to layout a list
    of user-interface elements horizontally following Ubuntu design standards.
    We call the elements which we want to layout "slots". \b {Slots} is just another
    name for SlotsLayout's \b {visual children}.

    SlotsLayout will layout its children according to Ubuntu's visual design
    rules, providing automatic spacing and positioning (both horizontal and
    vertical, unless \l overrideVerticalPositioning is set) for each of them.

    \b {If you're building list items,} we recommend to use \l {ListItemLayout}, as
    it is designed to accomodate up to 3 labels that follow our UI standards.

    \sa ListItemLayout

    There are three conceptual types of slots:
    \list
    \li The \b {leading slots}, which are the ones positioned at the
            beginning of the layout (i.e. they are the leftmost elements
            in left-to-right locales).
    \li The \b {trailing slots}, which are positioned after \l mainSlot,
            if any, or after the leading slots otherwise.
    \li The \b {main slot}, which drives the positioning of
            the vertical dimension of each slot as described in section
            \l {Automatic vertical positioning of slots}. This slot sits
            between leading and trailing slots, and can be set using the
            property \l mainSlot.
    \endlist

    In order to comply with Ubuntu design standards, this component hosts
    a maximum of 1 leading and 2 trailing slots. The layout algorithm will
    ignore any additional item.

    SlotsLayout also provides a series of properties which are attached to
    each slot. However not all properties are valid in all the circumstances.
    These properties allow an easy tweaking of slot's positioning within
    the layout.
    \l {SlotsLayout::position} can be used to set whether a slot should be a
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

    The positioning of each
    slot should only be tweaked using its attached properties. Just like
    when using QtQuick's Row, a child item within the layout should
    not set its \b x or \b anchors affecting the horizontal positioning
    (left, right, horizontalCenter, centerIn, fill). If you need to perform
    these actions, consider positioning the items without the use of a
    SlotsLayout.

    The vertical \b anchors of a slot can be modified, \b {provided
    that} \l {SlotsLayout::overrideVerticalPositioning} for that slot is set.
    More about this in the \l {Advanced layout tweaks} section.


    \section1 Resizing the layout

    SlotsLayout's \b {implicit width} is by default set to the width of the parent
    (usually a \l ListItem).
    Changing the width of the layout is possible but should not be needed in
    most of the usecases: keeping the width in sync with the width of its parent
    allows the user interface to be scalable across devices with varying resolution
    and form factors.

    The \b {implicit height} is not fixed either. In order not to clip any of the slots, the
    SlotsLayout adapts to accomodate its highest slot with padding and the padding around the layout.

    Because of the above, it is recommended to have items wrapping the layout
    bind to SlotsLayout's \b {height} (adding ListItem \b divider's height if needed),
    not the opposite. It is not recommended,
    for instance, to use anchors.fill to force SlotsLayout to fill another Item,
    because that item might not have enough space to accomdate all the slots,
    and therefore the slots will be clipped.

    \qml
    ListItem {
        //CORRECT
        height: layout.height + (divider.visible ? divider.height : 0)

        SlotsLayout {
            id: layout

            //WRONG
            //anchors.fill: parent

            mainSlot: Label {
                text: "Hello developers!"
            }
            CheckBox { SlotsLayout.position: SlotsLayout.Trailing }
        }
    }
    \endqml

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
    In order to provide a visually pleasing and consistent layout across the whole
    platform, SlotsLayout automatically handles the vertical positioning of its
    slots so that they comply with the following rules:
    \list
        \li if there's any slot which is taller or as tall as \l {mainSlot} or if
            no \l {mainSlot} is defined, all slots will be \b {vertically centered} within the
            layout (still taking \l {SlotsLayout::padding.top} and \l {SlotsLayout::padding.bottom}
            into account).
            \image SlotsLayout_centerVertically.png
        \li \b{Otherwise}, all the slots (including \l {mainSlot}) will be \b {aligned to
            the top} of the layout with a padding of \l {SlotsLayout::padding.top} plus
            the top padding of the slot, defined in its attached properties.
            \image SlotsLayout_alignToTop.png
    \endlist

    Even though it is \b {not recommended}, it is still possible to override
    this behaviour on a slot-by-slot basis by setting
    \l {SlotsLayout::overrideVerticalPositioning} to true. This is described in
    \l {Advanced layout tweaks}.

    \section1 Input handling
    The purpose of SlotsLayout is to position its slots following
    the rules defined in \l {Automatic vertical positioning of slots}.
    There are two recommended ways to implement input handling: the first one,
    in case the target is to create a list item, is to put the \l SlotsLayout
    (or any derived component, like \l {ListItemLayout}) inside a \l {ListItem},
    and use ListItem's onClicked handler to handle clicks and taps.

    \qml
    ListItem {
        height: layout.height + (divider.visible ? divider.height : 0)
        onClicked: console.log("clicked!")

        SlotsLayout {
            id: layout
            mainSlot: Label {
                text: "Hello developers!"
            }
            //just an example of something inside SlotsLayout
            CheckBox { SlotsLayout.position: SlotsLayout.Trailing }
        }
    }
    \endqml

    The second option is to use a MouseArea as a wrapper around
    SlotsLayout, as the following code demonstrates:
    \qml
    MouseArea {
        anchors.left: parent.left
        anchors.right: parent.right
        height: layout.height
        onClicked: console.log("clicked!")

        SlotsLayout {
            id: layout
            mainSlot: Label {
                text: "Hello developers!"
            }
            //just an example of something inside SlotsLayout
            CheckBox { SlotsLayout.position: SlotsLayout.Trailing }
        }
    }
    \endqml

    \b {Note}: as recommended in \l {Resizing the layout}, since SlotsLayout
    has a dynamic height, it is important to have the parent item's height (in this case
    MouseArea) follow SlotsLayout's \b {height}, and not the opposite, to
    avoid getting the slots clipped out of the layout.

    \section1 Advanced layout tweaks
    The automatic layout provided by SlotsLayout is designed to cover most
    of the usecases. There could be times, however, where you might want to
    tweak the positioning of one particular slot.

    A slot can set its attached properties \l SlotsLayout::padding and
    \l {SlotsLayout::overrideVerticalPositioning} to reach the desired
    position.

    When a slot enables \l {SlotsLayout::overrideVerticalPositioning}, it
    gains control over its vertical anchors (top, bottom, verticalCenter).
    Moreover, SlotsLayout will ignore the height of that slot in the
    following computations of its implicitHeight (see \l {Resizing the layout}).

    When manually positioning a slot, care must be taken to avoid
    pushing the slot outside of the layout perimeter, to avoid getting
    it clipped.

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

    It is also possible to sort the slots in a relative fashion, by
    using positions such as SlotsLayout.Leading+1, for instance, if
    we want that slot to always be positioned after items with position
    SlotsLayout.Leading. This is particularly useful, for example, when
    subclassing SlotsLayout-based components which already have
    predefined slots inside them. If, for instance, the component we're
    subclassing has a slot with position SlotsLayout.Trailing, we can
    add a slot to our subclass with position SlotsLayout.Trailing-1 if
    we want to make sure our slot is always positioned before the one
    in the component we are deriving from. This scenario is shown in the
    following example code:
    \qml
    //ExampleLayout.qml
    SlotsLayout {
        id: layout
        mainSlot: Label { id: label; text: "Hello developers!" }
        CheckBox { SlotsLayout.position: SlotsLayout.Trailing }
    }
    \endqml

    \qml
    //DerivedLayout.qml
    ExampleLayout {
        id: derivedLayout
        Icon {
            id: newSlot
            width: units.gu(2)
            name: "message"
            SlotsLayout.position: SlotsLayout.Trailing - 1
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

            if (data.item != d->mainSlot) {
                d->addSlot(data.item);
                //we relayout because we have to update the width of the main slot
                //FIXME: do this in a separate function? do were really have to do the whole relayout?
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
   \qmlproperty Item SlotsLayout::mainSlot
   This property represents the main slot of the layout. By default, SlotsLayout has
   no mainSlot set.

   The main slot is the
   one that defines the vertical positioning of the other slots. Because of this,
   changing its position and overrideVerticalPositioning attached properties has
   no effect. More details can be found in the section \l {Automatic vertical positioning of slots}.

   \b {Note}: because of limitations in QtQuick/QML, it is not possible to initialize
   \l mainSlot's attached properties. Setting \l mainSlot's attached properties has
   to be done via JS, for example inside Component.onCompleted, as shown in the following example:

   \qml
    SlotsLayout {
        id: layout
        Component.onCompleted: {
            mainSlot.SlotsLayout.padding.leading =
                Qt.binding(function() { return units.gu(5) })
        }
        mainSlot: Label { id: label; text: "Hello developers!" }

        CheckBox {
            anchors.top: label.top
            SlotsLayout.position: SlotsLayout.Trailing
            SlotsLayout.overrideVerticalPositioning: true
        }
    }
   \endqml
 */
QQuickItem *UCSlotsLayout::mainSlot()
{
    Q_D(const UCSlotsLayout);
    return d->mainSlot;
}
QQuickItem *UCSlotsLayout::mainSlot() const
{
    Q_D(const UCSlotsLayout);
    return d->mainSlot;
}

void UCSlotsLayout::setMainSlot(QQuickItem *item, bool fireSignal)
{
    Q_D(UCSlotsLayout);
    if (d->mainSlot != item && item != Q_NULLPTR) {
        //if it is not handled by GC, delete the old main slot
        if (d->mainSlot != Q_NULLPTR &&
                QQmlEngine::objectOwnership(d->mainSlot) == QQmlEngine::CppOwnership) {
            delete d->mainSlot;
            d->mainSlot = Q_NULLPTR;
        }
        d->mainSlot = item;
        d->mainSlot->setParentItem(this);

        if (fireSignal) {
            Q_EMIT mainSlotChanged();
        }
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
    The default value for padding.leading is 1 Grid Unit.
    The default value for padding.trailing is 1 Grid Unit.

    The value of padding.top and padding.bottom depends on the size
    of the slots which are in the layout:
    \list
    \li if, according to the rules defined in \l {Automatic vertical positioning of slots},
        the slots are supposed to align to the top of the layout, padding.top
        and padding.bottom will have a value of \b{2} Grid Units.
    \li otherwise, if according to the rules defined in \l {Automatic vertical positioning of slots}
        the slots are supposed to be vertically centered in the layout, \b {and}
        the tallest slot (\l {mainSlot} excluded) has a height of at least 4 Grid Units,
        padding.top and padding.bottom will be set to \b{1} Grid Unit.
    \endlist

    \b {Note}: the automatic handling of padding.top and padding.bottom is disabled once either
    padding.top or padding.bottom is set to a custom value.
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
    if (!padding.leadingWasSetFromQml)
        padding.setLeading(UCUnits::instance()->gu(SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU));

    if (!padding.trailingWasSetFromQml)
        padding.setTrailing(UCUnits::instance()->gu(SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU));

    if (!padding.topWasSetFromQml)
        padding.setTop(UCUnits::instance()->gu(SLOTSLAYOUT_SLOTS_TOPBOTTOMMARGINS_GU));

    if (!padding.bottomWasSetFromQml)
        padding.setBottom(UCUnits::instance()->gu(SLOTSLAYOUT_SLOTS_TOPBOTTOMMARGINS_GU));
}

/******************************************************************************
 * UCSlotsAttached
 */
UCSlotsAttached::UCSlotsAttached(QObject *object)
    : QObject(*(new UCSlotsAttachedPrivate), object)
{
    Q_D(UCSlotsAttached);
    d->_q_onGuValueChanged();
    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(_q_onGuValueChanged()));
}

/*!
    \qmlattachedproperty enumeration SlotsLayout::position
    This attached property defines the relative position of
    the slot inside the layout. It is attached to each slot.
    The default value is SlotsLayout.Trailing.

    Valid values for \l position are:
    \list
    \li SlotsLayout.First: the slot will be positioned at the
        beginning of the layout
    \li SlotsLayout.Leading: the slot will be positioned in
        the leading slots
    \li SlotsLayout.Trailing: the slot will be positioned in
        the trailing slots, i.e. the one towards the end of the
        layout.
    \li SlotsLayout.Last: the slot will be positioned at
        the end of the layout.
    \endlist

    \l {ProgressionSlot}, for instance, has its position set to SlotsLayout.Last,
    in order to make sure the chevron is always displayed as the last trailing slot.

    Whenever there are more slots with the same \l {SlotsLayout::position},
    they will be positioned following the order in which they were added to
    the layout.

    It is also possible to define relative positioning between the slots,
    by using values such as SlotsLayout.Leading+1, or SlotsLayout.Trailing-3.
    More about this in \l {Advanced layout tweaks}.

    \b Note: you can only change the position of standard slots. Changing
    the position of \l mainSlot has no effect.
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

    The default value for SlotsLayout.padding.top is 0.
    The default value for SlotsLayout.padding.bottom is 0.
    The default value for SlotsLayout.padding.leading is 1 Grid Unit.
    The default value for SlotsLayout.padding.trailing is 1 Grid Unit.

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
  \qmlattachedproperty bool SlotsLayout::overrideVerticalPositioning

   This attached property holds whether the layout should automatically
   handle the vertical positioning of a slot. It is attached to every slot.
   The default value is \b {false}.

   If this property is set to true, the layout will ignore the
   corresponding slot during the computation of the \b implicitHeight of
   the whole layout (see \l {Resizing the layout}) and will not modify
   its vertical position during the layout process. As a consequence, it
   is possible, in that case, to manually set the vertical anchors of that slot
   (verticalCenter, top, bottom) or even its \b y property.

   Care must be taken to avoid getting the slot clipped by positioning it
   partly or completely outside of the SlotsLayout that holds it.

   \b {Note}: changing the value of overrideVerticalPositioning of one slot
   after the layout has already been initialized will reset its top, bottom,
   verticalCenter, fill, centerIn anchors for that slot, and the
   corresponding margins. Care must be taken to set the anchors to the
   desired values only after setting overrideVerticalPositioning.

   \b {Note}: this property is only valid for standard slots. Changing
   the value of \l mainSlot's overrideVerticalPositioning has no effect.
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

UT_NAMESPACE_END

#include "moc_ucslotslayout_p.cpp"

