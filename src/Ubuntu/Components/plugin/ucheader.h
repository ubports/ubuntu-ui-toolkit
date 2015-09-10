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

#ifndef UCHEADER_H
#define UCHEADER_H

#include "ucstyleditembase.h" // FIXMME TIM: remove
#include <QtCore/QPointer>

class QQuickFlickable;
class QQuickNumberAnimation;

class UCUbuntuAnimation;

class UCHeader : public QQuickItem //UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(QQuickFlickable* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
    Q_PROPERTY(bool exposed READ exposed WRITE setExposed NOTIFY exposedChanged)

public:
    explicit UCHeader(QQuickItem *parent = 0);

    QQuickFlickable* flickable();
    void setFlickable(QQuickFlickable* flickable);
    bool exposed();
    void setExposed(bool exposed);

Q_SIGNALS:
    void flickableChanged();
    void exposedChanged();

protected:
    void show();
    void hide();

private Q_SLOTS:
    void _q_scrolledContents();

private:
    bool m_exposed;
    qreal m_previous_contentY;

    QQuickNumberAnimation* m_showHideAnimation;
    QPointer<QQuickFlickable> m_flickable;

    // used to set the easing and duration of m_showHideAnimation
    static UCUbuntuAnimation *s_ubuntuAnimation;
};

#endif // UCHEADER_H
