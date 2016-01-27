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

#include "ucstyleditembase.h"
#include <QtCore/QPointer>

class QQuickFlickable;
class QQuickNumberAnimation;
class UCUbuntuAnimation;
class PropertyChange;

class UCHeader : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(QQuickFlickable* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged FINAL)
    Q_PROPERTY(bool exposed MEMBER m_exposed WRITE setExposed NOTIFY exposedChanged FINAL)
    Q_PROPERTY(bool moving READ moving NOTIFY movingChanged FINAL)

public:
    explicit UCHeader(QQuickItem *parent = 0);
    ~UCHeader();

    QQuickFlickable* flickable();
    void setFlickable(QQuickFlickable* flickable);
    void setExposed(bool exposed);
    bool moving();

Q_SIGNALS:
    void flickableChanged();
    void exposedChanged();
    void movingChanged();

protected:
    void show(bool animate = true);
    void hide(bool animate = true);

private Q_SLOTS:
    void _q_scrolledContents();
    void _q_showHideAnimationRunningChanged();
    void _q_flickableMovementEnded();
    void _q_contentHeightChanged();
    void _q_flickableInteractiveChanged();
    void _q_heightChanged();

private:
    QPointer<QQuickFlickable> m_flickable;
    QQuickNumberAnimation* m_showHideAnimation;
    PropertyChange* m_flickableTopMarginBackup;

    qreal m_previous_contentY;
    bool m_exposed:1;
    bool m_moving:1;

    // used to set the easing and duration of m_showHideAnimation
    static UCUbuntuAnimation *s_ubuntuAnimation;

    void updateFlickableMargins();
};

#endif // UCHEADER_H
