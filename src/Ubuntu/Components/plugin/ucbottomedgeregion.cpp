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

#include "ucbottomedge.h"
#include "ucbottomedge_p.h"
#include "ucbottomedgeregion.h"
#include "propertychange_p.h"
#include <QtQml/private/qqmlproperty_p.h>

/*!
 * \qmltype BottomEdgeRegion
 * \instantiates UCBottomEdgeRegion
 * \inherits QtObject
 * \inmodule Ubuntu.Components 1.3
 * \since Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief Defines an active region within the BottomEdge component.
 *
 * Bottom edge regions are portions within the bottom edge area which can define
 * different content or action whenever the drag enters in the area. The area is
 * defined by \l from and \l to properties vertically, whereas horizontally is
 * stretched across bottom edge width. Custom content can be defined through
 * \l contentUrl or \l contentComponent properties, which will override the
 * \l BottomEdge::contentUrl and \l BottomEdge::contentComponent properties for the
 * time the gesture is in the section area.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 *
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(70)
 *
 *     Page {
 *         header: PageHeader {
 *             title: "BottomEdge regions"
 *         }
 *
 *         BottomEdge {
 *             id: bottomEdge
 *             height: parent.height - units.gu(20)
 *             hint: BottomEdgeHint {
 *                 text: "My bottom edge"
 *             }
 *             // a fake content till we reach the committable area
 *             contentComponent: Rectangle {
 *                 width: bottomEdge.width
 *                 height: bottomEdge.height
 *                 color: UbuntuColors.green
 *             }
 *             // override bottom edge sections to switch to real content
 *             BottomEdgeRegion {
 *                 from: 0.33
 *                 contentComponent: Page {
 *                     width: bottomEdge.width
 *                     height: bottomEdge.height
 *                     header: PageHeader {
 *                         title: "BottomEdge Content"
 *                     }
 *                 }
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * Entering into the section area is signalled by the \l entered signal and when
 * drag leaves the area the \l exited signal is emitted. If the drag ends within
 * the section area, the \l dragEnded signal is emitted. In case the section's
 * \l to property is less than 1.0, the bottom edge content will only be exposed
 * to that value, and the \l BottomEdge::status will get the \e Committed value.
 * No further drag is possible after reaching \e Commited state.
 *
 * \note Whereas there is no restriction on making overlapping sections, beware that
 * overlapping sections changing the content through the \l contentUrl or \l contentComponent
 * properties will cause unpredictable results.
 */

UCBottomEdgeRegion::UCBottomEdgeRegion(QObject *parent, bool isDefault)
    : QObject(parent)
    , m_bottomEdge(qobject_cast<UCBottomEdge*>(parent))
    , m_component(Q_NULLPTR)
    , m_contentItem(Q_NULLPTR)
    , m_from(0.0)
    , m_to(-1.0)
    , m_enabled(true)
    , m_active(false)
    , m_default(isDefault)
{
    connect(&m_loader, &UbuntuToolkit::AsyncLoader::loadingStatus,
            this, &UCBottomEdgeRegion::onLoaderStatusChanged);
}

void UCBottomEdgeRegion::attachToBottomEdge(UCBottomEdge *bottomEdge)
{
    QQml_setParent_noEvent(this, bottomEdge);
    m_bottomEdge = bottomEdge;
    // adjust to property value if not set yet
    if (m_to <= 0.0) {
        m_to = 1.0;
        Q_EMIT toChanged();
    }
}

bool UCBottomEdgeRegion::contains(qreal dragRatio)
{
    return (m_enabled && (m_from < m_to) && dragRatio >= m_from && dragRatio <= m_to);
}

void UCBottomEdgeRegion::enter()
{
    m_active = true;
    Q_EMIT entered();

    LOG << "ENTER REGION" << objectName();
    // if preloaded, or default(?), set the content
    if (m_bottomEdge->preloadContent() || m_default) {
        if (m_loader.status() == UbuntuToolkit::AsyncLoader::Ready) {
            LOG << "SET REGION CONTENT" << objectName();
            UCBottomEdgePrivate::get(m_bottomEdge)->setCurrentContent();
        }
    } else {
        // initiate loading, component has priority
        loadRegionContent();
    }
}

void UCBottomEdgeRegion::exit()
{
    m_active = false;
    Q_EMIT exited();

    // detach content from BottomEdge
    LOG << "EXIT REGION" << objectName();
    UCBottomEdgePrivate::get(m_bottomEdge)->resetCurrentContent(nullptr);

    // then cleanup
    if (!m_contentItem) {
        return;
    }
    LOG << "RESET REGION CONTENT" << objectName();
    if (!m_bottomEdge->preloadContent()) {
        LOG << "DISCARD REGION CONTENT" << objectName();
        discardRegionContent();
    }
}

const QRectF UCBottomEdgeRegion::rect(const QRectF &bottomEdgeRect)
{
    QRectF regionRect(
                bottomEdgeRect.topLeft() + QPointF(0, bottomEdgeRect.height() * (1.0 - m_to)),
                QSizeF(bottomEdgeRect.width(), bottomEdgeRect.height() * (m_to - m_from)));
    return regionRect;
}

void UCBottomEdgeRegion::loadRegionContent()
{
    if (!m_enabled) {
        return;
    }
    LOG << "LOAD REGION CONTENT" << objectName();
    if (m_component) {
        loadContent(LoadingComponent);
    } else if (m_url.isValid()) {
        loadContent(LoadingUrl);
    }
}

