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

#include "ucbottomedge_p_p.h"
#include "ucbottomedgeregion_p_p.h"
#include <propertychange_p.h>
#include <QtQml/private/qqmlproperty_p.h>


UT_NAMESPACE_BEGIN

UCBottomEdgeRegionPrivate::UCBottomEdgeRegionPrivate()
    : QObjectPrivate()
    , bottomEdge(Q_NULLPTR)
    , component(Q_NULLPTR)
    , contentItem(Q_NULLPTR)
    , from(0.0)
    , to(-1.0)
    , enabled(true)
    , active(false)
{
}

void UCBottomEdgeRegionPrivate::init()
{
    Q_Q(UCBottomEdgeRegion);
    bottomEdge = qobject_cast<UCBottomEdge*>(parent);
    QObject::connect(&loader, SIGNAL(loadingStatus(AsyncLoader::LoadingStatus,QObject*)),
            q, SLOT(onLoaderStatusChanged(AsyncLoader::LoadingStatus,QObject*)));
}

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

UCBottomEdgeRegion::UCBottomEdgeRegion(QObject *parent)
    : QObject(*(new UCBottomEdgeRegionPrivate), parent)
{
    d_func()->init();
}

UCBottomEdgeRegion::UCBottomEdgeRegion(UCBottomEdgeRegionPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    d_func()->init();
}

void UCBottomEdgeRegionPrivate::attachToBottomEdge(UCBottomEdge *bottomEdge)
{
    Q_Q(UCBottomEdgeRegion);
    QQml_setParent_noEvent(q, bottomEdge);
    this->bottomEdge = bottomEdge;
    // adjust to property value if not set yet
    if (to <= 0.0) {
        to = 1.0;
        Q_EMIT q->toChanged();
    }

    // if preload is set, load content
    if (bottomEdge->preloadContent()) {
        loadRegionContent();
    }
}

bool UCBottomEdgeRegion::contains(qreal dragRatio)
{
    Q_D(UCBottomEdgeRegion);
    return (d->enabled && (d->from < d->to) && dragRatio >= d->from && dragRatio <= d->to);
}

// Called when drag ends to check whether content can be committed. The default
// implementation returns true.
bool UCBottomEdgeRegion::canCommit(qreal dragRatio)
{
    Q_UNUSED(dragRatio);
    return true;
}

void UCBottomEdgeRegion::enter()
{
    Q_D(UCBottomEdgeRegion);
    d->active = true;
    Q_EMIT entered();

    LOG << "ENTER REGION" << objectName();
    // if preloaded, or default(?), set the content
    if (d->bottomEdge->preloadContent()) {
        if (d->loader.status() == AsyncLoader::Ready) {
            LOG << "SET REGION CONTENT" << objectName();
            UCBottomEdgePrivate::get(d->bottomEdge)->setCurrentContent();
        }
    } else {
        // initiate loading, component has priority
        d->loadRegionContent();
    }
}

void UCBottomEdgeRegion::exit()
{
    Q_D(UCBottomEdgeRegion);
    d->active = false;
    Q_EMIT exited();

    // detach content from BottomEdge
    LOG << "EXIT REGION" << objectName();
    UCBottomEdgePrivate::get(d->bottomEdge)->resetCurrentContent(nullptr);

    // then cleanup
    if (!d->contentItem) {
        return;
    }
    LOG << "RESET REGION CONTENT" << objectName();
    if (!d->bottomEdge->preloadContent()) {
        LOG << "DISCARD REGION CONTENT" << objectName();
        d->discardRegionContent();
    }
}

const QRectF UCBottomEdgeRegion::rect(const QRectF &bottomEdgeRect)
{
    Q_D(UCBottomEdgeRegion);
    QRectF regionRect(
                bottomEdgeRect.topLeft() + QPointF(0, bottomEdgeRect.height() * (1.0 - d->to)),
                QSizeF(bottomEdgeRect.width(), bottomEdgeRect.height() * (d->to - d->from)));
    return regionRect;
}

void UCBottomEdgeRegionPrivate::loadRegionContent()
{
    if (!enabled) {
        return;
    }
    LOG << "LOAD REGION CONTENT" << q_func()->objectName() << contentItem;
    if (component) {
        loadContent(LoadingComponent);
    } else if (url.isValid()) {
        loadContent(LoadingUrl);
    }
}

void UCBottomEdgeRegionPrivate::loadContent(LoadingType type)
{
    // we must delete the previous content before we (re)initiate loading
    if (contentItem) {
        contentItem->deleteLater();;
        contentItem = nullptr;
    }
    // no need to create new context as we do not set any context properties
    // for which we would need one
    switch (type) {
    case LoadingUrl:
        loader.load(url, qmlContext(bottomEdge));
        return;
    case LoadingComponent:
        loader.load(component, qmlContext(bottomEdge));
        return;
    }
}

void UCBottomEdgeRegionPrivate::discardRegionContent()
{
    loader.reset();
    if (contentItem) {
        LOG << "DISCARD CONTENT" << q_func()->objectName();
        contentItem->deleteLater();
    }
    contentItem = nullptr;
}

