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

#ifndef UCBOTTOMEDGEREGION_H
#define UCBOTTOMEDGEREGION_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include <QtCore/QUrl>
#include <QtCore/QPointer>

class UCBottomEdge;
class QQmlComponent;
class PropertyChange;
class UCBottomEdgeRegion : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled MEMBER m_enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(qreal from MEMBER m_from WRITE setFrom NOTIFY fromChanged FINAL)
    Q_PROPERTY(qreal to MEMBER m_to WRITE setTo NOTIFY toChanged FINAL)
    Q_PROPERTY(QUrl contentUrl MEMBER m_url NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent* contentComponent MEMBER m_component NOTIFY contentComponentChanged FINAL)
public:
    explicit UCBottomEdgeRegion(QObject *parent = 0);
    void attachToBottomEdge(UCBottomEdge *bottomEdge);

    // used internally
    void setFrom(qreal from);
    void setTo(qreal to);
    void setEnabled(bool enabled);
    bool contains(qreal dragRatio);
    void enter();
    void exit();
    const QRectF rect(const QRectF &bottomEdgeRect);

Q_SIGNALS:
    void enabledChanged();
    void fromChanged();
    void toChanged();
    void contentChanged();
    void contentComponentChanged();

    void entered();
    void exited();
    void dragEnded();

protected:
    QUrl m_url;
    QPointer<UCBottomEdge> m_bottomEdge;
    QQmlComponent *m_component;
    PropertyChange *m_urlBackup;
    PropertyChange *m_componentBackup;
    qreal m_from;
    qreal m_to;
    bool m_enabled:1;

    friend class UCBottomEdge;
    friend class UCBottomEdgePrivate;
    friend class tst_BottomEdge;
};

#endif // UCBOTTOMEDGEREGION_H
