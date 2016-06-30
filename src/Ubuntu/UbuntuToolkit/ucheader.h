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

#ifndef UCHEADER_H
#define UCHEADER_H

#include "ucstyleditembase.h"
#include <QtCore/QPointer>

class QQuickFlickable;
class QQuickNumberAnimation;

namespace UbuntuToolkit {

class UCUbuntuAnimation;
class UBUNTUTOOLKIT_EXPORT UCHeader : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(QQuickFlickable* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged FINAL)
    Q_PROPERTY(bool exposed MEMBER m_exposed WRITE setExposed NOTIFY exposedChanged FINAL)
    Q_PROPERTY(bool moving READ moving NOTIFY movingChanged FINAL)
    Q_PROPERTY(bool automaticHeight MEMBER m_automaticHeight NOTIFY automaticHeightChanged FINAL)

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
    void automaticHeightChanged();

protected:
    virtual void show(bool animate);
    virtual void hide(bool animate);
    void itemChange(ItemChange change, const ItemChangeData &value) override;

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

    qreal m_previous_contentY;
    qreal m_previous_header_height;
    bool m_exposed:1;
    bool m_moving:1;
    bool m_automaticHeight:1;

    // used to set the easing and duration of m_showHideAnimation
    static UCUbuntuAnimation *s_ubuntuAnimation;

    void updateFlickableMargins();
};

UT_NAMESPACE_END

#endif // UCHEADER_H