void UCBottomEdgeRegion::loadContent(LoadingType type)
{
    // we must delete the previous content before we (re)initiate loading
    if (m_contentItem) {
        m_contentItem->deleteLater();;
        m_contentItem = nullptr;
    }
    // no need to create new context as we do not set any context properties
    // for which we would need one
    switch (type) {
    case LoadingUrl:
        m_loader.load(m_url, qmlContext(m_bottomEdge));
        return;
    case LoadingComponent:
        m_loader.load(m_component, qmlContext(m_bottomEdge));
        return;
    }
}

void UCBottomEdgeRegion::discardRegionContent()
{
    m_loader.reset();
    if (m_contentItem) {
        LOG << "DISCARD CONTENT" << objectName();
        m_contentItem->deleteLater();
    }
    m_contentItem = nullptr;
}

QQuickItem *UCBottomEdgeRegion::regionContent()
{
    return m_contentItem;
}

void UCBottomEdgeRegion::onLoaderStatusChanged(UbuntuToolkit::AsyncLoader::LoadingStatus status, QObject *object)
{
    bool emitChange = false;
    LOG << "STATUS" << status << object;
    if (status == UbuntuToolkit::AsyncLoader::Ready) {
        // if we are no longer active, no need to continue, and discard content
        // this may occur when the component was still in Compiling state while
        // the region was exited, therefore reset() could not cancel the operation.
        if (!m_active && !m_default && !m_bottomEdge->preloadContent()) {
            LOG << "DELETE REGION CONTENT" << objectName();
            object->deleteLater();
            return;
        }
        m_contentItem = qobject_cast<QQuickItem*>(object);
        emitChange = m_active || m_default;
    }

    if (status == UbuntuToolkit::AsyncLoader::Reset) {
        // de-parent first
        if (m_contentItem) {
            m_contentItem->setParentItem(nullptr);
            LOG << "RESET CONTENT" << objectName();
            m_contentItem->deleteLater();
        }
        m_contentItem = nullptr;
        emitChange = true;
    }

    if (emitChange && m_bottomEdge && m_active) {
        UCBottomEdgePrivate::get(m_bottomEdge)->setCurrentContent();
    }
}

/*!
 * \qmlproperty bool BottomEdgeRegion::enabled
 * Enables the section. Disabled sections do not trigger nor change the BottomEdge
 * content. Defaults to false.
 */
void UCBottomEdgeRegion::setEnabled(bool enabled)
{
    if (enabled == m_enabled) {
        return;
    }
    m_enabled = enabled;
    if (m_bottomEdge) {
        UCBottomEdgePrivate::get(m_bottomEdge)->validateRegion(this);
    }

    // load content if preload is set
    if (m_bottomEdge->preloadContent()) {
        if (!m_enabled) {
            discardRegionContent();
        } else {
            loadRegionContent();
        }
    }
    Q_EMIT enabledChanged();
}

/*!
 * \qmlproperty real BottomEdgeRegion::from
 * Specifies the starting ratio of the bottom erge area. The value must be bigger
 * or equal to 0 but strictly smaller than \l to. Defaults to 0.0.
 */
void UCBottomEdgeRegion::setFrom(qreal from)
{
    if (from == m_from) {
        return;
    }
    m_from = from;
    if (m_bottomEdge) {
        UCBottomEdgePrivate::get(m_bottomEdge)->validateRegion(this);
    }
    Q_EMIT fromChanged();
}

/*!
 * \qmlproperty real BottomEdgeRegion::to
 * Specifies the ending ratio of the bottom edge area. The value must be bigger
 * than \l from and smaller or equal to 1.0.
 * \note If the end point is less than 1.0, ending the drag within the section
 * will result in exposing the bottom edge content only till the ration specified
 * by this property.
 */
void UCBottomEdgeRegion::setTo(qreal to)
{
    if (to == m_to) {
        return;
    }
    m_to = to;
    if (m_bottomEdge) {
        UCBottomEdgePrivate::get(m_bottomEdge)->validateRegion(this);
    }
    Q_EMIT toChanged();
}

/*!
 * \qmlproperty url BottomEdgeRegion::contentUrl
 * Specifies the url to the document defining the section specific content. This
 * propery will temporarily override the \l BottomEdge::contentUrl property value
 * when the drag gesture enters the section area. The orginal value will be restored
 * once the gesture leaves the section area.
 */
void UCBottomEdgeRegion::setUrl(const QUrl &url)
{
    if (m_url == url) {
        return;
    }
    m_url = url;
    Q_EMIT contentChanged(m_url);
    // invoke loader if the preload is set
    if (m_bottomEdge && (m_bottomEdge->preloadContent() || m_default) && !m_url.isValid()) {
        loadContent(LoadingUrl);
    }
}

/*!
 * \qmlproperty Component BottomEdgeRegion::contentComponent
 * Specifies the component defining the section specific content. This propery
 * will temporarily override the \l BottomEdge::contentComponent property value
 * when the drag gesture enters the section area. The orginal value will be restored
 * once the gesture leaves the section area.
 */
void UCBottomEdgeRegion::setComponent(QQmlComponent *component)
{
    if (m_component == component) {
        return;
    }
    m_component = component;
    Q_EMIT contentComponentChanged(m_component);
    // invoke loader if the preload is set
    if (m_bottomEdge && (m_bottomEdge->preloadContent() || m_default) && m_component) {
        loadContent(LoadingComponent);
    }
}

/*!
 * \qmlsignal void BottomEdgeRegion::entered()
 * Signal triggered when the drag enters into the area defined by the bottom edge
 * section.
 */

/*!
 * \qmlsignal void BottomEdgeRegion::exited()
 * Signal triggered when the drag leaves the area defined by the bottom edge section.
 */

/*!
 * \qmlsignal void BottomEdgeRegion::dragEnded()
 * Signal triggered when the drag ends within the active bottom edge section area.
 */
