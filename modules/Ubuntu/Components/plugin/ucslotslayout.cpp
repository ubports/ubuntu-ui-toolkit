#include "ucunits.h"
#include "ucslotslayout.h"
#include "ucslotslayout_p.h"
#include "ucfontutils.h"


UCSlotsAttached::UCSlotsAttached(QObject *object)
    : QObject(object)
    , m_position(UCSlotsLayout::Trailing)
    , m_leftMargin(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTSLOTSIDEMARGINS_GU))
    , m_rightMargin(UCUnits::instance().gu(SLOTSLAYOUT_DEFAULTSLOTSIDEMARGINS_GU))
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
    //NOTE (FIXME? it's stuff in Qt): contentHeight is not 0 when the string is empty, its default value is "fontHeight"!
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
            labelsBoundingBoxHeight += m_subsubtitle.contentHeight();
        }
    } else {
        if (emptySubsubtitle) {
            labelsBoundingBoxHeight += m_subtitle.contentHeight();
        } else {
            labelsBoundingBoxHeight += m_subtitle.baselineOffset()
                    + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_SPACING)
                    + m_subsubtitle.contentHeight();
        }
    }

    _q_updateSize();
}

void UCSlotsLayoutPrivate::_q_updateSlotsBBoxHeight() {
    if (!ready) return;

    Q_Q(UCSlotsLayout);

    qreal maxSlotsHeight = 0;
    for (int i=0; i<q->children().count(); i++) {
        QQuickItem* child = qobject_cast<QQuickItem*>(q->children().at(i));
        maxSlotsHeight = qMax<int>(maxSlotsHeight, child->height());
    }
    maxChildrenHeight = maxSlotsHeight;

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
                         + UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_MARGIN)*2);

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
    int currentX = 0;
    int totalWidth = 0;

    //reorder children and assign them to the sorted list
    for (int i=0; i<q->children().length(); i++) {
        //get the attached property, creating it if it wasn't defined already
        UCSlotsAttached *attached =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(q->children().at(i)));

        if (!attached) {
            qDebug() << "SlotsLayout: Invalid attaching type!";
            continue;
        }

        //if (!position.isValid()) continue;
        //TODO: IGNORE INVISIBLE CHILDREN?

        if (attached->position() == UCSlotsLayout::Leading) {
            //FIXME: is this safe?
            leadingSlots.append(reinterpret_cast<QQuickItem*>(q->children().at(i)));
            totalWidth += q->children().at(i)->property("width").toInt() + attached->leftMargin() + attached->rightMargin();
        } else if (attached->position() == UCSlotsLayout::Trailing) {
            trailingSlots.append(reinterpret_cast<QQuickItem*>(q->children().at(i)));
            totalWidth += q->children().at(i)->property("width").toInt() + attached->leftMargin() + attached->rightMargin();
        } else {
            qDebug() << "SlotsLayout: unrecognized position value, please use SlotsLayout.Leading or SlotsLayout.Trailing";
        }
    }

    QQuickItemPrivate* _q_private = QQuickItemPrivate::get(q);

    for (int i=0; i<leadingSlots.length(); i++) {
        UCSlotsAttached *attached =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(leadingSlots.at(i)));

        QQuickItemPrivate* item = QQuickItemPrivate::get((QQuickItem*) leadingSlots.at(i));

        //FIXME: just a temporary hack to allow vertical centering of the slots
        if (!item->anchors()->verticalCenter().item) {
            item->anchors()->setTop(_q_private->top());
            item->anchors()->setTopMargin(UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_MARGIN));
        }

        if (i==0) {
            item->anchors()->setLeft(_q_private->left());
            item->anchors()->setLeftMargin(attached->leftMargin());
        } else {
            UCSlotsAttached *attachedPreviousItem =
                    qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(trailingSlots.at(i-1)));

            item->anchors()->setLeft(QQuickItemPrivate::get((QQuickItem*) leadingSlots.at(i-1))->right());
            item->anchors()->setLeftMargin(attachedPreviousItem->rightMargin() + attached->leftMargin());
        }
        currentX += item->x + item->width + attached->leftMargin() + attached->rightMargin();
    }

    int numberOfLeadingSlots = leadingSlots.length();
    int numberOfTrailingSlots = trailingSlots.length();

    QQuickAnchorLine labelsLeftAnchor = numberOfLeadingSlots ? QQuickItemPrivate::get((QQuickItem*) leadingSlots.at(numberOfLeadingSlots-1))->right()
                                                             : _q_private->left();

    QQuickAnchors* titleAnchors = QQuickItemPrivate::get(&m_title)->anchors();
    QQuickAnchors* subtitleAnchors = QQuickItemPrivate::get(&m_subtitle)->anchors();
    QQuickAnchors* subsubtitleAnchors = QQuickItemPrivate::get(&m_subsubtitle)->anchors();
    qreal labelBoxWidth = q->width() - totalWidth - UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN);

    titleAnchors->setLeft(labelsLeftAnchor);
    subtitleAnchors->setLeft(labelsLeftAnchor);
    subsubtitleAnchors->setLeft(labelsLeftAnchor);

    if (numberOfLeadingSlots != 0) {
        UCSlotsAttached *attachedLastLeadingSlot =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(leadingSlots.last()));

        titleAnchors->setLeftMargin(attachedLastLeadingSlot->rightMargin());
        subtitleAnchors->setLeftMargin(attachedLastLeadingSlot->rightMargin());
        subsubtitleAnchors->setLeftMargin(attachedLastLeadingSlot->rightMargin());
    } else {
        //DO WE WANT LEFT MARGIN FOR THE LABELS WHEN THERE ARE NO LEADING SLOTS?
    }

    //center the labels vertically
    //titleAnchors->setTopMargin((q->height() - labelsBoundingBoxHeight) / 2.0);
    //new behaviour, latest visual design
    titleAnchors->setTopMargin(UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_MARGIN));

    currentX += labelBoxWidth + UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN);

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
            QQuickItemPrivate* item = QQuickItemPrivate::get((QQuickItem*) trailingSlots.at(i));

            UCSlotsAttached *attached =
                    qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(trailingSlots.at(i)));

            if (!item->anchors()->verticalCenter().item) {
                item->anchors()->setTop(_q_private->top());
                item->anchors()->setTopMargin(UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_MARGIN));
            }

            if (i==0) {
                item->anchors()->setLeft(QQuickItemPrivate::get(&m_title)->right());
                //the 2GU right margin of the labels is treated as left margin of the first trailing slot
                //because labels don't have a right anchor set up here!
                item->anchors()->setLeftMargin(UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN) + attached->leftMargin());
            } else {
                UCSlotsAttached *attachedPreviousItem =
                        qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(trailingSlots.at(i-1)));

                item->anchors()->setLeft(QQuickItemPrivate::get((QQuickItem*) trailingSlots.at(i-1))->right());
                item->anchors()->setLeftMargin(attachedPreviousItem->rightMargin() + attached->leftMargin());
            }
            currentX += item->x + item->width + attached->leftMargin() + attached->rightMargin();
        }
    } else {
        titleAnchors->setRight(_q_private->right());
        titleAnchors->setRightMargin(UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN));
        subtitleAnchors->setRight(_q_private->right());
        subtitleAnchors->setRightMargin(UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN));
        subsubtitleAnchors->setRight(_q_private->right());
        subsubtitleAnchors->setRightMargin(UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN));
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
            d->_q_updateSize();
            d->_q_relayout();
        }
        break;
    case ItemChildRemovedChange:
        if (data.item) {
            QObject::disconnect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));
            QObject::disconnect(data.item, SIGNAL(heightChanged()), this, SLOT(_q_updateSlotsBBoxHeight()));
            QObject::disconnect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));
            d->_q_updateSize();
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

    //TODO: do something more efficient? Using a spatial data structure maybe?
    //Just iterate through all the children and see if the press falls within the area of any of them
    for (int i=0; i<children().length(); i++) {
        QQuickItem* currChild = static_cast<QQuickItem*>(children().at(i));
        UCSlotsAttached *attachedSlot =
                qobject_cast<UCSlotsAttached*>(qmlAttachedPropertiesObject<UCSlotsLayout>(children().at(i)));

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

        //emit the signal if the release event falls inside the touch region of the pressedItem
        if ((event->x() >= d->pressedItem->x() - attachedSlot->leftMargin())
                && (event->x() <= d->pressedItem->x() + d->pressedItem->width() + attachedSlot->rightMargin())) {
            Q_EMIT slotClicked(d->pressedItem);
        }
    }

    d->pressedItem = 0;
}
#include "moc_ucslotslayout.cpp"
