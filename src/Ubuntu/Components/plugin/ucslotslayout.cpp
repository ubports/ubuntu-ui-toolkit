#include "ucunits.h"
#include "ucslotslayout.h"
#include "ucslotslayout_p.h"
#include "ucfontutils.h"
#include "QQmlEngine"
#include "unitythemeiconprovider.h"

UCSlotsAttached::UCSlotsAttached(QObject *object)
    : QObject(object)
    , m_position(UCSlotsLayout::Trailing)
    , m_leftMargin(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTSLOTSIDEMARGINS_GU))
    , m_rightMargin(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTSLOTSIDEMARGINS_GU))
    , m_overrideVerticalPositioning(false)
{
    //FIXME: if the user defines SlotsLayout.leftMargin and then GU changes, we will overwrite it!
    //But how to avoid that? We should check if the user has initizalized leftMargin, but how?
    //Maybe we should cache the old GU value and check if the margins value were modified?
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(updateGuValues()));
}

UCSlotsLayout::UCSlotPosition UCSlotsAttached::position() const {
    return m_position;
}

void UCSlotsAttached::setPosition(UCSlotsLayout::UCSlotPosition pos) {
    if (m_position != pos) {
        m_position = pos;
        Q_EMIT positionChanged();
    }
}

qreal UCSlotsAttached::leftMargin() const {
    return m_leftMargin;
}

void UCSlotsAttached::setLeftMargin(qreal margin) {
    if (m_leftMargin != margin) {
        m_leftMargin = margin;
        Q_EMIT leftMarginChanged();
    }
}

qreal UCSlotsAttached::rightMargin() const {
    return m_rightMargin;
}

void UCSlotsAttached::setRightMargin(qreal margin) {
    if (m_rightMargin != margin) {
        m_rightMargin = margin;
        Q_EMIT rightMarginChanged();
    }
}

bool UCSlotsAttached::overrideVerticalPositioning() const {
    return m_overrideVerticalPositioning;
}

void UCSlotsAttached::setOverrideVerticalPositioning(bool val) {
    if (m_overrideVerticalPositioning != val) {
        m_overrideVerticalPositioning = val;
        Q_EMIT overrideVerticalPositioningChanged();
    }
}

void UCSlotsAttached::updateGuValues() {
    setLeftMargin(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTSLOTSIDEMARGINS_GU));
    setRightMargin(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTSLOTSIDEMARGINS_GU));
}

UCSlotsAttached* UCSlotsLayout::qmlAttachedProperties(QObject *object)
{
    return new UCSlotsAttached(object);
}

/******************************************************************************
 * UCSlotsLayoutPrivate
 */
UCSlotsLayoutPrivate::UCSlotsLayoutPrivate()
    : QQuickItemPrivate()
    , ready(false)
    , pressedItem(0)
    , maxChildrenHeight(0)
    , _q_cachedHeight(-1)
    , leftOffset(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTLAYOUTSIDEMARGINS_GU))
    , rightOffset(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTLAYOUTSIDEMARGINS_GU))
    , topOffset(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTLAYOUTSIDEMARGINS_GU))
    , bottomOffset(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTLAYOUTSIDEMARGINS_GU))
    , progression(false)
    , chevron(0)
    , leftOffsetWasSetFromQml(false)
    , rightOffsetWasSetFromQml(false)
    , topOffsetWasSetFromQml(false)
    , bottomOffsetWasSetFromQml(false)
    , m_parentItem(0)
{
}

UCSlotsLayoutPrivate::~UCSlotsLayoutPrivate()
{
}

