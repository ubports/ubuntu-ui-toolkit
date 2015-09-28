#include "threelabelsslot_p.h"
#include "uclabel.h"
#include "ucunits.h"
#include "ucfontutils.h"

UCThreeLabelsSlotPrivate::UCThreeLabelsSlotPrivate()
    : QQuickItemPrivate()
    , m_title(Q_NULLPTR)
    , m_subtitle(Q_NULLPTR)
    , m_summary(Q_NULLPTR)
{
}

void UCThreeLabelsSlotPrivate::init()
{
    Q_Q(UCThreeLabelsSlot);

    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_onGuValueChanged()));
    _q_onGuValueChanged();
}

void UCThreeLabelsSlotPrivate::setTitleProperties()
{
    Q_Q(UCThreeLabelsSlot);
    if (m_title != Q_NULLPTR) {
        m_title->setWrapMode(UCLabel::WordWrap);
        m_title->setElideMode(UCLabel::ElideRight);
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
        m_subtitle->setWrapMode(UCLabel::WordWrap);
        m_subtitle->setElideMode(UCLabel::ElideRight);
        m_subtitle->setMaximumLineCount(1);
        QFont subtitleFont = m_subtitle->font();
        subtitleFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));
        subtitleFont.setWeight(QFont::Light);
        m_subtitle->setFont(subtitleFont);
    }
}

void UCThreeLabelsSlotPrivate::setSummaryProperties()
{
    Q_Q(UCThreeLabelsSlot);
    if (m_summary != Q_NULLPTR) {
        m_summary->setWrapMode(UCLabel::WordWrap);
        m_summary->setElideMode(UCLabel::ElideRight);
        m_summary->setMaximumLineCount(2);
        QFont summaryFont = m_summary->font();
        summaryFont.setPixelSize(UCFontUtils::instance().sizeToPixels("small"));
        summaryFont.setWeight(QFont::Light);
        m_summary->setFont(summaryFont);
    }
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

    //even if a UCLabel is empty it will have height as if it had one character, so we can't rely
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

UCLabel *UCThreeLabelsSlot::title()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_title == Q_NULLPTR) {
        d->m_title = new UCLabel(this);
        QQmlData::get(d->m_title, true);
        d->m_title->init();

        QQuickAnchors *titleAnchors = QQuickItemPrivate::get(d->m_title)->anchors();
        titleAnchors->setLeft(d->left());
        titleAnchors->setRight(d->right());

        //we need this to know when any of the labels is empty. In that case, we'll have to change the
        //anchors because even if a UCLabel has empty text, its height will not be 0 but "fontHeight",
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
        d->_q_updateLabelsAnchorsAndBBoxHeight();
    }
    return d->m_title;
}

UCLabel *UCThreeLabelsSlot::subtitle()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_subtitle == Q_NULLPTR) {
        d->m_subtitle = new UCLabel(this);
        QQmlData::get(d->m_subtitle, true);
        d->m_subtitle->init();

        QQuickAnchors *subtitleAnchors = QQuickItemPrivate::get(d->m_subtitle)->anchors();
        subtitleAnchors->setLeft(d->left());
        subtitleAnchors->setRight(d->right());

        QObject::connect(d->m_subtitle, SIGNAL(textChanged(QString)), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_subtitle, SIGNAL(heightChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_subtitle, SIGNAL(visibleChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        d->setSubtitleProperties();
        d->_q_updateLabelsAnchorsAndBBoxHeight();
    }
    return d->m_subtitle;
}

UCLabel *UCThreeLabelsSlot::summary()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_summary == Q_NULLPTR) {
        d->m_summary = new UCLabel(this);
        QQmlData::get(d->m_summary, true);
        d->m_summary->init();

        QQuickAnchors *summaryAnchors = QQuickItemPrivate::get(d->m_summary)->anchors();
        summaryAnchors->setLeft(d->left());
        summaryAnchors->setRight(d->right());

        QObject::connect(d->m_summary, SIGNAL(textChanged(QString)), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_summary, SIGNAL(heightChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_summary, SIGNAL(visibleChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        d->setSummaryProperties();
        d->_q_updateLabelsAnchorsAndBBoxHeight();
    }
    return d->m_summary;
}
