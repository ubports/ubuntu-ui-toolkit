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
 */

#ifndef UCLABEL_H
#define UCLABEL_H

#include <QtQuick/private/qquicktext_p.h>
#include "ucthemingextension.h"

class UCLabel : public QQuickText, public UCThemingExtension
{
    Q_OBJECT

    Q_ENUMS(TextSize)
    Q_PROPERTY(TextSize textSize MEMBER m_textSize WRITE setTextSize NOTIFY textSizeChanged FINAL)

    // Deprecated.
    Q_PROPERTY(QString fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)

public:
    UCLabel(QQuickItem* parent=0);
    //QQuickTextPrivate is not exported as of 5.4.1 so we need the init here
    void init();

    enum TextSize {
        XxSmall = 0,
        XSmall = 1,
        Small = 2,
        Medium = 3,
        Large = 4,
        XLarge = 5
    };

    void setTextSize(TextSize size);

    // Deprecated.
    QString fontSize() const
    {
        if (m_flags & TextSizeSet) {
            return "";
        }
        const char* const sizes[] =
            { "xx-small", "x-small", "small", "medium", "large", "x-large" };
        return QString(sizes[m_textSize]);
    }
    void setFontSize(const QString& fontSize);

protected:
    // from QQuickItem
    void classBegin();
    void customEvent(QEvent *event);

    // from UCItemExtension
    void preThemeChanged(){}
    void postThemeChanged();

Q_SIGNALS:
    void textSizeChanged();

    // Deprecated.
    void fontSizeChanged();

private:
    void updatePixelSize();
    Q_SLOT void _q_updateFontFlag(const QFont &font);
    Q_SLOT void _q_customColor();

    enum {
        TextSizeSet = 1,
        PixelSizeSet = 2,
        ColorSet = 4
    };

    QFont m_defaultFont;
    TextSize m_textSize;
    quint8 m_flags;

    Q_DISABLE_COPY(UCLabel)
};

QML_DECLARE_TYPE(UCLabel)

#endif  // UCLABEL_H