void UCSlotsLayoutPrivate::init()
{
    Q_Q(UCSlotsLayout);
    setDefaultLabelsProperties();

    QObject::connect(q, SIGNAL(leftOffsetChanged()), q, SLOT(_q_relayout()));
    QObject::connect(q, SIGNAL(rightOffsetChanged()), q, SLOT(_q_relayout()));
    QObject::connect(q, SIGNAL(topOffsetChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
    QObject::connect(q, SIGNAL(bottomOffsetChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));
    QObject::connect(q, SIGNAL(progressionChanged()), q, SLOT(_q_updateProgressionStatus()));

    // connect theme changes
    //QObject::connect(q, SIGNAL(themeChanged()),
    //                 q, SLOT(_q_themeChanged()), Qt::DirectConnection);

    // watch grid unit size change and set implicit size
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSlotsBBoxHeight()));

    //fixme: this will cause relayout to be called 4-5 times when the layout has "anchors.fill: parent"
    //defined on QML side
    //the ideal would be to only bind to width and implicitWidth changes, and for the rest only
    //call relayout() if it makes sense (for instance if the height goes from 0 to non 0), but that
    //requires caching all those variables
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(_q_relayout()));

    //we redirect height changes to different functions, because height changes only cause a relayout
    //in some cases (for instance when height goes from 0 to non 0)
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(_q_updateCachedHeight()));

    QObject::connect(q, SIGNAL(visibleChanged()), q, SLOT(_q_relayout()));
    QObject::connect(q, SIGNAL(relayoutNeeded()), q, SLOT(_q_relayout()));

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
    //- etc etc...
    //so we have to monitor height change as well
    QObject::connect(&m_title, SIGNAL(heightChanged()), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
    QObject::connect(&m_subtitle, SIGNAL(heightChanged()), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
    QObject::connect(&m_subsubtitle, SIGNAL(heightChanged()), q, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

}

int UCSlotsLayoutPrivate::getVerticalPositioningMode() {
    //we could just return the bool, I just want to stress that there could be other
    //modes in the future
    return (labelsBoundingBoxHeight > maxChildrenHeight) ? 1 : 0;
}

void UCSlotsLayoutPrivate::updateTopBottomOffsetsIfNeeded() {
    Q_Q(UCSlotsLayout);
    if (!topOffsetWasSetFromQml) {
        q->setTopOffset((!getVerticalPositioningMode() && maxChildrenHeight > UCUnits::instance().gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD))
                        ? UCUnits::instance().gu(1.0)
                        : UCUnits::instance().gu(2.0));
    }

    if (!bottomOffsetWasSetFromQml) {
        q->setBottomOffset((!getVerticalPositioningMode() && maxChildrenHeight > UCUnits::instance().gu(SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD))
                           ? UCUnits::instance().gu(1.0)
                           : UCUnits::instance().gu(2.0));
    }
}

void UCSlotsLayoutPrivate::_q_updateCachedHeight() {
    Q_Q(UCSlotsLayout);
    if (_q_cachedHeight != q->height()) {
        if (_q_cachedHeight == 0) {
            Q_EMIT q->relayoutNeeded();
        }
        _q_cachedHeight = q->height();
    }
}

void UCSlotsLayoutPrivate::_q_updateProgressionStatus() {
    Q_Q(UCSlotsLayout);
    if (progression) {
        //No parents on purpose! We'll set the parent after we force the creation of QQmlData,
        //otherwise qmlAttachedPropertiesObject in the relayout function (triggered by parent's ChildrenAdded signal)
        //will fail
        chevron = new UCSlotsLayoutChevron;

        //NOTE: this may change in the future as it's an implementation detail
        //of the current Qt (5.4.2)
        //This line will force the creation of QQmlData for the chevron.
        //without this, qmlAttachedPropertiesObject on the chevron will fail,
        //as it was created on c++ side!
        QQmlData::get(chevron, true);

        chevron->setParent(q);
        chevron->setParentItem(q);
        QQmlEngine::setObjectOwnership(chevron, QQmlEngine::CppOwnership);

    } else {
        chevron->setVisible(false);
        delete chevron;
        chevron = 0;
    }

    Q_EMIT q->relayoutNeeded();
}

void UCSlotsLayoutPrivate::setDefaultLabelsProperties() {
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

    //FIXME: hardcoded colours!! Because we currently don't have access to the color palette from C++
    m_title.setColor(QColor("#525252"));
    m_subtitle.setColor(QColor("#525252"));
    m_subsubtitle.setColor(QColor("#525252"));

    //do we want to disable positioning when the developer specifies custom anchoring?
    //QQuickAnchors* subtitleAnchors = QQuickItemPrivate::get(&m_subtitle)->anchors();
    //QObject::connect(subtitleAnchors, SIGNAL(topChanged()) , q, _q_disableLabelsPositioning());
}

void UCSlotsLayoutPrivate::_q_updateLabelsAnchorsAndBBoxHeight() {
    //if the component is not ready the QML properties may not have been evaluated yet,
    //it's not worth doing anything if that's the case
    if (!ready) return;

    Q_Q(UCSlotsLayout);

    //qDebug() << "updating anchors";
    QQuickAnchors* titleAnchors = QQuickItemPrivate::get(&m_title)->anchors();
    titleAnchors->setTop(top());

    //even if a QQuickText is empty it will have height as if it had one character, so we can't rely
    //on just anchoring to bottom of the text above us (title in this case) because that will lead
    //to wrong positioning when title is empty
    QQuickAnchors* subtitleAnchors = QQuickItemPrivate::get(&m_subtitle)->anchors();
    subtitleAnchors->setTop(m_title.text() == ""
                            ? QQuickItemPrivate::get(&m_title)->top()
                            : QQuickItemPrivate::get(&m_title)->baseline());
    subtitleAnchors->setTopMargin(m_title.text() == ""
                                  ? 0
                                  : UCUnits::instance().gridUnit());

    QQuickAnchors* subsubtitleAnchors = QQuickItemPrivate::get(&m_subsubtitle)->anchors();
    subsubtitleAnchors->setTop(m_subtitle.text() == ""
                               ? QQuickItemPrivate::get(&m_subtitle)->top()
                               : QQuickItemPrivate::get(&m_subtitle)->baseline());
    subsubtitleAnchors->setTopMargin(m_subtitle.text() == ""
                                     ? 0
                                     : UCUnits::instance().gridUnit());

    //Update height of the labels box
    //NOTE (FIXME? it's stuff in Qt): height is not 0 when the string is empty, its default value is "fontHeight"!
    labelsBoundingBoxHeight = 0;

    bool emptyTitle = m_title.text() == "";
    bool emptySubtitle = m_subtitle.text() == "";
    bool emptySubsubtitle = m_subsubtitle.text() == "";

    if (!emptyTitle) {
        if (emptySubtitle && emptySubsubtitle) {
            labelsBoundingBoxHeight += m_title.height();
        } else {
            labelsBoundingBoxHeight += m_title.baselineOffset() + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SPACING);
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
                    + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SPACING)
                    + m_subsubtitle.height();
        }
    }

    updateTopBottomOffsetsIfNeeded();
    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateSlotsBBoxHeight() {
    if (!ready) return;

    Q_Q(UCSlotsLayout);

    qreal maxSlotsHeight = 0;
    for (int i=0; i<q->childItems().count(); i++) {
        QQuickItem* child = qobject_cast<QQuickItem*>(q->childItems().at(i));

        //skip labels
        if (child == &m_title || child == &m_subtitle || child == &m_subsubtitle) {
            continue;
        }

        //ignore children which have custom vertical positioning
        UCSlotsAttached *attachedProperty =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(child));

        if (!attachedProperty) {
            qDebug() << "SLOTSLAYOUT: invalid attached property!";
            continue;
        }

        if (!attachedProperty->overrideVerticalPositioning()) {
            maxSlotsHeight = qMax<int>(maxSlotsHeight, child->height());
        }
    }
    maxChildrenHeight = maxSlotsHeight;

    updateTopBottomOffsetsIfNeeded();
    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateSize()
{
    if (!ready) {
        //qDebug() << "Skipping updatesize, component is not ready";
        return;
    }

    Q_Q(UCSlotsLayout);
    QQuickItem *parent = qobject_cast<QQuickItem*>(q->parentItem());
    q->setImplicitWidth(parent ? parent->width() : UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_WIDTH_GU));
    q->setImplicitHeight(qMax<qreal>(maxChildrenHeight, labelsBoundingBoxHeight)
                         + topOffset + bottomOffset);

    Q_EMIT q->relayoutNeeded();
}

