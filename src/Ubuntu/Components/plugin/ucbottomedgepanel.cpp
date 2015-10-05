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

#include "ucbottomedgepanel.h"
#include "plugin.h"
#include "quickutils.h"

#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlListProperty>
#include <QtQuick/private/qquickitem_p.h>

UCBottomEdgePanel::UCBottomEdgePanel(QObject *parent)
    : QObject(parent)
    , m_panelComponent(Q_NULLPTR)
    , m_engine(Q_NULLPTR)
    , m_context(Q_NULLPTR)
    , m_panel(Q_NULLPTR)
{
}

void UCBottomEdgePanel::initialize(QQmlEngine *engine)
{
    if (m_panelComponent || m_panel) {
        return;
    }
    m_engine = engine;
    // FIXME: perhaps we shoudl load this from the theme
    const QString relativeUrl = "1.3/BottomEdgePanel.qml";
    const QUrl url(UbuntuComponentsPlugin::pluginUrl().resolved(relativeUrl));
    m_panelComponent = new QQmlComponent(m_engine, url, QQmlComponent::Asynchronous, this);
    if (m_panelComponent->status() == QQmlComponent::Ready) {
        componentStatusChanged(QQmlComponent::Ready);
    } else {
        connect(m_panelComponent, SIGNAL(statusChanged(QQmlComponent::Status)),
                this, SLOT(componentStatusChanged(QQmlComponent::Status)));
    }
}

void UCBottomEdgePanel::componentStatusChanged(QQmlComponent::Status status)
{
    if (status == QQmlComponent::Error) {
        qWarning() << m_panelComponent->errorString();
        delete m_panelComponent;
        m_panelComponent = Q_NULLPTR;
        return;
    }
    if (status == QQmlComponent::Ready) {
        // create the panel asynchronously
        m_context = new QQmlContext(m_engine->rootContext());
        m_panelComponent->create(*this, m_context);
    }
}

// incubator
void UCBottomEdgePanel::setInitialState(QObject *panel)
{
    // export this object to panel as styledItem
    m_context->setContextProperty("styledItem", this);
    // the parent of the panel is the root object
    QQuickItem *parentItem = QuickUtils::instance().rootObject();
    static_cast<QQuickItem*>(panel)->setParentItem(parentItem);
}

void UCBottomEdgePanel::statusChanged(QQmlIncubator::Status status)
{
    switch (status) {
    case QQmlIncubator::Error:
    {
        // TODO: compile erorr string and display it
        qWarning() << "Panel loading error";
        delete m_panelComponent;
        m_panelComponent = Q_NULLPTR;
        break;
    }
    case QQmlIncubator::Ready:
    {
        m_panel = static_cast<QQuickItem*>(object());
        delete m_panelComponent;
        m_panelComponent = Q_NULLPTR;
        break;
    }
    default: break;
    }
}

/*
 * List of bottom edges.
 */
QQmlListProperty<UCBottomEdgeProxy> UCBottomEdgePanel::content()
{
    return QQmlListProperty<UCBottomEdgeProxy>(this, m_list);
}

/*
 * Adds a BottomEdge component to the panel. Return -1 on error. If \c column is
 * -1, the component will be appended. If the column points to a valid index,
 * it will replace the data for that column.
 */
int UCBottomEdgePanel::addBottomEdge(UCBottomEdgeProxy *data, int column)
{
    if (m_list.contains(data)) {
        return m_list.indexOf(data);
    }
    if (column < 0) {
        m_list.append(data);
        column = m_list.count() - 1;
    } else if (column < m_list.count()) {
        m_list[column] = data;
    } else {
        column = -1;
    }
    Q_EMIT contentChanged();
    return column;
}

void UCBottomEdgePanel::removeBottomEdge(UCBottomEdgeProxy *data)
{
    int index = m_list.indexOf(data);
    if (index < 0) {
        return;
    }
    m_list.removeAt(index);
    Q_EMIT contentChanged();
}
