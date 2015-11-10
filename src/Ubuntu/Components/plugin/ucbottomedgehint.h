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

#include "ucstyleditembase.h"
#include "gestures/ucswipearea.h"

class QQuickFlickable;
class UCBottomEdgeHint : public UCStyledItemBase
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged FINAL)
    Q_PROPERTY(QUrl iconSource MEMBER m_iconSource NOTIFY iconSourceChanged FINAL)
    Q_PROPERTY(QString iconName MEMBER m_iconName NOTIFY iconNameChanged FINAL)
    Q_PROPERTY(QQuickFlickable *flickable MEMBER m_flickable WRITE setFlickable NOTIFY flickableChanged FINAL)
    Q_PROPERTY(Status status MEMBER m_status WRITE setStatus NOTIFY statusChanged FINAL)
    Q_PROPERTY(int deactivateTimeout MEMBER m_deactivateTimeout WRITE setDeactivateTimeout NOTIFY deactivateTimeoutChanged FINAL)
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

    // deprecated
    QString state() const;
    void setState(const QString &state);
    void setDeactivateTimeout(int timeout);

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

private:
    QBasicTimer m_deactivationTimer;
    QString m_text;
    QUrl m_iconSource;
    QString m_iconName;
    UCSwipeArea *m_swipeArea;
    QQuickFlickable *m_flickable;
    int m_deactivateTimeout;
    Status m_status;
    bool m_pressed:1;
};

#endif // UCBOTTOMEDGEHINT_H