void UCSlotsLayoutPrivate::_q_relayout() {
    //only relayout after the component has been initialized
    Q_Q(UCSlotsLayout);
    if (ready) {
        //qDebug() << "RELAYOUT!";
    } else {
        //qDebug() << "RELAYOUT SKIPPED!";
        return;
    }

    //if either of the side lengths is non-positive, or the item is not visible, skip relayout
    if (q->width() <= 0 || q->height() <= 0 || !q->isVisible() || !q->opacity()) {
        return;
    }

    QList<QQuickItem*> leadingSlots;
    QList<QQuickItem*> trailingSlots;
    //int currentX = 0;
    int totalWidth = 0;

    //reorder children and assign them to the sorted list
    for (int i=0; i<q->childItems().length(); i++) {
        QQuickItem* child = qobject_cast<QQuickItem*>(q->childItems().at(i));

        //NOTE: skip C++ labels, because we do custom layout for them (and also because
        //qmlAttachedProperties will fail on them if none of their properties is initialized via QML)
        //We also skip the chevron as we handle it separately later
        if (child == &m_title || child == &m_subtitle
                || child == &m_subsubtitle || child == chevron) {
            continue;
        }

        //get the attached property, creating it if it wasn't defined already
        UCSlotsAttached *attached =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(child));

        if (!attached) {
            qDebug() << "SLOTSLAYOUT: invalid attached property!";
            continue;
        }

        //if (!position.isValid()) continue;
        //TODO: IGNORE INVISIBLE CHILDREN?

        if (attached->position() == UCSlotsLayout::Leading) {
            //FIXME: is this safe?
            leadingSlots.append(static_cast<QQuickItem*>(child));
            totalWidth += static_cast<QQuickItem*>(child)->width() + attached->leftMargin() + attached->rightMargin();
        } else if (attached->position() == UCSlotsLayout::Trailing) {
            trailingSlots.append(static_cast<QQuickItem*>(child));
            totalWidth += static_cast<QQuickItem*>(child)->width() + attached->leftMargin() + attached->rightMargin();
        } else {
            qDebug() << "SlotsLayout: unrecognized position value, please use SlotsLayout.Leading or SlotsLayout.Trailing";
        }
    }

    //put the chevron at the end, no matter what its position in childItems() is
    //DOC NOTE: this means its "position" property will be ignored!
    if (progression && chevron) {
        //get the attached property, creating it if it wasn't defined already
        UCSlotsAttached *attached =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(chevron));

        if (!attached) {
            qDebug() << "SLOTSLAYOUT: invalid attached property!";
        } else {
            trailingSlots.append(static_cast<QQuickItem*>(chevron));
            totalWidth += static_cast<QQuickItem*>(chevron)->width() + attached->leftMargin() + attached->rightMargin();
        }
    }

    QQuickItemPrivate* _q_private = QQuickItemPrivate::get(q);

    for (int i=0; i<leadingSlots.length(); i++) {
        UCSlotsAttached *attached =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(leadingSlots.at(i)));

        if (!attached) {
            qDebug() << "SLOTSLAYOUT: invalid attached property!";
            continue;
        }

        QQuickItemPrivate* item = QQuickItemPrivate::get((QQuickItem*) leadingSlots.at(i));

        if (!attached->overrideVerticalPositioning()) {
            if (getVerticalPositioningMode()) {
                //reset the vertical anchor as we might be transitioning from the configuration
                //where all items are vertically centered to the one where they're anchored to top
                item->anchors()->resetVerticalCenter();
                item->anchors()->setVerticalCenterOffset(0);

                item->anchors()->setTop(_q_private->top());
                item->anchors()->setTopMargin(topOffset);
            } else {
                item->anchors()->resetTop();

                item->anchors()->setVerticalCenter(_q_private->verticalCenter());
                //bottom and top margin could be different, we have to take that into account
                item->anchors()->setVerticalCenterOffset((topOffset - bottomOffset) / 2.0);
            }
        }

        if (i==0) {
            item->anchors()->setLeft(_q_private->left());
            item->anchors()->setLeftMargin(attached->leftMargin() + leftOffset);
        } else {
            UCSlotsAttached *attachedPreviousItem =
                    qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(leadingSlots.at(i-1)));

            if (!attachedPreviousItem) {
                qDebug() << "SLOTSLAYOUT: invalid attached property!";
            } else {
                item->anchors()->setLeft(QQuickItemPrivate::get(static_cast<QQuickItem*>(leadingSlots.at(i-1)))->right());
                item->anchors()->setLeftMargin(attachedPreviousItem->rightMargin() + attached->leftMargin());
            }
        }
        //currentX += item->x + item->width + attached->leftMargin() + attached->rightMargin();
    }

    int numberOfLeadingSlots = leadingSlots.length();
    int numberOfTrailingSlots = trailingSlots.length();

    QQuickAnchorLine labelsLeftAnchor = numberOfLeadingSlots ? QQuickItemPrivate::get(static_cast<QQuickItem*>(leadingSlots.at(numberOfLeadingSlots-1)))->right()
                                                             : _q_private->left();

    QQuickAnchors* titleAnchors = QQuickItemPrivate::get(&m_title)->anchors();
    QQuickAnchors* subtitleAnchors = QQuickItemPrivate::get(&m_subtitle)->anchors();
    QQuickAnchors* subsubtitleAnchors = QQuickItemPrivate::get(&m_subsubtitle)->anchors();

    titleAnchors->setLeft(labelsLeftAnchor);
    subtitleAnchors->setLeft(labelsLeftAnchor);
    subsubtitleAnchors->setLeft(labelsLeftAnchor);

    qreal labelBoxWidth = q->width() - totalWidth - UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS)*2
            - leftOffset - rightOffset;

    if (numberOfLeadingSlots != 0) {
        UCSlotsAttached *attachedLastLeadingSlot =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(leadingSlots.last()));

        if (!attachedLastLeadingSlot) {
            qDebug() << "SLOTSLAYOUT: invalid attached property!";
        } else {
            titleAnchors->setLeftMargin(attachedLastLeadingSlot->rightMargin() + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
            subtitleAnchors->setLeftMargin(attachedLastLeadingSlot->rightMargin() + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
            subsubtitleAnchors->setLeftMargin(attachedLastLeadingSlot->rightMargin() + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
        }
    } else {
        titleAnchors->setLeftMargin(leftOffset + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
        subtitleAnchors->setLeftMargin(leftOffset + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
        subsubtitleAnchors->setLeftMargin(leftOffset + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
    }

    //new visual design rules
    if (getVerticalPositioningMode()) {
        titleAnchors->setTopMargin(topOffset);
    } else {
        //center the whole labels block vertically, we can't use verticalCenter as we don't
        //have a QQuickItem which contains the labels
        titleAnchors->setTopMargin((q->height() - bottomOffset + topOffset - labelsBoundingBoxHeight) / 2.0) ;
    }

    //currentX += labelBoxWidth + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN);

    if (numberOfTrailingSlots != 0) {
        //it could be that in the previous relayout there were no trailing actions, and now there are.
        //in that case, we have to reset the anchors of the labels as they won't anchor their right
        //anymore, it's the first trailing slot that will anchor its left-anchor to the labels
        titleAnchors->resetRight();
        titleAnchors->resetRightMargin();
        subtitleAnchors->resetRight();
        subtitleAnchors->resetRightMargin();
        subsubtitleAnchors->resetRight();
        subsubtitleAnchors->resetRightMargin();

        m_title.setWidth(labelBoxWidth);
        m_subtitle.setWidth(labelBoxWidth);
        m_subsubtitle.setWidth(labelBoxWidth);

        for (int i=0; i<trailingSlots.length(); i++) {
            QQuickItemPrivate* item = QQuickItemPrivate::get(static_cast<QQuickItem*>(trailingSlots.at(i)));

            UCSlotsAttached *attached =
                    qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(trailingSlots.at(i)));

            if (!attached) {
                qDebug() << "SLOTSLAYOUT: invalid attached property!";
                continue;
            }

            if (!attached->overrideVerticalPositioning()) {
                if (getVerticalPositioningMode()) {
                    //reset the vertical anchor as we might be transitioning from the configuration
                    //where all items are vertically centered to the one where they're anchored to top
                    item->anchors()->resetVerticalCenter();
                    item->anchors()->setVerticalCenterOffset(0);

                    item->anchors()->setTop(_q_private->top());
                    item->anchors()->setTopMargin(topOffset);
                } else {
                    //reset the anchor as we may be switching to one vertical positioning mode to another
                    item->anchors()->resetTop();

                    item->anchors()->setVerticalCenter(_q_private->verticalCenter());
                    //bottom and top margin could be different, we have to take that into account
                    item->anchors()->setVerticalCenterOffset((topOffset - bottomOffset) / 2.0);
                }
            }

            if (i==0) {
                item->anchors()->setLeft(QQuickItemPrivate::get(&m_title)->right());
                //the 2GU right margin of the labels is treated as left margin of the first trailing slot
                //because labels don't have a right anchor set up here!
                item->anchors()->setLeftMargin(UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS) + attached->leftMargin());
            } else {
                UCSlotsAttached *attachedPreviousItem =
                        qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(trailingSlots.at(i-1)));

                if (!attachedPreviousItem) {
                    qDebug() << "SLOTSLAYOUT: invalid attached property!";
                } else {
                    item->anchors()->setLeft(QQuickItemPrivate::get(static_cast<QQuickItem*>(trailingSlots.at(i-1)))->right());
                    item->anchors()->setLeftMargin(attachedPreviousItem->rightMargin() + attached->leftMargin());
                }
            }
            //currentX += item->x + item->width + attached->leftMargin() + attached->rightMargin();
        }
    } else {
        titleAnchors->setRight(_q_private->right());
        titleAnchors->setRightMargin(rightOffset + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
        subtitleAnchors->setRight(_q_private->right());
        subtitleAnchors->setRightMargin(rightOffset + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
        subsubtitleAnchors->setRight(_q_private->right());
        subsubtitleAnchors->setRightMargin(rightOffset + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SIDEMARGINS));
    }
}

UCSlotsLayout::UCSlotsLayout(QQuickItem *parent) :
    QQuickItem(*(new UCSlotsLayoutPrivate), parent)
{
    setFlag(ItemHasContents);
    Q_D(UCSlotsLayout);
    d->init();

    setAcceptedMouseButtons(Qt::LeftButton);
}

void UCSlotsLayout::classBegin() {
    /*Q_D(UCSlotsLayout);

    UCSlotsAttached *titleAttachedProperty =
            qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(&d->m_title));
    UCSlotsAttached *subtitleAttachedProperty =
            qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(&d->m_subtitle));
    UCSlotsAttached *subsubtitleAttachedProperty =
            qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(&d->m_subsubtitle));

    if (!titleAttachedProperty) qDebug() << "FAILING";
    else {
        qDebug() << titleAttachedProperty->leftMargin();

    }
    Q_UNUSED(titleAttachedProperty);
    Q_UNUSED(subtitleAttachedProperty);
    Q_UNUSED(subsubtitleAttachedProperty);*/

}

void UCSlotsLayout::componentComplete() {
    QQuickItem::componentComplete();

    Q_D(UCSlotsLayout);
    d->ready = true;

    //We want to call these functions for the first time after the
    //QML properties (such as titleItem.text) have been initialized!
    d->_q_updateLabelsAnchorsAndBBoxHeight();
    d->_q_updateSlotsBBoxHeight();

}

void UCSlotsLayout::itemChange(ItemChange change, const ItemChangeData &data)
{    
    Q_D(UCSlotsLayout);

    switch (change) {
    case ItemChildAddedChange:
        if (data.item) {
            QObject::connect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));
            QObject::connect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
            //we relayout because we have to update the width of the labels
            //TODO: do this in a separate function? do were really have to do the whole relayout?
            QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));

            //this will also trigger relayout
            d->_q_updateSlotsBBoxHeight();
        }
        break;
    case ItemChildRemovedChange:
        if (data.item) {
            QObject::disconnect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));
            QObject::disconnect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
            QObject::disconnect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));
            d->_q_updateSlotsBBoxHeight();
        }
        break;
    default:
        break;
    }

    if (change == ItemParentHasChanged) {
        Q_D(UCSlotsLayout);
        QQuickItem* newParent = data.item;
        if (newParent) {
            if (d->m_parentItem) {
                QObject::disconnect(d->m_parentItem, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
            }

            d->m_parentItem = newParent;
            QObject::connect(newParent, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()), Qt::DirectConnection);
            d->_q_updateSize();
        }
    }

    QQuickItem::itemChange(change, data);

}

