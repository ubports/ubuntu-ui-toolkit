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

#ifndef UCVIEWITEM_P_H
#define UCVIEWITEM_P_H

#include "uclistitem.h"
#include "ucstyleditembase_p.h"
#include <QtCore/QPointer>

class QQuickFlickable;
class UCListItemBackground;
class UCListItemBasePrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCListItemBase)
public:
    UCListItemBasePrivate();
    virtual ~UCListItemBasePrivate();
    void init();

    static inline UCListItemBasePrivate *get(UCListItemBase *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    // override setFocusable()
    void setFocusable();

    void _q_rebound();
    void setPressed(bool pressed);
    void listenToRebind(bool listen);

    bool pressed:1;
    QPointer<QQuickFlickable> flickable;
    UCListItemBackground *background;
};

class UCListItemBackground : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color MEMBER m_color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor pressedColor MEMBER m_pressedColor WRITE setPressedColor NOTIFY pressedColorChanged)
public:
    explicit UCListItemBackground(QQuickItem *parent = 0);
    ~UCListItemBackground();

    void setColor(const QColor &color);
    void setPressedColor(const QColor &color);

Q_SIGNALS:
    void colorChanged();
    void pressedColorChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data);

private Q_SLOTS:
    void updateColors();

private:
    QColor m_color;
    QColor m_pressedColor;
    UCListItemBase *m_item;
    bool m_pressedColorChanged:1;
};

QColor getPaletteColor(const char *profile, const char *color);

QML_DECLARE_TYPE(UCListItemBackground)

#endif // UCVIEWITEM_P_H
