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

#ifndef LABEL_P
#define LABEL_P

#include "uclabel.h"

class UCLabelPrivate
{
    Q_DECLARE_PUBLIC(UCLabel)
public:
    explicit UCLabelPrivate(UCLabel *qq);
    UCLabelPrivate(UCLabel *qq, UCLabel::ColorProviderFunc func);

    static UCLabelPrivate *get(UCLabel *q)
    {
        return q->d_func();
    }

    void init();

    // slots
    void updateRenderType();

    // methods
    void updatePixelSize();

    // members
    enum {
        TextSizeSet = 1,
        PixelSizeSet = 2,
        ColorSet = 4
    };

    UCLabel *q_ptr;
    QFont defaultFont;
    UCLabel::ColorProviderFunc defaultColor;
    UCLabel::TextSize textSize;
    quint8 flags;

protected:
    static QColor getDefaultColor(QQuickItem *item, UCTheme *theme);
};

#endif // LABEL_P