QQuickText* UCSlotsLayout::titleItem() const {
    Q_D(const UCSlotsLayout);
    //FIXME: is this safe? We're throwing away the const qualifier of d->m_title
    return (QQuickText* const) &(d->m_title);
}

QQuickText* UCSlotsLayout::subtitleItem() const {
    Q_D(const UCSlotsLayout);
    return (QQuickText* const) &(d->m_subtitle);
}

QQuickText* UCSlotsLayout::subsubtitleItem() const {
    Q_D(const UCSlotsLayout);
    return (QQuickText* const) &(d->m_subsubtitle);
}

void UCSlotsLayout::mousePressEvent(QMouseEvent *event) {
    QQuickItem::mousePressEvent(event);

    //TODO: do we want something like UCStyledItemBase::requestFocus here?
    //requestFocus(Qt::MouseFocusReason);

    Q_D(UCSlotsLayout);

    if (!contains(event->localPos())) {
        qDebug() << "PRESS EVENT OUTSIDE OF SLOTSLAYOUT REGION, IGNORING.";
        return;
    }

    //Just iterate through all the children and see if the press falls within the area of any of them
    //TODO: do something more efficient? Using a spatial data structure maybe?
    for (int i=0; i<childItems().length(); i++) {
        QQuickItem* currChild = static_cast<QQuickItem*>(childItems().at(i));

        //skip c++ labels
        if (currChild == &d->m_title || currChild == &d->m_subtitle
                || currChild == &d->m_subsubtitle) {
            continue;
        }

        UCSlotsAttached *attachedSlot =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(currChild));

        if (!attachedSlot) {
            qDebug() << "SLOTSLAYOUT: invalid attached property!";
            continue;
        }

        //we just check X, as according to the UX spec the touch region goes top-to-bottom
        //so checking the y is not needed
        if ((event->x() >= currChild->x() - attachedSlot->leftMargin())
                && (event->x() <= currChild->x() + currChild->width() + attachedSlot->rightMargin())) {
            //child found
            event->setAccepted(true);
            d->pressedItem = currChild;
            return;
        }
    }

    //the press event didn't fall inside the touch region of any slot
    event->setAccepted(false);
}

