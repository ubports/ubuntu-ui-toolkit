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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCBOTTOMEDGEHINT_H
#define UCBOTTOMEDGEHINT_H

#include "ucactionitem.h"

class QQuickFlickable;
class UCSwipeArea;
class PropertyChange;
class UCBottomEdgeHint : public UCActionItem
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(QQuickFlickable *flickable MEMBER m_flickable WRITE setFlickable NOTIFY flickableChanged FINAL)
    Q_PROPERTY(Status status MEMBER m_status WRITE setStatus NOTIFY statusChanged FINAL)
    Q_PROPERTY(int deactivateTimeout MEMBER m_deactivateTimeout WRITE setDeactivateTimeout NOTIFY deactivateTimeoutChanged FINAL)
    Q_PROPERTY(UCSwipeArea* swipeArea READ swipeArea CONSTANT FINAL)
    // deprecated
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
public:
    enum Status {
        Hidden,
        Inactive,
        Active,
        Locked
    };
    explicit UCBottomEdgeHint(QQuickItem *parent = 0);

    void setFlickable(QQuickFlickable *flickable);
    Status status();
    void setStatus(Status status);
    void setDeactivateTimeout(int timeout);
    UCSwipeArea *swipeArea() const
    {
        return m_swipeArea;
    }

    // deprecated
    QString state() const;
    void setState(const QString &state);

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void iconNameChanged();
    void flickableChanged();
    void statusChanged();
    void deactivateTimeoutChanged();

    void clicked();

    // deprecated
    void stateChanged();
protected:
    void classBegin();
    void itemChange(ItemChange change, const ItemChangeData &data);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void handleFlickableActivation();
    void onDraggingChanged(bool dragging);

    void init();
    void onMouseAttached();
    void onGridUnitChanged();

private:
    QBasicTimer m_deactivationTimer;
    UCSwipeArea *m_swipeArea;
    QQuickFlickable *m_flickable;
    PropertyChange *m_flickableBottomMargin = nullptr;
    int m_deactivateTimeout;
    Status m_status;
    bool m_pressed:1;

    friend class UCBottomEdge;

    void adjustFlickableBottomMargin();
};

#endif // UCBOTTOMEDGEHINT_H
