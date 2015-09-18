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

    QObject::connect(&contentMargins, SIGNAL(leftMarginChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&contentMargins, SIGNAL(rightMarginChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&contentMargins, SIGNAL(topMarginChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
    QObject::connect(&contentMargins, SIGNAL(bottomMarginChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));

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

void UCSlotsLayoutPrivate::updateTopBottomMarginsIfNeeded()
{
    if (!contentMargins.topMarginWasSetFromQml) {
        contentMargins.setTopMargin((getVerticalPositioningMode() == UCSlotPositioningMode::CenterVertically
                                     && maxSlotsHeight > UCUnits::instance().gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU))
                                    ? UCUnits::instance().gu(SLOTSLAYOUT_TOPMARGIN1_GU)
                                    : UCUnits::instance().gu(SLOTSLAYOUT_TOPMARGIN2_GU));
    }

    if (!contentMargins.bottomMarginWasSetFromQml) {
        contentMargins.setBottomMargin((getVerticalPositioningMode() == UCSlotPositioningMode::CenterVertically
                                        && maxSlotsHeight > UCUnits::instance().gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU))
                                       ? UCUnits::instance().gu(SLOTSLAYOUT_BOTTOMMARGIN1_GU)
                                       : UCUnits::instance().gu(SLOTSLAYOUT_BOTTOMMARGIN2_GU));
    }

    return;
}

void UCSlotsLayoutPrivate::_q_onGuValueChanged()
{
    _q_onMainSlotHeightChanged();
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

    if (!contentMargins.leftMarginWasSetFromQml) {
        contentMargins.setLeftMargin(UCUnits::instance().gu(SLOTSLAYOUT_LEFTMARGIN_GU));
    }

    if (!contentMargins.rightMarginWasSetFromQml) {
        contentMargins.setRightMargin(UCUnits::instance().gu(SLOTSLAYOUT_RIGHTMARGIN_GU));
    }

    updateTopBottomMarginsIfNeeded();

    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_onMainSlotHeightChanged()
{
    //if the component is not ready the QML properties may not have been evaluated yet,
    //it's not worth doing anything if that's the case
    if (!componentComplete)
        return;

    Q_Q(UCSlotsLayout);

    if (mainSlot) {
        mainSlotHeight = mainSlot->height();
    } else {
        mainSlotHeight = 0;
    }

    updateTopBottomMarginsIfNeeded();
    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateSlotsBBoxHeight()
{
    if (!componentComplete)
        return;

    Q_Q(UCSlotsLayout);

    qreal maxSlotsHeightTmp = 0;
    const int size = q->childItems().count();
    for (int i = 0; i < size; i++) {
        QQuickItem *child = q->childItems().at(i);

        //skip mainSlot, we want to know the max height of the slots, mainSlot excluded
        if (child == mainSlot)
            continue;

        //ignore children which have custom vertical positioning
        UCSlotsAttached *attachedProperty =
                qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(child));

        if (!attachedProperty) {
            qmlInfo(q) << "Invalid attached property!";
            continue;
        }

        if (!attachedProperty->overrideVerticalPositioning())
            maxSlotsHeightTmp = qMax<int>(maxSlotsHeightTmp, child->height());
    }
    maxSlotsHeight = maxSlotsHeightTmp;

    updateTopBottomMarginsIfNeeded();
    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateSize()
{
    if (!componentComplete)
        return;

    Q_Q(UCSlotsLayout);
    q->setImplicitWidth(parentItem ? parentItem->width() : UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_WIDTH_GU));
    q->setImplicitHeight(qMax<qreal>(maxSlotsHeight, mainSlotHeight)
                         + contentMargins.topMargin() + contentMargins.bottomMargin());

    _q_relayout();
}

qreal UCSlotsLayoutPrivate::populateSlotsListsAndComputeWidth()
{
    Q_Q(UCSlotsLayout);

    leadingSlots.clear();
    trailingSlots.clear();
    int numberOfLeadingBeginningSlots = 0;
    int numberOfLeadingEndSlots = 0;
    int numberOfTrailingBeginningSlots = 0;
    int numberOfTrailingEndSlots = 0;
    int indexToInsertAt = -1;

    //the total width of the visible slots, paddings included
    qint32 totalWidth = 0;

    const int size = q->childItems().count();
    for (int i = 0; i < size; i++) {
        QQuickItem *child = q->childItems().at(i);
        indexToInsertAt = -1;

        //NOTE: skip mainSlot, as we handle is separately
        if (child == mainSlot) {
            continue;
        }

        if (!child->isVisible()) {
            continue;
        }

        UCSlotsAttached *attached =
                qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(child));

        if (!attached) {
            qmlInfo(q) << "Invalid attached property!";
            continue;
        }

        if (attached->position() == UCSlotsLayout::Leading
                || attached->position() == UCSlotsLayout::LeadingBeginning
                || attached->position() == UCSlotsLayout::LeadingEnd) {

            int totalLeadingSlots = leadingSlots.length();
            if (leadingSlots.length() < maxNumberOfLeadingSlots) {
                switch (attached->position()) {
                case UCSlotsLayout::LeadingBeginning:
                    indexToInsertAt = numberOfLeadingBeginningSlots;
                    ++numberOfLeadingBeginningSlots;
                    break;
                case UCSlotsLayout::Leading:
                    indexToInsertAt = totalLeadingSlots - numberOfLeadingEndSlots;
                    break;
                case UCSlotsLayout::LeadingEnd:
                    indexToInsertAt = totalLeadingSlots;
                    ++numberOfLeadingEndSlots;
                    break;
                default:
                    break;
                }

                leadingSlots.insert(indexToInsertAt, child);
                totalWidth += child->width() + attached->leftMargin() + attached->rightMargin();
            } else {
                qmlInfo(q) << "This layout only allows up to " << maxNumberOfLeadingSlots
                           << " leading slots. Please remove any additional leading slot.";
                continue;
            }

        } else if (attached->position() == UCSlotsLayout::Trailing
                   || attached->position() == UCSlotsLayout::TrailingBeginning
                   || attached->position() == UCSlotsLayout::TrailingEnd) {

            int totalTrailingSlots = trailingSlots.length();
            if (trailingSlots.length() < maxNumberOfTrailingSlots) {
                switch (attached->position()) {
                case UCSlotsLayout::TrailingBeginning:
                    indexToInsertAt = numberOfTrailingBeginningSlots;
                    ++numberOfTrailingBeginningSlots;
                    break;
                case UCSlotsLayout::Trailing:
                    indexToInsertAt = totalTrailingSlots - numberOfTrailingEndSlots;
                    break;
                case UCSlotsLayout::TrailingEnd:
                    indexToInsertAt = totalTrailingSlots;
                    ++numberOfTrailingEndSlots;
                    break;
                default:
                    break;
                }

                trailingSlots.insert(indexToInsertAt, child);
                totalWidth += child->width() + attached->leftMargin() + attached->rightMargin();
            } else {
                qmlInfo(q) << "This layout only allows up to " << maxNumberOfTrailingSlots
                           << " trailing slots. Please remove any additional trailing slot.";
                continue;
            }
        } else {
            qmlInfo(q) << "Unrecognized position value!";
            continue;
        }
    }

    return totalWidth;
}

void UCSlotsLayoutPrivate::setupSlotsVerticalPositioning(QQuickItem *slot)
{
    if (slot == Q_NULLPTR)
        return;

    QQuickAnchors *slotAnchors = QQuickItemPrivate::get(slot)->anchors();

    if (getVerticalPositioningMode() == UCSlotPositioningMode::AlignToTop) {
        //reset the vertical anchor as we might be transitioning from the configuration
        //where all items are vertically centered to the one where they're anchored to top
        slotAnchors->resetVerticalCenter();
        slotAnchors->setVerticalCenterOffset(0);

        slotAnchors->setTop(top());
        slotAnchors->setTopMargin(contentMargins.topMargin());
    } else {
        slotAnchors->resetTop();

        slotAnchors->setVerticalCenter(verticalCenter());
        //bottom and top offsets could have different values
        slotAnchors->setVerticalCenterOffset((contentMargins.topMargin() - contentMargins.bottomMargin()) / 2.0);
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
            setupSlotsVerticalPositioning(item);
        }

        if (i == 0) {
            //skip anchoring if the anchor given as input is invalid
            if (siblingAnchor.item == Q_NULLPTR)
                continue;

            itemAnchors->setLeft(siblingAnchor);
            itemAnchors->setLeftMargin(attached->leftMargin() + siblingAnchorMargin);
        } else {
            UCSlotsAttached *attachedPreviousItem =
                    qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(items.at(i - 1)));

            if (!attachedPreviousItem) {
                qmlInfo(q) << "Invalid attached property!";
            } else {
                itemAnchors->setLeft(QQuickItemPrivate::get(items.at(i - 1))->right());
                itemAnchors->setLeftMargin(attachedPreviousItem->rightMargin() + attached->leftMargin());
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

    qreal totalSlotsWidth = populateSlotsListsAndComputeWidth();

    QList<QQuickItem *> itemsToLayout(leadingSlots);

    if (mainSlot) {
        itemsToLayout.append(mainSlot);

        UCSlotsAttached *attachedSlot =
                qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(mainSlot));

        if (!attachedSlot) {
            qmlInfo(q) << "Invalid attached property!";
            return;
        }
        mainSlot->setWidth(q->width() - totalSlotsWidth
                           - attachedSlot->leftMargin() - attachedSlot->rightMargin()
                           - contentMargins.leftMargin() - contentMargins.rightMargin());
    } else {
        Q_UNUSED(totalSlotsWidth);
    }

    itemsToLayout.append(trailingSlots);

    layoutInRow(contentMargins.leftMargin(), left(), itemsToLayout);
}

void UCSlotsLayoutPrivate::handleAttachedPropertySignals(QQuickItem *item, bool connect)
{
    Q_Q(UCSlotsLayout);
    UCSlotsAttached *attachedSlot =
            qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(item));

    if (!attachedSlot) {
        qmlInfo(q) << "Invalid attached property!";
        return;
    }

    if (connect) {
        QObject::connect(attachedSlot, SIGNAL(positionChanged()), q, SLOT(_q_relayout()));
        QObject::connect(attachedSlot, SIGNAL(leftMarginChanged()), q, SLOT(_q_relayout()));
        QObject::connect(attachedSlot, SIGNAL(rightMarginChanged()), q, SLOT(_q_relayout()));
        QObject::connect(attachedSlot, SIGNAL(overrideVerticalPositioningChanged()), q, SLOT(_q_relayout()));
    } else {
        QObject::disconnect(attachedSlot, SIGNAL(positionChanged()), q, SLOT(_q_relayout()));
        QObject::disconnect(attachedSlot, SIGNAL(leftMarginChanged()), q, SLOT(_q_relayout()));
        QObject::disconnect(attachedSlot, SIGNAL(rightMarginChanged()), q, SLOT(_q_relayout()));
        QObject::disconnect(attachedSlot, SIGNAL(overrideVerticalPositioningChanged()), q, SLOT(_q_relayout()));
    }
}

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

    d->_q_onMainSlotHeightChanged();
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

            QObject::connect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));

            //we relayout because we have to update the width of the main slot
            //TODO: do this in a separate function? do were really have to do the whole relayout?
            if (data.item != d->mainSlot) {
                QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));
                QObject::connect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
                d->_q_updateSlotsBBoxHeight();
            } else {
                QObject::connect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_onMainSlotHeightChanged()));
                d->_q_onMainSlotHeightChanged();
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
                QObject::disconnect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));
                QObject::disconnect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
                d->_q_updateSlotsBBoxHeight();
            } else {
                QObject::disconnect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_onMainSlotHeightChanged()));
                d->_q_onMainSlotHeightChanged();
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