void UCSlotsLayout::mouseReleaseEvent(QMouseEvent *event) {
    QQuickItem::mousePressEvent(event);
    Q_D(UCSlotsLayout);

    if (d->pressedItem) {
        UCSlotsAttached *attachedSlot =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(d->pressedItem));

        if (!attachedSlot) {
            qDebug() << "SLOTSLAYOUT: invalid attached property!";
        }
        //emit the signal if the release event falls inside the touch region of the pressedItem
        else if ((event->x() >= d->pressedItem->x() - attachedSlot->leftMargin())
                 && (event->x() <= d->pressedItem->x() + d->pressedItem->width() + attachedSlot->rightMargin())) {
            Q_EMIT slotClicked(d->pressedItem);
        }
    }

    d->pressedItem = 0;
}

qreal UCSlotsLayout::leftOffset() const {
    Q_D(const UCSlotsLayout);
    return d->leftOffset;
}

void UCSlotsLayout::setLeftOffset(qreal val) {
    Q_D(UCSlotsLayout);
    if (d->leftOffset != val) {
        d->leftOffset = val;
        Q_EMIT leftOffsetChanged();
    }
}

void UCSlotsLayout::setLeftOffsetQML(qreal val) {
    Q_D(UCSlotsLayout);
    d->leftOffsetWasSetFromQml = true;
    setLeftOffset(val);
}

