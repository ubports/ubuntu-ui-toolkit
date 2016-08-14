/*
 * Copyright 2015-2016 Canonical Ltd.
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
 */

#include "threelabelsslot_p.h"
#include "label_p.h"
#include "ucunits_p.h"
#include "ucfontutils_p.h"
#include "uctheme_p.h"

UT_NAMESPACE_BEGIN

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

    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_onGuValueChanged()));
    _q_onGuValueChanged();
}

void UCThreeLabelsSlotPrivate::setTitleProperties()
{
    if (m_title != Q_NULLPTR) {
        //Using WrapAnywhere because ElideRight elides too early when used
        //together with WrapWord, and that produces an unexpected result.
        //This will cover most of the usecases.
        //NOTE: If you want to use ElideMiddle or ElideLeft you'll have to
        //set wrapMode to NoWrap (see Text docs for more details)
        m_title->setWrapMode(UCLabel::WrapAnywhere);
        m_title->setElideMode(UCLabel::ElideRight);
        m_title->setMaximumLineCount(1);
        m_title->setTextSize(UCLabel::Medium);
    }
}

void UCThreeLabelsSlotPrivate::setSubtitleProperties()
{
    if (m_subtitle != Q_NULLPTR) {
        m_subtitle->setWrapMode(UCLabel::WrapAnywhere);
        //NOTE: If you want to use ElideMiddle or ElideLeft you'll have to
        //set wrapMode to NoWrap (see Text docs for more details)
        m_subtitle->setElideMode(UCLabel::ElideRight);
        m_subtitle->setMaximumLineCount(1);
        m_subtitle->setTextSize(UCLabel::Small);
    }
}

void UCThreeLabelsSlotPrivate::setSummaryProperties()
{
    if (m_summary != Q_NULLPTR) {
        m_summary->setWrapMode(UCLabel::WrapAnywhere);
        //NOTE: If you want to use ElideMiddle or ElideLeft you'll have to
        //set wrapMode to NoWrap (see Text docs for more details)
        m_summary->setElideMode(UCLabel::ElideRight);
        m_summary->setMaximumLineCount(2);
        m_summary->setTextSize(UCLabel::Small);
    }
}

void UCThreeLabelsSlotPrivate::_q_onGuValueChanged()
{
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
                                : QQuickItemPrivate::get(m_title)->bottom());
        subtitleAnchors->setTopMargin(skipTitle
                                      ? 0
                                      : UCUnits::instance()->dp(TITLE_SPACING_DP));
    }

    if (!skipSummary) {
        QQuickAnchors *summaryAnchors = QQuickItemPrivate::get(m_summary)->anchors();
        summaryAnchors->setTop(skipSubtitle
                               ? (skipTitle ? top() : QQuickItemPrivate::get(m_title)->bottom())
                               : QQuickItemPrivate::get(m_subtitle)->bottom());
        summaryAnchors->setTopMargin(skipSubtitle
                                     ? (skipTitle ? 0 : UCUnits::instance()->dp(TITLE_SPACING_DP))
                                     : 0);
    }
    //Update height of the labels box
    //NOTE (FIXME? it's stuff in Qt): height is not 0 when the string is empty, its default value is "fontHeight"!
    qreal labelsBoundingBoxHeight = 0;

    //We're assuming they're positioned in a column, title->subtitle->summary
    if (!skipSummary) {
        labelsBoundingBoxHeight = m_summary->y() + m_summary->height();
    } else if (!skipSubtitle) {
        labelsBoundingBoxHeight = m_subtitle->y() + m_subtitle->height();
    } else if (!skipTitle) {
        labelsBoundingBoxHeight = m_title->y() + m_title->height();
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
        QQmlEngine::setContextForObject(d->m_title, qmlContext(this));
        UCLabelPrivate::get(d->m_title)->init();

        QQuickAnchors *titleAnchors = QQuickItemPrivate::get(d->m_title)->anchors();
        titleAnchors->setLeft(d->left());
        titleAnchors->setRight(d->right());

        //we need this to know when any of the labels is empty. In that case, we'll have to change the
        //anchors because even if a UCLabel has empty text, its height will not be 0 but "fontHeight",
        //so anchoring to text's bottom will result in the wrong outcome as a consequence.
        //FIXME: updating anchors just because text changes is too much, we should probably just
        //have variables signal when a label becomes empty
        QObject::connect(d->m_title, SIGNAL(textChanged(QString)), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        //the height may change for many reasons, for instance:
        //- change of fontsize
        //- or resizing the layout until text wrapping is triggered
        //so we have to monitor height change as well
        QObject::connect(d->m_title, SIGNAL(heightChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));
        QObject::connect(d->m_title, SIGNAL(visibleChanged()), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        //When changing fontsize of the title, heightChanged triggers an update of the height of "this", but at that point
        //baseline anchor and baselineOffset are not updated yet, so the height computed is not correct.
        //For that reason, we have to update the height of the item also when baselineOffset changes
        QObject::connect(d->m_title, SIGNAL(baselineOffsetChanged(qreal)), this, SLOT(_q_updateLabelsAnchorsAndBBoxHeight()));

        d->setTitleProperties();
        d->_q_updateLabelsAnchorsAndBBoxHeight();
    }
    return d->m_title;
}

QColor UCThreeLabelsSlot::getSubtitleColor(QQuickItem *item, UCTheme *theme)
{
    // FIXME: replace the code below with automatic color
    // change detection based on teh item's state
    const char *valueSet = item->isEnabled() ? "normal" : "disabled";
    return theme ? theme->getPaletteColor(valueSet, "backgroundSecondaryText") : QColor();
}

UCLabel *UCThreeLabelsSlot::subtitle()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_subtitle == Q_NULLPTR) {
        d->m_subtitle = new UCLabel(getSubtitleColor, this);
        QQmlEngine::setContextForObject(d->m_subtitle, qmlContext(this));
        UCLabelPrivate::get(d->m_subtitle)->init();

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

QColor UCThreeLabelsSlot::getSummaryColor(QQuickItem *item, UCTheme *theme)
{
    // FIXME: replace the code below with automatic color
    // change detection based on teh item's state
    const char *valueSet = item->isEnabled() ? "normal" : "disabled";
    return theme ? theme->getPaletteColor(valueSet, "backgroundTertiaryText") : QColor();
}

UCLabel *UCThreeLabelsSlot::summary()
{
    Q_D(UCThreeLabelsSlot);
    if (d->m_summary == Q_NULLPTR) {
        d->m_summary = new UCLabel(getSummaryColor, this);
        QQmlEngine::setContextForObject(d->m_summary, qmlContext(this));
        UCLabelPrivate::get(d->m_summary)->init();

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

UT_NAMESPACE_END
