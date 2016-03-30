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

#ifndef UCLABEL_H
#define UCLABEL_H

#include <QtQuick/private/qquicktext_p.h>
#include "ucthemingextension.h"
// C++ std lib for std::function declaration
#include <functional>

class UCLabelPrivate;
class UCLabel : public QQuickText, public UCThemingExtension
{
    Q_OBJECT
    Q_INTERFACES(UCThemingExtension)
    Q_ENUMS(TextSize)
    Q_PROPERTY(TextSize textSize READ textSize WRITE setTextSize NOTIFY textSizeChanged FINAL)

    // Overriden from QQuickText
    Q_PROPERTY(RenderType renderType READ renderType WRITE setRenderType)
    Q_PROPERTY(QFont font READ font WRITE setFont2 NOTIFY fontChanged2)
    Q_PROPERTY(QColor color READ color WRITE setColor2 NOTIFY colorChanged2)

    // Deprecated.
    Q_PROPERTY(QString fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)

public:

    typedef std::function<QColor (QQuickItem*, UCTheme*)> ColorProviderFunc;

    UCLabel(QQuickItem* parent=0);
    // custom constructor to create the label with a different default color provider
    UCLabel(ColorProviderFunc defaultColor, QQuickItem *parent = 0);
    ~UCLabel();

    enum TextSize {
        XxSmall = 0,
        XSmall = 1,
        Small = 2,
        Medium = 3,
        Large = 4,
        XLarge = 5
    };

    TextSize textSize() const;
    void setTextSize(TextSize size);

    // overriden from QQuickText
    void setFont2(const QFont &font);
    void setColor2(const QColor &color);
    void setRenderType(RenderType renderType);

    // Deprecated.
    QString fontSize() const;
    void setFontSize(const QString& fontSize);

protected:
    // from QQuickItem
    void classBegin();

    // from UCItemExtension
    void preThemeChanged(){}
    void postThemeChanged();

Q_SIGNALS:
    void textSizeChanged();

    // overrides
    void fontChanged2();
    void colorChanged2();

    // Deprecated.
    void fontSizeChanged();

private:
    QScopedPointer<UCLabelPrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr.data(), UCLabel)
    Q_DISABLE_COPY(UCLabel)
    Q_PRIVATE_SLOT(d_func(), void updateRenderType())
};

QML_DECLARE_TYPE(UCLabel)

#endif  // UCLABEL_H