qreal UCSlotsLayout::rightOffset() const {
    Q_D(const UCSlotsLayout);
    return d->rightOffset;
}

void UCSlotsLayout::setRightOffset(qreal val) {
    Q_D(UCSlotsLayout);
    if (d->rightOffset != val) {
        d->rightOffset = val;
        Q_EMIT rightOffsetChanged();
    }
}

void UCSlotsLayout::setRightOffsetQML(qreal val) {
    Q_D(UCSlotsLayout);
    d->rightOffsetWasSetFromQml = true;
    setRightOffset(val);
}

qreal UCSlotsLayout::topOffset() const {
    Q_D(const UCSlotsLayout);
    return d->topOffset;
}

void UCSlotsLayout::setTopOffset(qreal val) {
    Q_D(UCSlotsLayout);
    if (d->topOffset != val) {
        d->topOffset = val;
        Q_EMIT topOffsetChanged();
    }
}

void UCSlotsLayout::setTopOffsetQML(qreal val) {
    Q_D(UCSlotsLayout);
    d->topOffsetWasSetFromQml = true;
    setTopOffset(val);
}

qreal UCSlotsLayout::bottomOffset() const {
    Q_D(const UCSlotsLayout);
    return d->bottomOffset;
}

void UCSlotsLayout::setBottomOffset(qreal val) {
    Q_D(UCSlotsLayout);
    if (d->bottomOffset != val) {
        d->bottomOffset = val;
        Q_EMIT bottomOffsetChanged();
    }
}

