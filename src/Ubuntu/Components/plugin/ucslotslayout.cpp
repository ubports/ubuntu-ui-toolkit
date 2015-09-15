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
    , m_parentItem(Q_NULLPTR)
    , pressedItem(Q_NULLPTR)
    , chevron(Q_NULLPTR)
    , maxSlotsHeight(0)
    , _q_cachedHeight(-1)
    , maxNumberOfLeadingSlots(1)
    , maxNumberOfTrailingSlots(2)
    , progression(false)
{
}

UCSlotsLayoutPrivate::~UCSlotsLayoutPrivate()
{
    delete chevron;
}

void UCSlotsLayoutPrivate::init()
{
    Q_Q(UCSlotsLayout);
    setDefaultLabelsProperties();

    _q_updateGuValues();

    // FIXME: This is not yet possible from C++ since we're not a StyledItem
    //QObject::connect(q, SIGNAL(themeChanged()),
    //                 q, SLOT(_q_onThemeChanged()), Qt::DirectConnection);

    QObject::connect(&contentMargins, SIGNAL(leftMarginChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&contentMargins, SIGNAL(rightMarginChanged()), q, SLOT(_q_relayout()));
    QObject::connect(&contentMargins, SIGNAL(topMarginChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
    QObject::connect(&contentMargins, SIGNAL(bottomMarginChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
    QObject::connect(q, SIGNAL(progressionChanged()), q, SLOT(_q_updateProgressionStatus()));

    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_onGuValueChanged()));

    //FIXME (if possible): this will cause relayout to be called 4-5 times when the layout has "anchors.fill: parent"
    //defined on QML side
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(_q_relayout()));

    //we connect height changes to a different function, because height changes only cause a relayout
    //in some cases (for instance when height goes from 0 to non 0)
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(_q_updateCachedHeight()));

    QObject::connect(q, SIGNAL(visibleChanged()), q, SLOT(_q_relayout()));

    //we need this to know when any of the labels is empty. In that case, we'll have to change the
    //anchors because even if a QQuickText has empty text, its height will not be 0 but "fontHeight",
    //so anchoring to text's bottom will result in the wrong outcome as a consequence.
    //TODO: updating anchors just because text changes is too much, we should probably just
    //have variables signal when a label becomes empty
    QObject::connect(&m_title, SIGNAL(textChanged(QString)), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
    QObject::connect(&m_subtitle, SIGNAL(textChanged(QString)), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
    QObject::connect(&m_subsubtitle, SIGNAL(textChanged(QString)), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

    //the height may change for many reasons, for instance:
    //- change of fontsize
    //- or resizing the layout until text wrapping is triggered
    //so we have to monitor height change as well
    QObject::connect(&m_title, SIGNAL(heightChanged()), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
    QObject::connect(&m_subtitle, SIGNAL(heightChanged()), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
    QObject::connect(&m_subsubtitle, SIGNAL(heightChanged()), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
}

UCSlotsLayoutPrivate::UCSlotPositioningMode UCSlotsLayoutPrivate::getVerticalPositioningMode() const
{
    return (labelsBoundingBoxHeight > maxSlotsHeight)
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

void UCSlotsLayoutPrivate::setDefaultLabelsProperties()
{
    Q_Q(UCSlotsLayout);
    m_title.setParentItem(q);
    m_subtitle.setParentItem(q);
    m_subsubtitle.setParentItem(q);

    m_title.setWrapMode(QQuickText::WordWrap);
    m_subtitle.setWrapMode(QQuickText::WordWrap);
    m_subsubtitle.setWrapMode(QQuickText::WordWrap);

    m_title.setElideMode(QQuickText::ElideRight);
    m_subtitle.setElideMode(QQuickText::ElideRight);
    m_subsubtitle.setElideMode(QQuickText::ElideRight);

    m_title.setMaximumLineCount(1);
    m_subtitle.setMaximumLineCount(1);
    m_subsubtitle.setMaximumLineCount(2);

    QFont titleFont = m_title.font();
    QFont subtitleFont = m_subtitle.font();
    QFont subsubtitleFont = m_subsubtitle.font();

    titleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("medium"));
    subtitleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));
    subsubtitleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));

    titleFont.setWeight(QFont::Light);
    subtitleFont.setWeight(QFont::Light);
    subsubtitleFont.setWeight(QFont::Light);

    m_title.setFont(titleFont);
    m_subtitle.setFont(subtitleFont);
    m_subsubtitle.setFont(subsubtitleFont);

    //We set the theme-dependent properties (such as the colour) later
    //as it requires qmlContext(q), which has not been initialized yet, at this point.
}

void UCSlotsLayoutPrivate::_q_onThemeChanged()
{
    Q_Q(UCSlotsLayout);
    UCTheme *theme = qmlContext(q)->contextProperty("theme").value<UCTheme*>();
    if (theme) {
        m_title.setColor(theme->getPaletteColor("selected", "backgroundText"));
        m_subtitle.setColor(theme->getPaletteColor("selected", "backgroundText"));
        m_subsubtitle.setColor(theme->getPaletteColor("selected", "backgroundText"));
    }
}

void UCSlotsLayoutPrivate::_q_onGuValueChanged()
{
    _q_updateLabelsAnchorsAndBBoxHeight();
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

void UCSlotsLayoutPrivate::_q_updateProgressionStatus()
{
    Q_Q(UCSlotsLayout);
    if (progression) {
        //Postponing parent initialization makes it so that inside itemChange method we have
        //data.item == chevron
        //Using new UCSlotsLayoutChevron(q) would call itemChange with the temporary object as parameter
        //and we wouldn't be able to recognize that it was this chevron
        chevron = new UCSlotsLayoutChevron;

        //NOTE: this may change in the future as it's an implementation detail
        //of the current Qt (5.4.2)
        //This line will force the creation of QQmlData for the chevron.
        //without this, qmlAttachedPropertiesObject on the chevron will fail,
        //as the object was created on c++ side!
        QQmlData::get(chevron, true);

        QQml_setParent_noEvent(chevron, q);
        chevron->setParentItem(q);
    } else {
        chevron->setVisible(false);
        delete chevron;
        chevron = Q_NULLPTR;
    }

    //In either case, ChildAdded or ChildRemoved will be called, and that will
    //trigger relayout
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

void UCSlotsLayoutPrivate::_q_updateLabelsAnchorsAndBBoxHeight()
{
    //if the component is not ready the QML properties may not have been evaluated yet,
    //it's not worth doing anything if that's the case
    if (!componentComplete)
        return;

    Q_Q(UCSlotsLayout);

    QQuickAnchors *titleAnchors = QQuickItemPrivate::get(&m_title)->anchors();
    titleAnchors->setTop(top());

    //even if a QQuickText is empty it will have height as if it had one character, so we can't rely
    //on just anchoring to bottom of the text above us (title in this case) because that will lead
    //to wrong positioning when title is empty
    QQuickAnchors *subtitleAnchors = QQuickItemPrivate::get(&m_subtitle)->anchors();
    subtitleAnchors->setTop(m_title.text().isEmpty()
                            ? QQuickItemPrivate::get(&m_title)->top()
                            : QQuickItemPrivate::get(&m_title)->baseline());
    subtitleAnchors->setTopMargin(m_title.text().isEmpty()
                                  ? 0
                                  : UCUnits::instance().gridUnit());

    QQuickAnchors *subsubtitleAnchors = QQuickItemPrivate::get(&m_subsubtitle)->anchors();
    subsubtitleAnchors->setTop(m_subtitle.text().isEmpty()
                               ? QQuickItemPrivate::get(&m_subtitle)->top()
                               : QQuickItemPrivate::get(&m_subtitle)->baseline());
    subsubtitleAnchors->setTopMargin(m_subtitle.text().isEmpty()
                                     ? 0
                                     : UCUnits::instance().gridUnit());

    //Update height of the labels box
    //NOTE (FIXME? it's stuff in Qt): height is not 0 when the string is empty, its default value is "fontHeight"!
    labelsBoundingBoxHeight = 0;

    bool emptyTitle = m_title.text().isEmpty();
    bool emptySubtitle = m_subtitle.text().isEmpty();
    bool emptySubsubtitle = m_subsubtitle.text().isEmpty();

    if (!emptyTitle) {
        if (emptySubtitle && emptySubsubtitle) {
            labelsBoundingBoxHeight += m_title.height();
        } else {
            labelsBoundingBoxHeight += m_title.baselineOffset() + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SPACING_GU);
        }
    }

    if (emptySubtitle) {
        if (!emptySubsubtitle) {
            labelsBoundingBoxHeight += m_subsubtitle.height();
        }
    } else {
        if (emptySubsubtitle) {
            labelsBoundingBoxHeight += m_subtitle.height();
        } else {
            labelsBoundingBoxHeight += m_subtitle.baselineOffset()
                    + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SPACING_GU)
                    + m_subsubtitle.height();
        }
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

        //skip labels
        if (child == &m_title || child == &m_subtitle || child == &m_subsubtitle)
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
    q->setImplicitHeight(qMax<qreal>(maxSlotsHeight, labelsBoundingBoxHeight)
                         + contentMargins.topMargin() + contentMargins.bottomMargin());

    _q_relayout();
}

qreal UCSlotsLayoutPrivate::populateSlotsListsAndComputeWidth()
{
    Q_Q(UCSlotsLayout);

    qint32 trailingSlotsAvailable = maxNumberOfTrailingSlots - (progression && chevron);
    qint32 leadingSlotsAvailable = maxNumberOfLeadingSlots;

    leadingSlots.clear();
    trailingSlots.clear();

    qint32 totalWidth = 0;
    const int size = q->childItems().count();
    for (int i = 0; i < size; i++) {
        QQuickItem *child = q->childItems().at(i);

        //NOTE: skip C++ labels, because we do custom layout for them (and also because
        //qmlAttachedProperties will fail on them if none of their properties is initialized via QML)
        if (child == &m_title || child == &m_subtitle
                || child == &m_subsubtitle) {
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

        //ignore if it's a chevron because we only position chevron as trailing slot
        if (attached->position() == UCSlotsLayout::Leading && child != chevron) {
            if (leadingSlots.length() < leadingSlotsAvailable) {
                leadingSlots.append(child);
                totalWidth += child->width() + attached->leftMargin() + attached->rightMargin();
            } else {
                qmlInfo(q) << "This layout only allows up to " << maxNumberOfLeadingSlots
                           << " leading slots. Please remove any additional leading slot.";
                continue;
            }
        } else if (attached->position() == UCSlotsLayout::Trailing || child == chevron) {
            //ignore positiong property and max slots available when considering the chevron
            if (trailingSlots.length() < trailingSlotsAvailable || child == chevron) {
                trailingSlots.append(child);
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

void UCSlotsLayoutPrivate::setupLabelsLeftAnchors() {
    Q_Q(UCSlotsLayout);

    int numberOfLeadingSlots = leadingSlots.length();

    //use a list to avoid copy paste errors when setting the same anchors for all the labels
    QList<QQuickAnchors *> anchors;
    anchors.append(QQuickItemPrivate::get(&m_title)->anchors());
    anchors.append(QQuickItemPrivate::get(&m_subtitle)->anchors());
    anchors.append(QQuickItemPrivate::get(&m_subsubtitle)->anchors());

    const int size = anchors.length();
    for (int i = 0; i < size; ++i) {
        QQuickAnchors* curr = anchors.at(i);
        if (numberOfLeadingSlots > 0) {
            curr->setLeft(QQuickItemPrivate::get(leadingSlots.at(numberOfLeadingSlots-1))->right());

            UCSlotsAttached *attachedLastLeadingSlot =
                    qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(leadingSlots.last()));
            if (!attachedLastLeadingSlot) {
                qmlInfo(q) << "Invalid attached property!";
            } else {
                curr->setLeftMargin(attachedLastLeadingSlot->rightMargin()
                                    + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS_GU));
            }
        } else {
            curr->setLeft(left());
            curr->setLeftMargin(contentMargins.leftMargin() + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS_GU));
        }
    }
}

void UCSlotsLayoutPrivate::setupLabelsRightAnchors()
{
    Q_Q(UCSlotsLayout);

    //use a list to avoid copy paste errors when setting the same anchors for all the labels
    QList<QQuickAnchors *> anchors;
    anchors.append(QQuickItemPrivate::get(&m_title)->anchors());
    anchors.append(QQuickItemPrivate::get(&m_subtitle)->anchors());
    anchors.append(QQuickItemPrivate::get(&m_subsubtitle)->anchors());

    const int size = anchors.length();
    if (trailingSlots.length() != 0) {
        for (int i = 0; i < size; ++i) {
            //it could be that in the previous relayout there were no trailing actions, and now there are.
            //in that case, we have to reset the anchors of the labels as they won't anchor their right
            //anymore, it's the first trailing slot that will anchor its left-anchor to the labels
            QQuickAnchors* curr = anchors.at(i);
            curr->resetRight();
            curr->resetRightMargin();
        }
    } else {
        for (int i = 0; i < size; ++i) {
            QQuickAnchors* curr = anchors.at(i);
            curr->setRight(right());
            curr->setRightMargin(contentMargins.rightMargin() + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS_GU));
        }
    }
}

void UCSlotsLayoutPrivate::setupLabelsVerticalPositioning() {
    Q_Q(UCSlotsLayout);
    QQuickAnchors *titleAnchors = QQuickItemPrivate::get(&m_title)->anchors();

    if (getVerticalPositioningMode() == UCSlotPositioningMode::AlignToTop) {
        titleAnchors->setTopMargin(contentMargins.topMargin());
    } else {
        //center the whole labels block vertically, we can't use verticalCenter as we're not using
        //a container for the labels
        titleAnchors->setTopMargin((q->height() - contentMargins.bottomMargin()
                                    + contentMargins.topMargin() - labelsBoundingBoxHeight) / 2.0) ;
    }
}

void UCSlotsLayoutPrivate::layoutLabels(qreal totalSlotsWidth)
{
    Q_Q(UCSlotsLayout);

    //width of the labels excluding the margins, this is pretty much like a "fillWidth" behaviour
    qreal labelsBoxWidth = q->width() - totalSlotsWidth - (UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS_GU) * 2)
            - contentMargins.leftMargin() - contentMargins.rightMargin();

    setupLabelsLeftAnchors();
    setupLabelsVerticalPositioning();
    setupLabelsRightAnchors();

    if (trailingSlots.length() != 0) {
        m_title.setWidth(labelsBoxWidth);
        m_subtitle.setWidth(labelsBoxWidth);
        m_subsubtitle.setWidth(labelsBoxWidth);
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

    qreal idealLabelsWidth = populateSlotsListsAndComputeWidth();

    layoutInRow(contentMargins.leftMargin(), left(), leadingSlots);

    layoutLabels(idealLabelsWidth);

    layoutInRow(UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS_GU),
                QQuickItemPrivate::get(&m_title)->right(),
                trailingSlots);
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

    d->_q_onThemeChanged();

    //We want to call these functions for the first time after the
    //QML properties (such as titleItem.text) have been initialized!
    d->_q_updateLabelsAnchorsAndBBoxHeight();
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
            QObject::connect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));
            QObject::connect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
            //we relayout because we have to update the width of the labels
            //TODO: do this in a separate function? do were really have to do the whole relayout?
            QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));

            //single labels can't be considered a slot
            if (data.item != &d->m_title && data.item != &d->m_subtitle
                    && data.item != &d->m_subsubtitle) {
                d->handleAttachedPropertySignals(data.item, true);

                //this will also trigger relayout
                d->_q_updateSlotsBBoxHeight();
            }

            //keep chevron at the end of the children so that the relayout process
            //positions it as the last trailing slot
            QList<QQuickItem *> visualChildren = childItems();
            if (progression() && d->chevron
                    && data.item != d->chevron && visualChildren.length() != 0) {
                //we have to query last() because data.item may not be the last one
                d->chevron->stackAfter(visualChildren.last());
            }
        }
        break;
    case ItemChildRemovedChange:
        if (data.item) {
            //This wouldn't be needed if the child is destroyed, but we can't know what, we just know
            //that it's changing parent, so we still disconnect from all the signals manually
            QObject::disconnect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));
            QObject::disconnect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
            QObject::disconnect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));

            if (data.item != &d->m_title && data.item != &d->m_subtitle
                    && data.item != &d->m_subsubtitle) {
                d->handleAttachedPropertySignals(data.item, false);

                //this will also trigger relayout
                d->_q_updateSlotsBBoxHeight();
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

QQuickText *UCSlotsLayout::titleItem()
{
    Q_D(UCSlotsLayout);
    return &(d->m_title);
}

QQuickText *UCSlotsLayout::subtitleItem()
{
    Q_D(UCSlotsLayout);
    return &(d->m_subtitle);
}

QQuickText *UCSlotsLayout::subsubtitleItem()
{
    Q_D(UCSlotsLayout);
    return &(d->m_subsubtitle);
}

UCSlotsLayoutMargins* UCSlotsLayout::contentMargins() {
    Q_D(UCSlotsLayout);
    return &(d->contentMargins);
}

bool UCSlotsLayout::progression() const
{
    Q_D(const UCSlotsLayout);
    return d->progression;
}

void UCSlotsLayout::setProgression(bool val)
{
    Q_D(UCSlotsLayout);
    if (d->progression != val) {
        d->progression = val;
        Q_EMIT progressionChanged();
    }
}

UCSlotsAttached *UCSlotsLayout::progressionSlot() const
{
    Q_D(const UCSlotsLayout);

    if (d->chevron == Q_NULLPTR) {
        qmlInfo(this) << "Please enable the progression symbol before trying to set its properties";
        return Q_NULLPTR;
    }

    UCSlotsAttached *attached = qobject_cast<UCSlotsAttached *>(qmlAttachedPropertiesObject<UCSlotsLayout>(d->chevron));
    //make sure QQmlEngine doesn't delete our object when doing GC
    QQmlEngine::setObjectOwnership(attached, QQmlEngine::CppOwnership);

    return (UCSlotsAttached * const) attached;
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

UCSlotsLayoutChevron::UCSlotsLayoutChevron(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    updateGuValues();
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(updateGuValues()));
}

void UCSlotsLayoutChevron::paint(QPainter *painter)
{
    painter->drawPixmap(0, 0, width(), height(), progressionPixmap);
}

void UCSlotsLayoutChevron::updateGuValues()
{
    setImplicitWidth(UCUnits::instance().gu(CHEVRON_DEFAULT_WIDTH_GU));
    setImplicitHeight(UCUnits::instance().gu(CHEVRON_DEFAULT_HEIGHT_GU));

    reloadIcon();
}

void UCSlotsLayoutChevron::reloadIcon()
{
    UnityThemeIconProvider *provider = new UnityThemeIconProvider;

    //load a new image with the correct size
    QSize loadedSize;

    //scale is so that the height is the one we want, and the width is the one which keeps the aspect ratio
    progressionPixmap = provider->requestPixmap("chevron", &loadedSize, QSize(-1, height()));

    delete provider;
    provider = Q_NULLPTR;

    setWidth(loadedSize.width());
    setHeight(loadedSize.height());
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
