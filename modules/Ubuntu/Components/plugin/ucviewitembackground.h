/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef UCVIEWITEMBACKGROUND_H
#define UCVIEWITEMBACKGROUND_H

#include <QtQuick/QQuickItem>

class UCViewItem;
class UCViewItemBackground : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor pressedColor READ pressedColor WRITE setPressedColor NOTIFY pressedColorChanged)
public:
    explicit UCViewItemBackground(QQuickItem *parent = 0);
    ~UCViewItemBackground();

    QColor color() const;
    void setColor(const QColor &color);
    QColor pressedColor() const;
    void setPressedColor(const QColor &pressedColor);

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data);

Q_SIGNALS:
    void colorChanged();
    void pressedColorChanged();

private:
    UCViewItem *m_item;
    QColor m_color;
    QColor m_pressedColor;

private Q_SLOTS:
    void onPaletteUpdated();
};

#endif // UCVIEWITEMBACKGROUND_H