UCSlotsLayoutMargins* UCSlotsLayout::contentMargins() {
    Q_D(UCSlotsLayout);
    return &(d->contentMargins);
}

/******************************************************************************
 * UCSlotsAttachedPrivate
 */
UCSlotsAttachedPrivate::UCSlotsAttachedPrivate()
    : QObjectPrivate()
    , position(UCSlotsLayout::Trailing)
    , leftMargin(0)
    , rightMargin(0)
    , overrideVerticalPositioning(false)
    , leftMarginWasSetFromQml(false)
    , rightMarginWasSetFromQml(false)
{
}

void UCSlotsAttachedPrivate::_q_onGuValueChanged()
{
    Q_Q(UCSlotsAttached);
    if (!leftMarginWasSetFromQml)
        q->setLeftMargin(UCUnits::instance().gu(SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU));

    if (!rightMarginWasSetFromQml)
        q->setRightMargin(UCUnits::instance().gu(SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU));
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

qreal UCSlotsAttached::leftMargin() const
{
    Q_D(const UCSlotsAttached);
    return d->leftMargin;
}

void UCSlotsAttached::setLeftMargin(qreal margin)
{
    Q_D(UCSlotsAttached);
    if (d->leftMargin != margin) {
        d->leftMargin = margin;
        Q_EMIT leftMarginChanged();
    }
}

void UCSlotsAttached::setLeftMarginQml(qreal margin)
{
    Q_D(UCSlotsAttached);
    d->leftMarginWasSetFromQml = true;

    //if both have been set from QML, then disconnect the signal from the slot, to avoid overwriting dev's values
    //when GU changes
    if (d->rightMarginWasSetFromQml)
        QObject::disconnect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(_q_onGuValueChanged()));

    setLeftMargin(margin);
}