void UCBottomEdgeRegionPrivate::onLoaderStatusChanged(AsyncLoader::LoadingStatus status, QObject *object)
{
    bool emitChange = false;
    LOG << "STATUS" << status << object;
    if (status == AsyncLoader::Ready) {
        // if we are no longer active, no need to continue, and discard content
        // this may occur when the component was still in Compiling state while
        // the region was exited, therefore reset() could not cancel the operation.
        if (!active && !bottomEdge->preloadContent()) {
            LOG << "DELETE REGION CONTENT" << q_func()->objectName();
            object->deleteLater();
            return;
        }
        contentItem = qobject_cast<QQuickItem*>(object);
        emitChange = active;
    }

    if (status == AsyncLoader::Reset) {
        // de-parent first
        if (contentItem) {
            contentItem->setParentItem(nullptr);
            LOG << "RESET CONTENT" << q_func()->objectName();
            contentItem->deleteLater();
        }
        contentItem = nullptr;
        emitChange = true;
    }

    if (emitChange && bottomEdge && active) {
        UCBottomEdgePrivate::get(bottomEdge)->setCurrentContent();
    }
}

/*!
 * \qmlproperty bool BottomEdgeRegion::enabled
 * Enables the section. Disabled sections do not trigger nor change the BottomEdge
 * content. Defaults to false.
 */
bool UCBottomEdgeRegion::enabled() const
{
    Q_D(const UCBottomEdgeRegion);
    return d->enabled;
}
void UCBottomEdgeRegion::setEnabled(bool enabled)
{
    Q_D(UCBottomEdgeRegion);
    if (enabled == d->enabled) {
        return;
    }
    d->enabled = enabled;
    if (d->bottomEdge) {
        UCBottomEdgePrivate::get(d->bottomEdge)->validateRegion(this);
        // load content if preload is set
        if (d->bottomEdge->preloadContent()) {
            if (!d->enabled) {
                d->discardRegionContent();
            } else {
                d->loadRegionContent();
            }
        }
    }

    Q_EMIT enabledChanged();
}

/*!
 * \qmlproperty real BottomEdgeRegion::from
 * Specifies the starting ratio of the bottom erge area. The value must be bigger
 * or equal to 0 but strictly smaller than \l to. Defaults to 0.0.
 */
qreal UCBottomEdgeRegion::from() const
{
    Q_D(const UCBottomEdgeRegion);
    return d->from;
}
void UCBottomEdgeRegion::setFrom(qreal from)
{
    Q_D(UCBottomEdgeRegion);
    if (from == d->from) {
        return;
    }
    d->from = from;
    if (d->bottomEdge) {
        UCBottomEdgePrivate::get(d->bottomEdge)->validateRegion(this);
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
qreal UCBottomEdgeRegion::to() const
{
    Q_D(const UCBottomEdgeRegion);
    return d->to;
}
void UCBottomEdgeRegion::setTo(qreal to)
{
    Q_D(UCBottomEdgeRegion);
    if (to == d->to) {
        return;
    }
    d->to = to;
    if (d->bottomEdge) {
        UCBottomEdgePrivate::get(d->bottomEdge)->validateRegion(this);
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
QUrl UCBottomEdgeRegion::url() const
{
    Q_D(const UCBottomEdgeRegion);
    return d->url;
}
void UCBottomEdgeRegion::setUrl(const QUrl &url)
{
    Q_D(UCBottomEdgeRegion);
    if (d->url == url) {
        return;
    }
    d->url = url;
    Q_EMIT contentChanged(d->url);
    // invoke loader if the preload is set
    if (d->bottomEdge && (d->bottomEdge->preloadContent()) && d->url.isValid()) {
        d->loadContent(UCBottomEdgeRegionPrivate::LoadingUrl);
    }
}

/*!
 * \qmlproperty Component BottomEdgeRegion::contentComponent
 * Specifies the component defining the section specific content. This propery
 * will temporarily override the \l BottomEdge::contentComponent property value
 * when the drag gesture enters the section area. The orginal value will be restored
 * once the gesture leaves the section area.
 */
QQmlComponent *UCBottomEdgeRegion::component() const
{
    Q_D(const UCBottomEdgeRegion);
    return d->component;
}
void UCBottomEdgeRegion::setComponent(QQmlComponent *component)
{
    Q_D(UCBottomEdgeRegion);
    if (d->component == component) {
        return;
    }
    d->component = component;
    Q_EMIT contentComponentChanged(d->component);
    // invoke loader if the preload is set
    if (d->bottomEdge && d->bottomEdge->preloadContent() && d->component) {
        d->loadContent(UCBottomEdgeRegionPrivate::LoadingComponent);
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

// default region

DefaultRegionPrivate::DefaultRegionPrivate()
{
}

void DefaultRegionPrivate::loadRegionContent()
{
    if (contentItem) {
        LOG << "region content loaded, return";
        return;
    }
    UCBottomEdgeRegionPrivate::loadRegionContent();
}

void DefaultRegionPrivate::discardRegionContent()
{
    // do nothing, do not cleanup the content!
    LOG << "suppress region discarding!";
}

DefaultRegion::DefaultRegion(UCBottomEdge *parent)
    : UCBottomEdgeRegion(*(new DefaultRegionPrivate), parent)
{
    UCBottomEdgeRegionPrivate *d = UCBottomEdgeRegionPrivate::get(this);
    d->from = 0.0;
    d->to = 1.0;
    setObjectName("default_BottomEdgeRegion");
}

bool DefaultRegion::canCommit(qreal dragRatio)
{
    return (dragRatio >= 0.33);
}

UT_NAMESPACE_END

#include "moc_ucbottomedgeregion_p.cpp"

