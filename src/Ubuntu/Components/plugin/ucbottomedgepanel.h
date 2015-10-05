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

#ifndef UCBOTTOMEDGEPANEL_H
#define UCBOTTOMEDGEPANEL_H

#include <QtCore/QObject>
#include <QtQml/QQmlIncubator>
#include "ucbottomedgeproxy.h"

class UCBottomEdgeProxy;
class UCBottomEdgePanel : public QObject, public QQmlIncubator
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<UCBottomEdgeProxy> content READ content NOTIFY contentChanged)
public:

    static UCBottomEdgePanel &instance()
    {
        static UCBottomEdgePanel instance;
        return instance;
    }
    void initialize(QQmlEngine *engine);

    QQmlListProperty<UCBottomEdgeProxy> content();
    QQmlListProperty<QPointF> scenePoints();

    int addBottomEdge(UCBottomEdgeProxy *data, int column = -1);
    void removeBottomEdge(UCBottomEdgeProxy *data);

Q_SIGNALS:
    void contentChanged();

protected:
    explicit UCBottomEdgePanel(QObject *parent = 0);

    Q_SLOT void componentStatusChanged(QQmlComponent::Status status);
    // QQmlIncubator funcs
    void statusChanged(Status status);
    void setInitialState(QObject *panel);

    // members
    QList<UCBottomEdgeProxy*> m_list;
    QQmlComponent *m_panelComponent;
    QQmlEngine *m_engine;
    QQmlContext *m_context;
    QQuickItem *m_panel;
};

#endif // UCBOTTOMEDGEPANEL_H