void UCSlotsLayout::setBottomOffsetQML(qreal val) {
    Q_D(UCSlotsLayout);
    d->bottomOffsetWasSetFromQml = true;
    setBottomOffset(val);
}

bool UCSlotsLayout::progression() const {
    Q_D(const UCSlotsLayout);
    return d->progression;
}

void UCSlotsLayout::setProgression(bool val) {
    Q_D(UCSlotsLayout);
    if (d->progression != val) {
        d->progression = val;
        Q_EMIT progressionChanged();
    }
}

UCSlotsAttached* UCSlotsLayout::progressionSlot() const {
    Q_D(const UCSlotsLayout);

    if (d->chevron == 0) {
        qDebug() << "SlotsLayout: please enable the progression symbol before trying to set its properties";
        return 0;
    } else {
        UCSlotsAttached* attached = qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(d->chevron));
        //make sure QQmlEngine doesn't delete our object when doing GC
        QQmlEngine::setObjectOwnership(attached, QQmlEngine::CppOwnership);

        return (UCSlotsAttached* const) attached;
    }
}

UCSlotsLayoutChevron::UCSlotsLayoutChevron(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    updateGuValues();
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(updateGuValues()));
}

void UCSlotsLayoutChevron::paint(QPainter *painter) {
    painter->drawPixmap(0,0, width(), height(), progressionPixmap);
}

void UCSlotsLayoutChevron::updateGuValues() {
    setImplicitWidth(UCUnits::instance().gu(CHEVRON_DEFAULT_WIDTH_GU));
    setImplicitHeight(UCUnits::instance().gu(CHEVRON_DEFAULT_HEIGHT_GU));

    reloadIcon();
}

void UCSlotsLayoutChevron::reloadIcon()
{
    UnityThemeIconProvider* provider = new UnityThemeIconProvider;

    //load a new image with the correct size
    QSize loadedSize;

    //scale is so that the height is the one we want, and the width is the one which keeps the aspect ratio
    progressionPixmap = provider->requestPixmap("chevron", &loadedSize, QSize(-1, height()));

    delete provider;
    provider = 0;

    setWidth(loadedSize.width());
    setHeight(loadedSize.height());
}

#include "moc_ucslotslayout.cpp"
