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
#include "ucitemextension.h"

class UCLabel : public QQuickText, public UCItemExtension
{
    Q_OBJECT

    Q_ENUMS(AdaptiveSize)
    Q_PROPERTY(AdaptiveSize adaptiveSize MEMBER m_adaptiveSize WRITE setAdaptiveSize NOTIFY adaptiveSizeChanged FINAL REVISION 1)

    // Deprecated.
    Q_PROPERTY(QString fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)

public:
    UCLabel(QQuickItem* parent=0);

    enum AdaptiveSize {
        XxSmall = 0,
        XSmall = 1,
        Small = 2,
        Medium = 3,
        Large = 4,
        XLarge = 5
    };

    void setAdaptiveSize(AdaptiveSize adaptiveSize);

    // Deprecated.
    QString fontSize() const
    {
        if (m_flags & AdaptiveSizeSet) {
            return "";
        }
        const char* const sizes[] =
            { "xx-small", "x-small", "small", "medium", "large", "x-large" };
        return QString(sizes[m_adaptiveSize]);
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
    Q_REVISION(1) void adaptiveSizeChanged();

    // Deprecated.
    void fontSizeChanged();

private:
    void updatePixelSize();
    Q_SLOT void _q_updateFontFlag(const QFont &font);
    Q_SLOT void _q_customColor();

    enum {
        AdaptiveSizeSet = 1,
        PixelSizeSet = 2,
        ColorSet = 4
    };

    QFont m_defaultFont;
    AdaptiveSize m_adaptiveSize;
    quint8 m_flags;

    Q_DISABLE_COPY(UCLabel)
};

QML_DECLARE_TYPE(UCLabel)

#endif  // UCLABEL_H