qreal UCSlotsAttached::rightMargin() const
{
    Q_D(const UCSlotsAttached);
    return d->rightMargin;
}

void UCSlotsAttached::setRightMargin(qreal margin)
{
    Q_D(UCSlotsAttached);
    if (d->rightMargin != margin) {
        d->rightMargin = margin;
        Q_EMIT rightMarginChanged();
    }
}

void UCSlotsAttached::setRightMarginQml(qreal margin)
{
    Q_D(UCSlotsAttached);
    d->rightMarginWasSetFromQml = true;

    //if both have been set from QML, then disconnect the signal from the slot, to avoid overwriting dev's values
    //when GU changes
    if (d->leftMarginWasSetFromQml)
        QObject::disconnect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(_q_onGuValueChanged()));

    setRightMargin(margin);
}

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

UCSlotsLayoutMargins::UCSlotsLayoutMargins(QObject *parent)
    : QObject(parent)
    , leftMarginWasSetFromQml(false)
    , rightMarginWasSetFromQml(false)
    , topMarginWasSetFromQml(false)
    , bottomMarginWasSetFromQml(false)
    , m_leftMargin(0)
    , m_rightMargin(0)
    , m_topMargin(0)
    , m_bottomMargin(0)
{
}

qreal UCSlotsLayoutMargins::leftMargin() const
{
    return m_leftMargin;
}

