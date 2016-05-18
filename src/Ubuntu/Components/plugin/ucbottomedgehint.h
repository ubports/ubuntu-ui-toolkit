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
class UCBottomEdgeHintPrivate;
class UCBottomEdgeHint : public UCActionItem
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(QQuickFlickable *flickable READ flickable WRITE setFlickable NOTIFY flickableChanged FINAL)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged FINAL)
    Q_PROPERTY(int deactivateTimeout READ deactivateTimeout WRITE setDeactivateTimeout NOTIFY deactivateTimeoutChanged FINAL)
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

    QQuickFlickable *flickable() const;
    void setFlickable(QQuickFlickable *flickable);
    Status status();
    void setStatus(Status status);
    int deactivateTimeout() const;
    void setDeactivateTimeout(int timeout);
    UCSwipeArea *swipeArea() const;

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
    UCBottomEdgeHint(UCBottomEdgeHintPrivate &&, QQuickItem *parent);
    void classBegin() override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void handleFlickableActivation();
    void onDraggingChanged(bool dragging);

    void init();
    void onMouseAttached();
    void onGridUnitChanged();

private:
    Q_DECLARE_PRIVATE(UCBottomEdgeHint)

    friend class UCBottomEdge;

    void adjustFlickableBottomMargin();
};

#endif // UCBOTTOMEDGEHINT_H
