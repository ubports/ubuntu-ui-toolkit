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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCBOTTOMEDGESTYLE_H
#define UCBOTTOMEDGESTYLE_H

#include <QtQuick/QQuickItem>
#include "ubuntutoolkitglobal.h"

class QQuickItem;
class QQuickAbstractAnimation;

namespace UbuntuToolkit {

class UCBottomEdge;
class UBUNTUTOOLKIT_EXPORT UCBottomEdgeStyle : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* panel MEMBER m_panel NOTIFY panelChanged FINAL)
    Q_PROPERTY(QQuickItem* contentItem MEMBER m_contentItem NOTIFY contentItemChanged FINAL)
    Q_PROPERTY(QQuickAbstractAnimation *panelAnimation MEMBER m_panelAnimation NOTIFY panelAnimationChanged FINAL)
    Q_PROPERTY(qreal revealThreshold MEMBER m_revealThreshold NOTIFY revealThresholdChanged FINAL)
public:
    explicit UCBottomEdgeStyle(QQuickItem *parent = 0)
        : QQuickItem(parent)
    {}

    void setConsumeMouse(bool consume);

Q_SIGNALS:
    void panelChanged();
    void contentItemChanged();
    void panelAnimationChanged();
    void revealThresholdChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QQuickItem *m_panel = nullptr;
    QQuickItem *m_contentItem = nullptr;
    QQuickAbstractAnimation *m_panelAnimation = nullptr;
    qreal m_revealThreshold = 0.;

    friend class UCBottomEdge;
    friend class UCBottomEdgePrivate;
};

UT_NAMESPACE_END

#endif // UCBOTTOMEDGESTYLE_H