void UCSlotsLayoutMargins::setLeftMargin(qreal val)
{
    if (m_leftMargin != val) {
        m_leftMargin = val;
        Q_EMIT leftMarginChanged();
    }
}

void UCSlotsLayoutMargins::setLeftMarginQml(qreal val)
{
    leftMarginWasSetFromQml = true;
    setLeftMargin(val);
}

qreal UCSlotsLayoutMargins::rightMargin() const
{
    return m_rightMargin;
}

void UCSlotsLayoutMargins::setRightMargin(qreal val)
{
    if (m_rightMargin != val) {
        m_rightMargin = val;
        Q_EMIT rightMarginChanged();
    }
}

void UCSlotsLayoutMargins::setRightMarginQml(qreal val)
{
    rightMarginWasSetFromQml = true;
    setRightMargin(val);
}

qreal UCSlotsLayoutMargins::topMargin() const
{
    return m_topMargin;
}

void UCSlotsLayoutMargins::setTopMargin(qreal val)
{
    if (m_topMargin != val) {
        m_topMargin = val;
        Q_EMIT topMarginChanged();
    }
}

void UCSlotsLayoutMargins::setTopMarginQml(qreal val)
{
    topMarginWasSetFromQml = true;
    setTopMargin(val);
}

qreal UCSlotsLayoutMargins::bottomMargin() const
{
    return m_bottomMargin;
}

void UCSlotsLayoutMargins::setBottomMargin(qreal val)
{
    if (m_bottomMargin != val) {
        m_bottomMargin = val;
        Q_EMIT bottomMarginChanged();
    }
}

void UCSlotsLayoutMargins::setBottomMarginQml(qreal val)
{
    bottomMarginWasSetFromQml = true;
    setBottomMargin(val);
}


#include "moc_ucslotslayout.cpp"
