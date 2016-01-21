/*
 * Copyright 2016 Canonical Ltd.
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

#include "uclabel.h"
#include "ucfontutils.h"
#include "ucnamespace.h"
#include "ucunits.h"
#include "uctheme.h"

void UCLabel::updatePixelSize()
{
    if (m_flags & PixelSizeSet) {
        return;
    }
    const float sizes[] = {
        UCFontUtils::xxSmallScale, UCFontUtils::xSmallScale, UCFontUtils::smallScale,
        UCFontUtils::mediumScale, UCFontUtils::largeScale, UCFontUtils::xLargeScale
    };
    QFont textFont = font();
    textFont.setPixelSize(
        qRound(sizes[m_textSize] * UCUnits::instance().dp(UCFontUtils::fontUnits)));
    setFont(textFont);
    // remove PixelSizeSet flag
    m_flags &= ~PixelSizeSet;
}

void UCLabel::_q_updateFontFlag(const QFont &font)
{
    Q_UNUSED(font);
    if (m_defaultFont.pixelSize() != font.pixelSize()) {
        m_flags |= PixelSizeSet;
    }
}

void UCLabel::_q_customColor()
{
    m_flags |= ColorSet;
}

/*!
 * \qmltype LabelBase
 * \qmlabstract
 * \instantiates UCLabel
 * \inqmlmodule Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief Extended Text item with Ubuntu styling.
 *
 * Label is an extended Text item with Ubuntu styling. It exposes an additional property that
 * provides adaptive resizing based on the measurement unit.
 *
 * Example:
 * \qml
 * Rectangle {
 *     color: UbuntuColors.warmGrey
 *     width: units.gu(30)
 *     height: units.gu(30)
 *
 *     Label {
 *         anchors.centerIn: parent
 *         text: "Hello world!"
 *         textSize: Label.Large
 *     }
 * }
 * \endqml
 */
UCLabel::UCLabel(QQuickItem* parent)
    : QQuickText(parent)
    , UCThemingExtension(this)
    , m_textSize(Medium)
    , m_flags(0)
{
}

void UCLabel::classBegin()
{
    QQuickText::classBegin();
    init();
}

void UCLabel::init()
{
    postThemeChanged();
    updatePixelSize();
    m_defaultFont = font();
    m_defaultFont.setFamily("Ubuntu");
    m_defaultFont.setWeight(QFont::Light);
    setFont(m_defaultFont);

    connect(this, &UCLabel::fontChanged, this, &UCLabel::_q_updateFontFlag, Qt::DirectConnection);
    connect(this, &UCLabel::colorChanged, this, &UCLabel::_q_customColor, Qt::DirectConnection);
}

void UCLabel::postThemeChanged()
{
    if (m_flags & ColorSet) {
        return;
    }
    UCTheme *theme = getTheme();
    if (theme) {
        setColor(theme->getPaletteColor("normal", "baseText"));
        m_flags &= ~ColorSet;
    }
}

/*!
 * \qmlproperty enumeration LabelBase::textSize
 * \since Ubuntu.Components 1.3
 *
 * This property holds an abstract size that allows adaptive resizing based on the measurement unit
 * (see Units). The default value is \c Label.Medium.
 *
 * \note Setting this disables support for the deprecated \l fontSize property.
 *
 * \list
 *  \li \b Label.XxSmall - extremely small font size
 *  \li \b Label.XSmall - very small font size
 *  \li \b Label.Small - small font size
 *  \li \b Label.Medium - medium font size
 *  \li \b Label.Large - large font size
 *  \li \b Label.XLarge - very large font size
 *  \endlist
 */
void UCLabel::setTextSize(TextSize size)
{
    if (!(m_flags & TextSizeSet)) {
        Q_EMIT fontSizeChanged();
        m_flags |= TextSizeSet;
    }

    if (m_textSize != size) {
        m_textSize = size;
        updatePixelSize();
        Q_EMIT textSizeChanged();
    }
}

/*!
 * \qmlproperty string LabelBase::fontSize
 * \deprecated
 *
 * This property holds an abstract size represented as a string that allows adaptive resizing based
 * on the measurement unit (see Units). The default value is \c "medium".
 *
 * \note Use \l textSize instead.
 *
 * Here is the list of allowed strings from the smallest to the largest:
 * \list
 *  \li \b "xx-small" - extremely small font size
 *  \li \b "x-small" - very small font size
 *  \li \b "small" - small font size
 *  \li \b "medium" - medium font size
 *  \li \b "large" - large font size
 *  \li \b "x-large" - very large font size
 *  \endlist
 */
void UCLabel::setFontSize(const QString& fontSize)
{
    if (m_flags & TextSizeSet) {
        return;
    }
    if (fontSize.size() < 4) {
        return;
    }

    UC_QML_DEPRECATION_WARNING("'fontSize' is deprecated, use 'textSize' property instead.");

    TextSize textSize;
    switch (SCALE_CODE(fontSize)) {
        case SCALE_MEDIUM: { textSize = Medium; break; }
        case SCALE_LARGE: { textSize = Large; break; }
        case SCALE_SMALL: { textSize = Small; break; }
        case SCALE_XLARGE: { textSize = XLarge; break; }
        case SCALE_XSMALL: { textSize = XSmall; break; }
        case SCALE_XXSMALL: { textSize = XxSmall; break; }
        default: { return; }
    }

    if (m_textSize != textSize) {
        m_textSize = textSize;
        updatePixelSize();
        Q_EMIT fontSizeChanged();
    }
}
