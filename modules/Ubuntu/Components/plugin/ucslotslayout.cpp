#include "ucunits.h"
#include "ucslotslayout.h"
#include "ucslotslayout_p.h"
#include "ucfontutils.h"

/******************************************************************************
 * ListItemPrivate
 */
UCSlotsLayoutPrivate::UCSlotsLayoutPrivate()
    : QQuickItemPrivate()
    , ready(false)
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
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSize()));

    //fixme: this will cause relayout to be called 4-5 times when the layout has "anchors.fill: parent"
    //defined on QML side
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(_q_relayout()));

    //we need this to know when any of the labels is empty. In that case, we'll have to change the
    //anchors because even if a QQuickText has empty text, its height will not be 0 but "fontHeight",
    //so anchoring to text's bottom will result in the wrong outcome as a consequence.
    //TODO: updating anchors just because text changes is too much, we should probably just
    //have variables signal when a label becomes empty
    QObject::connect(&m_title, SIGNAL(textChanged(QString)), q, SLOT(_q_updateLabelsAnchors()));
    QObject::connect(&m_subtitle, SIGNAL(textChanged(QString)), q, SLOT(_q_updateLabelsAnchors()));
    QObject::connect(&m_subsubtitle, SIGNAL(textChanged(QString)), q, SLOT(_q_updateLabelsAnchors()));
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

void UCSlotsLayoutPrivate::_q_updateLabelsAnchors() {
    //if the component is not ready the QML properties may not have been evaluated yet,
    //it's not worth doing anything if that's the case
    if (!ready) return;

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
}

// called when units size changes
void UCSlotsLayoutPrivate::_q_updateSize()
{
    if (!ready) {
        //qDebug() << "Skipping updatesize, component is not ready";
        return;
    }

    Q_Q(UCSlotsLayout);
    QQuickItem *parent = qobject_cast<QQuickItem*>(q->parentItem());
    q->setImplicitWidth(parent ? parent->width() : UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_WIDTH_GU));

    qreal height = 0;
    for (int i=0; i<q->children().count(); i++) {
        QQuickItem* child = qobject_cast<QQuickItem*>(q->children().at(i));
        height = qMax<int>(height, child->childrenRect().height());
    }

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

    q->setImplicitHeight(qMax<qreal>(height, labelsBoundingBoxHeight)
                         + UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_MARGIN)*2);
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

    if (q->width() <= 0 || q->height() <= 0 || q->implicitWidth() <= 0 || q->implicitHeight() <= 0
            || !q->isVisible() || !q->opacity()) {
        //skip relayout if the item will be invisible anyway, this helps at initizialization time
        //as well, when using "anchors.fill: parent" the size will vary a few times before stabilizing.
        //with this check we skip a few of those instable relayouts
        return;
    }

    QList<QQuickItem*> leadingSlots;
    QList<QQuickItem*> trailingSlots;
    int currentX = 0;
    int totalWidth = 0;
    QVariant position;
    //reorder children and assign them to the sorted list
    for (int i=0; i<q->children().length(); i++) {
        //only consider items having the "position" property, we don't want to enforce using SlotQML types
        //but at the same time we want to avoid random items (such as the style)
        position = q->children().at(i)->property("position");
        if (!position.isValid()) continue;

        //TODO: IGNORE INVISIBLE CHILDREN?

        if (!position.toString().compare("Slot.Leading")) {
            //FIXME: is this safe?
            leadingSlots.append(reinterpret_cast<QQuickItem*>(q->children().at(i)));
            totalWidth += q->children().at(i)->property("width").toInt();
        } else if (!q->children().at(i)->property("position").toString().compare("Slot.Trailing")) {
            trailingSlots.append(reinterpret_cast<QQuickItem*>(q->children().at(i)));
            totalWidth += q->children().at(i)->property("width").toInt();
        } else {
            qDebug() << "SlotsLayout: unrecognized position value, please use Slot.Leading or Slot.Trailing";
        }
    }

    for (int i=0; i<leadingSlots.length(); i++) {
        leadingSlots.at(i)->setX(currentX);
        currentX += leadingSlots.at(i)->width();
    }

    qreal labelBoxWidthIncludingMargins = q->width() - totalWidth;
    m_title.setWidth(labelBoxWidthIncludingMargins - UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN));
    m_title.setX(currentX);
    m_subtitle.setWidth(labelBoxWidthIncludingMargins - UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN));
    m_subtitle.setX(currentX);
    m_subsubtitle.setWidth(labelBoxWidthIncludingMargins - UCUnits::instance().gu(SLOTSLAYOUT_LABELS_RIGHTMARGIN));
    m_subsubtitle.setX(currentX);

    QQuickAnchors* titleAnchors = QQuickItemPrivate::get(&m_title)->anchors();
    //center the labels vertically
    //titleAnchors->setTopMargin((q->height() - labelsBoundingBoxHeight) / 2.0);
    //new behaviour, latest visual design
    titleAnchors->setTopMargin(UCUnits::instance().gu(IMPLICIT_SLOTSLAYOUT_MARGIN));

    currentX += labelBoxWidthIncludingMargins;

    for (int i=0; i<trailingSlots.length(); i++) {
        trailingSlots.at(i)->setX(currentX);
        currentX += trailingSlots.at(i)->width();
    }
}

UCSlotsLayout::UCSlotsLayout(QQuickItem *parent) :
    QQuickItem(*(new UCSlotsLayoutPrivate), parent)
{
    setFlag(ItemHasContents);
    Q_D(UCSlotsLayout);
    d->init();
}

void UCSlotsLayout::componentComplete() {
    QQuickItem::componentComplete();

    Q_D(UCSlotsLayout);
    d->ready = true;

    //We want to call this functions for the first time after the
    //QML properties (such as titleItem.text) have been initialized!
    d->_q_updateLabelsAnchors();
    d->_q_updateSize();
    d->_q_relayout();
}

void UCSlotsLayout::itemChange(ItemChange change, const ItemChangeData &data)
{    

    Q_D(UCSlotsLayout);

    switch (change) {
    case ItemChildAddedChange:
        if (data.item) {
            QObject::connect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));

            //we connect to the width change of the Slot, but for the height we use its childrenRect.height,
            //because:
            //- Slot component binds to OUR height, so binding on that would cause a loop
            //- we actually care about the height of the component which is inside the Slot, not Slot's height.
            //  In the layout process we'll then use max(componentsHeight)+margin to compute OUR height
            //FIXME: We have to connect to childrenRectChanged as there's no way to connect to childrenRect Height change only
            //but the signal fired when childrenRect's WIDTH changes is a problem for us, because it calls relayout but
            //at that point data.item.width (which is what we use in the relayout) may not be updated using the new childrenrect yet
            QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));
            QObject::connect(data.item, SIGNAL(childrenRectChanged(QRectF)), this, SLOT(_q_relayout()));
            d->_q_updateSize();
            d->_q_relayout();
        }
        break;
    case ItemChildRemovedChange:
        if (data.item) {
            QObject::disconnect(data.item, SIGNAL(visibleChanged()), this, SLOT(_q_relayout()));
            QObject::disconnect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_relayout()));
            QObject::connect(data.item, SIGNAL(childrenRectChanged(QRectF)), this, SLOT(_q_relayout()));
            d->_q_updateSize();
            d->_q_relayout();
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

            d->m_parentItem = data.item;
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

#include "moc_ucslotslayout.cpp"
