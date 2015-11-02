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

#ifndef UCBOTTOMEDGESECTION_H
#define UCBOTTOMEDGESECTION_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include <QtCore/QUrl>
#include <QtCore/QPointer>

class UCBottomEdge;
class QQmlComponent;
class PropertyChange;
class UCBottomEdgeSection : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(qreal startsAt MEMBER m_startsAt NOTIFY startsAtChanged FINAL)
    Q_PROPERTY(qreal endsAt MEMBER m_endsAt NOTIFY endsAtChanged FINAL)
    Q_PROPERTY(QUrl content MEMBER m_url NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent* contentComponent MEMBER m_component NOTIFY contentComponentChanged FINAL)
public:
    explicit UCBottomEdgeSection(QObject *parent = 0);

    // used internally
    bool dragInSection(qreal dragRatio);
    void enterSection();
    void exitSection();

Q_SIGNALS:
    void enabledChanged();
    void commitWhenDragEndsChanged();
    void startsAtChanged();
    void endsAtChanged();
    void contentChanged();
    void contentComponentChanged();

    void entered();
    void exited();
    void dragEnded();

protected:
    void classBegin(){}
    void componentComplete(){}
    void onDragEnded();

    QUrl m_url;
    QPointer<UCBottomEdge> m_bottomEdge;
    QQmlComponent *m_component;
    PropertyChange *m_urlBackup;
    PropertyChange *m_componentBackup;
    qreal m_startsAt;
    qreal m_endsAt;
    bool m_enabled:1;

    friend class UCBottomEdgePrivate;
};

#endif // UCBOTTOMEDGESECTION_H
