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

#include "label_p.h"
#include "ucfontutils.h"
#include "ucunits.h"
#include "uctheme.h"
#include <quickutils.h>

UT_NAMESPACE_BEGIN

UCLabelPrivate::UCLabelPrivate(UCLabel *qq)
    : q_ptr(qq)
    , defaultColor(getDefaultColor)
    , textSize(UCLabel::Medium)
    , flags(0)
{
}

UCLabelPrivate::UCLabelPrivate(UCLabel *qq, UCLabel::ColorProviderFunc func)
    : q_ptr(qq)
    , defaultColor(func)
    , textSize(UCLabel::Medium)
    , flags(0)
{
}

void UCLabelPrivate::updatePixelSize()
{
    if (flags & PixelSizeSet) {
        return;
    }

    Q_Q(UCLabel);
    const float sizes[] = {
        UCFontUtils::xxSmallScale, UCFontUtils::xSmallScale, UCFontUtils::smallScale,
        UCFontUtils::mediumScale, UCFontUtils::largeScale, UCFontUtils::xLargeScale
    };
    QFont textFont = q->font();
    textFont.setPixelSize(
        qRound(sizes[textSize] * UCUnits::instance()->dp(UCFontUtils::fontUnits)));
    q->setFont(textFont);
}

void UCLabelPrivate::updateRenderType()
{
    Q_Q(UCLabel);
    QQuickText *qtext = static_cast<QQuickText*>(q);
    if (UCUnits::instance()->gridUnit() <= 10) {
        qtext->setRenderType(QQuickText::NativeRendering);
    } else {
        qtext->setRenderType(QQuickText::QtRendering);
    }
}

/*!
 * \qmltype Label
 * \qmlabstract
 * \instantiates UbuntuToolkit::UCLabel
 * \inherits Text
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
    , d_ptr(new UCLabelPrivate(this))
{
}

UCLabel::UCLabel(ColorProviderFunc defaultColor, QQuickItem *parent)
    : QQuickText(parent)
    , UCThemingExtension(this)
    , d_ptr(new UCLabelPrivate(this, defaultColor))
{
}
UCLabel::~UCLabel()
{
    // disconnect functor, so QQuickItem's enabledChanged won't call into the invalid functor
    disconnect(this, &UCLabel::enabledChanged, this, &UCLabel::postThemeChanged);
}

QColor UCLabelPrivate::getDefaultColor(QQuickItem *item, UCTheme *theme)
{
    // FIXME: replace the code below with automatic color
    // change detection based on the item's state
    const char *valueSet = item->isEnabled() ? "normal" : "disabled";
    return theme ? theme->getPaletteColor(valueSet, "backgroundText") : QColor();
}

void UCLabel::classBegin()
{
    Q_D(UCLabel);
    QQuickText::classBegin();
    d->init();
}

void UCLabelPrivate::init()
{
    Q_Q(UCLabel);
    q->postThemeChanged();

    updatePixelSize();
    QFont defaultFont = q->font();
    defaultFont.setFamily("Ubuntu");
    defaultFont.setWeight(QFont::Light);
    q->setFont(defaultFont);
    updateRenderType();

    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(updateRenderType()));
    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(updatePixelSize()));

    QObject::connect(q, &UCLabel::enabledChanged, q, &UCLabel::postThemeChanged, Qt::DirectConnection);

    QObject::connect(q, &UCLabel::fontChanged, q, &UCLabel::fontChanged2, Qt::DirectConnection);
    QObject::connect(q, &UCLabel::colorChanged, q, &UCLabel::colorChanged2, Qt::DirectConnection);
}

void UCLabel::postThemeChanged()
{
    Q_D(UCLabel);
    if (d->flags & UCLabelPrivate::ColorSet) {
        return;
    }
    UCTheme *theme = getTheme();
    if (theme) {
        setColor(d->defaultColor(this, theme));
    }
}

/*!
 * \qmlproperty enumeration Label::textSize
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
UCLabel::TextSize UCLabel::textSize() const
{
    Q_D(const UCLabel);
    return d->textSize;
}
void UCLabel::setTextSize(TextSize size)
{
    Q_D(UCLabel);
    if (!(d->flags & UCLabelPrivate::TextSizeSet)) {
        Q_EMIT fontSizeChanged();
        d->flags |= UCLabelPrivate::TextSizeSet;
    }

    if (d->textSize != size) {
        d->textSize = size;
        d->updatePixelSize();
        Q_EMIT textSizeChanged();
    }
}

void UCLabel::setFont2(const QFont &font)
{
    Q_D(UCLabel);
    // we must restrict ourself to the pixelSize change as any font property change will
    // lead to the setter call.
    if (this->font().pixelSize() != font.pixelSize()) {
        d->flags |= UCLabelPrivate::PixelSizeSet;
    }
    QQuickText::setFont(font);
}

void UCLabel::setColor2(const QColor &color)
{
    Q_D(UCLabel);
    d->flags |= UCLabelPrivate::ColorSet;
    QQuickText::setColor(color);
}

void UCLabel::setRenderType(RenderType renderType)
{
    disconnect(UCUnits::instance(), SIGNAL(gridUnitChanged()),
               this, SLOT(updateRenderType()));
    QQuickText::setRenderType(renderType);
}

/*!
 * \qmlproperty string Label::fontSize
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
QString UCLabel::fontSize() const
{
    Q_D(const UCLabel);
    if (d->flags & UCLabelPrivate::TextSizeSet) {
        return "";
    }
    const char* const sizes[] =
        { "xx-small", "x-small", "small", "medium", "large", "x-large" };
    return QString(sizes[d->textSize]);
}
void UCLabel::setFontSize(const QString& fontSize)
{
    Q_D(UCLabel);
    if (d->flags & UCLabelPrivate::TextSizeSet) {
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

    if (d->textSize != textSize) {
        d->textSize = textSize;
        d->updatePixelSize();
        Q_EMIT fontSizeChanged();
    }
}

UT_NAMESPACE_END

#include "moc_uclabel.cpp"
