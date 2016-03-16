/*
 * Copyright 2016 Canonical Ltd.
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

#include "ucbottomedge_p.h"
#include "ucbottomedgestyle.h"
#include "ucbottomedgeregion.h"
#include "ucbottomedgehint_p.h"
#include "ucstyleditembase_p.h"
#include <QtQml/QQmlEngine>
#include <QtGui/QScreen>
#include <QtQml/QQmlProperty>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>

#include "plugin.h"
#include "ucnamespace.h"
#include "ucheader.h"
#include "ucaction.h"
#include "quickutils.h"
#include "private/ucswipearea_p.h"
#include <QtQuick/private/qquickanimation_p.h>

using namespace UbuntuToolkit;

Q_LOGGING_CATEGORY(ucBottomEdge, "ubuntu.components.BottomEdge", QtMsgType::QtWarningMsg)

UCBottomEdgePrivate::UCBottomEdgePrivate()
    : UCStyledItemBasePrivate()
    , defaultRegion(new UCBottomEdgeRegion(nullptr, true))
    , activeRegion(Q_NULLPTR)
    , hint(new UCBottomEdgeHint)
    , bottomPanel(Q_NULLPTR)
    , previousDistance(0.0)
    , dragProgress(0.)
    , status(UCBottomEdge::Hidden)
    , operationStatus(Idle)
    , dragDirection(UCBottomEdge::Undefined)
    , defaultRegionsReset(false)
    , mousePressed(false)
    , preloadContent(false)
{
}

void UCBottomEdgePrivate::init()
{
    Q_Q(UCBottomEdge);
    // initialize hint
    QQml_setParent_noEvent(hint, q);
    // keep BottomEdge.enabled in sync with the hint
    QObject::connect(q, &UCBottomEdge::enabledChanged, [=] {
        hint->setEnabled2(q->isEnabled());
    });

    // create default region
    // for testing purposes
    defaultRegion->setObjectName("default_BottomEdgeRegion");
    // enters in this stage when drag ratio reaches 30% of the area
    defaultRegion->m_from = 0.33;
    defaultRegion->m_to = 1.0;
    defaultRegion->attachToBottomEdge(q);

    QObject::connect(defaultRegion, &UCBottomEdgeRegion::contentChanged,
                     q, &UCBottomEdge::contentChanged);
    QObject::connect(defaultRegion, &UCBottomEdgeRegion::contentComponentChanged,
                     q, &UCBottomEdge::contentComponentChanged);

    regions.append(defaultRegion);

    // set the style name
    styleDocument = QStringLiteral("BottomEdgeStyle");
}

// overload default data property so we can filter out the regions declared
// in the body of the component as resources
QQmlListProperty<QObject> UCBottomEdgePrivate::data()
{
    return QQmlListProperty<QObject>(q_func(), 0, UCBottomEdgePrivate::overload_data_append,
                                             QQuickItemPrivate::data_count,
                                             QQuickItemPrivate::data_at,
                                             UCBottomEdgePrivate::overload_data_clear);
}

void UCBottomEdgePrivate::overload_data_append(QQmlListProperty<QObject> *data, QObject *object)
{
    QQuickItemPrivate::data_append(data, object);
    // if the object is a region, add to the regions as well
    UCBottomEdgeRegion *region = qobject_cast<UCBottomEdgeRegion*>(object);
    if (region) {
        UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(data->object));
        bottomEdge->appendRegion(region);
    }
}

void UCBottomEdgePrivate::overload_data_clear(QQmlListProperty<QObject> *data)
{
    // clear regions as well
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(data->object));
    bottomEdge->clearRegions(false);
    QQuickItemPrivate::data_clear(data);
}

// appends a BottomEdgeRegion to the list; clears the default regions before appending the
// custom ones
void UCBottomEdgePrivate::appendRegion(UCBottomEdgeRegion *region)
{
    Q_Q(UCBottomEdge);
    Q_ASSERT(region);
    // the region must be owned by other non-BottomEdge component,
    // otherwise we cannot "reuse" the region
    if (region->parent() != q && qobject_cast<UCBottomEdge*>(region->parent())) {
        qmlInfo(q) << "Cannot reuse region owned by other BottomEdge components";
        return;
    }

    // make sure we own the region!
    QQml_setParent_noEvent(region, q);
    // take ownership!
    QQmlEngine::setObjectOwnership(region, QQmlEngine::CppOwnership);
    region->attachToBottomEdge(q);

    if (!defaultRegionsReset) {
        defaultRegionsReset = true;
        regions.clear();
    }

    // validate the region before we append
    validateRegion(region);

    // if preload is set, load content
    if (preloadContent) {
        region->loadRegionContent();
    }

    // append region definition
    regions.append(region);
}

// clears the custom regions list and restores the default ones
void UCBottomEdgePrivate::clearRegions(bool destroy)
{
    if (!defaultRegionsReset) {
        return;
    }
    if (destroy) {
        qDeleteAll(regions);
    }
    regions.clear();
    defaultRegionsReset = false;
    regions.append(defaultRegion);
}

// validates an added region
void UCBottomEdgePrivate::validateRegion(UCBottomEdgeRegion *region, int regionsSize)
{
    // we should not validate unti the bottom edge is not completed,
    // property changes may invalidate the result of the validation
    // also leave if the region is disabled
    if (!componentComplete || !region->m_enabled) {
        return;
    }
    Q_Q(UCBottomEdge);
    // use QRectF to help on finding the intersection
    QRectF boundingRect = q->boundingRect();
    if (boundingRect.isNull()) {
        boundingRect = QRectF(0, 0, 1, 1);
    }
    if (regionsSize < 0 || regionsSize > regions.size()) {
        regionsSize = regions.size();
    }
    const QRectF regionRect(region->rect(boundingRect));
    for (int i = 0; i < regionsSize; ++i) {
        UCBottomEdgeRegion *stackedRegion = regions[i];
        if (region == stackedRegion || !stackedRegion->m_enabled) {
            continue;
        }
        QRectF rect(stackedRegion->rect(boundingRect));
        if (rect.contains(regionRect)) {
            qmlInfo(region) << QString("Region at index %1 contains this region. This region will never activate.").arg(i);
        } else {
            QRectF intersect = regionRect.intersected(stackedRegion->rect(boundingRect));
            if (!intersect.isNull()) {
                qmlInfo(region) << QString("Region intersects the one from index %1 having from: %2 and to: %3")
                                   .arg(i).arg(stackedRegion->m_from).arg(stackedRegion->m_to);
            }
        }
    }
}

// update status, drag direction and activeRegion during drag
void UCBottomEdgePrivate::updateProgressionStates(qreal distance)
{
    Q_Q(UCBottomEdge);

    // refresh drag progress
    setDragProgress(distance / q->height());

    detectDirection(distance);
    if (isLocked()) {
        // there is an operation ongoing, do not update drag and activeRegion
        return;
    }
    if (distance >= bottomPanel->m_revealThreshold) {
        // the content can be revealed
        setStatus(UCBottomEdge::Revealed);
    }

    // go through the regions and spot the active region
    UCBottomEdgeRegion *newActive = Q_NULLPTR;
    Q_FOREACH(UCBottomEdgeRegion *region, regions) {
        if (region->contains(dragProgress)) {
            newActive = region;
            break;
        }
    }
    if (newActive != activeRegion) {
        setActiveRegion(newActive);
    }
}

// set the active region
bool UCBottomEdgePrivate::setActiveRegion(UCBottomEdgeRegion *region)
{
    if (activeRegion == region) {
        return false;
    }
    if (activeRegion) {
        activeRegion->exit();
    }
    activeRegion = region;
    if (activeRegion) {
        activeRegion->enter();
    }
    Q_EMIT q_func()->activeRegionChanged(activeRegion);
    return true;
}

// updates the dragDirection property
void UCBottomEdgePrivate::detectDirection(qreal currentDistance)
{
    if (!previousDistance) {
        previousDistance = currentDistance;
    }

    UCBottomEdge::DragDirection newDirection = dragDirection;
    qreal delta = previousDistance - currentDistance;
    bool deltaPassed = abs(delta) >= qApp->styleHints()->startDragDistance();
    if (!deltaPassed) {
        return;
    }

    previousDistance = currentDistance;
    newDirection = (delta < 0) ? UCBottomEdge::Upwards : UCBottomEdge::Downwards;
    setDragDirection(newDirection);
}

// internal dragDirection property setter
void UCBottomEdgePrivate::setDragDirection(UCBottomEdge::DragDirection direction)
{
    if (dragDirection != direction) {
        dragDirection = direction;
        switch (dragDirection) {
        case UCBottomEdge::Undefined: LOG << "direction: Undefined"; break;
        case UCBottomEdge::Upwards: LOG << "direction: Upwards"; break;
        case UCBottomEdge::Downwards: LOG << "direction: Downwards"; break;
        }
        Q_EMIT q_func()->dragDirectionChanged(dragDirection);
    }
}

// proceed with drag completion action
void UCBottomEdgePrivate::onDragEnded()
{
    // collapse if we drag downwards, or not in any active region and we did not pass 30% of the BottomEdge height
    if (dragDirection == UCBottomEdge::Downwards || (!activeRegion && dragProgress < 0.33)) {
        q_func()->collapse();
    } else if (!activeRegion && dragProgress >= 0.33) {
        // commit if we are not in an active region but we passed 30% of the BottomEdge height
        q_func()->commit();
    } else if (activeRegion) {
        // emit region's dragEnded first
        Q_EMIT activeRegion->dragEnded();
        commit(activeRegion->m_to);
    }
}

void UCBottomEdgePrivate::commit(qreal to)
{
    if (operationStatus == CommitToTop
            || operationStatus == CommitToRegion
            || status == UCBottomEdge::Committed) {
        LOG << "redundant commit call";
        return;
    }
    Q_Q(UCBottomEdge);
    setOperationStatus(qFuzzyCompare(to, 1.0) ? CommitToTop : CommitToRegion);
    if (operationStatus == CommitToTop) {
        LOG << "emit commitStarted()";
        Q_EMIT q->commitStarted();
    }
    // make sure the status is set to revealed first
    bool animated = bottomPanel && bottomPanel->m_panelAnimation;
    if (animated) {
        QObject::connect(bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                q, &UCBottomEdge::unlockOperation, Qt::UniqueConnection);
    }
    // make sure the setStatus is set to Revealed first
    if (status == UCBottomEdge::Hidden) {
        setStatus(UCBottomEdge::Revealed);
    }
    setDragProgress(to);
    if (!animated) {
        q->unlockOperation(false);
    }
}

// common handler to complete the operations
void UCBottomEdge::unlockOperation(bool running)
{
    if (running) {
        return;
    }
    Q_D(UCBottomEdge);
    if (d->bottomPanel && d->bottomPanel->m_panelAnimation) {
        disconnect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                   0, 0);
    }

    UCBottomEdgePrivate::OperationStatus oldStatus = d->operationStatus;
    switch (d->operationStatus) {
    case UCBottomEdgePrivate::CommitToTop:
    case UCBottomEdgePrivate::CommitToRegion:
        d->setStatus(UCBottomEdge::Committed);
        d->patchContentItemHeader();
        if (d->operationStatus == UCBottomEdgePrivate::CommitToTop) {
            LOG << "emit commitCompleted()";
            Q_EMIT commitCompleted();
        }
        break;
    case UCBottomEdgePrivate::Collapsing:
        // no active region when collapsed!
        d->setActiveRegion(nullptr);
        d->setStatus(UCBottomEdge::Hidden);
        Q_EMIT collapseCompleted();
        break;
    default: break;
    }

    // the operation status may got changed due to a new operation being
    // initiated from the previosu one (e.g.collapse from commitCompleted, etc
    if (oldStatus == d->operationStatus) {
        d->setDragDirection(Undefined);
        d->setOperationStatus(UCBottomEdgePrivate::Idle);
    }
}

// positions the bottom edge panel holding the content to the given height percentage
void UCBottomEdgePrivate::setDragProgress(qreal position)
{
    if (dragProgress == position || (position < 0.0) || position > 1.0) {
        return;
    }
    dragProgress = position;
    Q_EMIT q_func()->dragProgressChanged(dragProgress);
}

// separated in a derived class to ease testing
UCCollapseAction::UCCollapseAction(QObject *parent)
    : UCAction(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::objectOwnership(parent));
}

void UCCollapseAction::activate()
{
    setIconName("down");
}

// inject collapse action into the content if the content has a PageHeader
void UCBottomEdgePrivate::patchContentItemHeader()
{
    // ugly, as it can be, as we don't have the PageHeader in cpp to detect the type
    UCHeader *header = currentContentItem ? currentContentItem->findChild<UCHeader*>() : Q_NULLPTR;
    if (!header || !QuickUtils::inherits(header, "PageHeader")) {
        return;
    }

    LOG << "PATCH HEADER" << currentContentItem.data();
    // get the navigationActions and inject an action there
    QVariant list(header->property("navigationActions"));
    QQmlListProperty<UCAction> actions = list.value< QQmlListProperty<UCAction> >();
    QList<UCAction*> *navigationActions = reinterpret_cast<QList<UCAction*>*>(actions.data);

    // do we have any action in the list?
    if (navigationActions->size()) {
        // we have actions in the list, check if those are ours
        UCCollapseAction *collapse = qobject_cast<UCCollapseAction*>(navigationActions->at(0));
        if (!collapse) {
            // not ours, clear the list
            navigationActions->clear();
        }
    }
    if (navigationActions->size() <= 0) {
        // no actions, patch
        navigationActions->append(new UCCollapseAction(header));
        // invoke PageHeader.navigationActionsChanged signal
        int signal = header->metaObject()->indexOfSignal("navigationActionsChanged()");
        if (signal >= 0) {
            header->metaObject()->invokeMethod(header, "navigationActionsChanged");
        }
    }

    // are we committed?
    if (status == UCBottomEdge::Committed) {
        // activate the action
        UCCollapseAction *collapse = qobject_cast<UCCollapseAction*>(navigationActions->at(0));
        Q_ASSERT(collapse);
        collapse->activate();
        QObject::connect(collapse, &UCAction::triggered, q_func(), &UCBottomEdge::collapse, Qt::DirectConnection);
    }
}

bool UCBottomEdgePrivate::loadStyleItem(bool animated)
{
    // fix styleVersion
    Q_Q(UCBottomEdge);
    if (!styleVersion) {
        styleVersion = BUILD_VERSION(1, 3);
    }
    bool result = UCStyledItemBasePrivate::loadStyleItem(animated);
    bottomPanel = qobject_cast<UCBottomEdgeStyle*>(styleItem);
    if (bottomPanel) {
        // reparent style item to the BottomEdge's parent
        bottomPanel->setParentItem(parentItem);
        // bring style item in front
        bottomPanel->setZ(std::numeric_limits<qreal>::max());
        // anchor to the bottom of the BottomEdge
        QQuickAnchors *styleAnchors = QQuickItemPrivate::get(bottomPanel)->anchors();
        styleAnchors->setBottom(anchors()->bottom());

        // move hint under the panel
        hint->setParentItem(bottomPanel);
        // and stack it before the panel, so it is covered by the panel when revealed
        hint->stackBefore(bottomPanel->m_panel);
    }
    return result;
}

// make sure the bottom edge panel is always the last one
void UCBottomEdgePrivate::itemChildAdded(QQuickItem *item, QQuickItem *)
{
    // make sure the BottomEdge's panel is the last one
    QQuickItem *last = item->childItems().last();
    if (bottomPanel && last != bottomPanel) {
        bottomPanel->stackAfter(last);
    }
}

// remove this to be change listener from the previous parent
void UCBottomEdgePrivate::itemChildRemoved(QQuickItem *item, QQuickItem *child)
{
    Q_Q(UCBottomEdge);
    if (child == q) {
        QQuickItemPrivate::get(item)->removeItemChangeListener(this, QQuickItemPrivate::Children);
    }
}

void UCBottomEdgePrivate::setOperationStatus(OperationStatus s)
{
    operationStatus = s;
    switch (s) {
    case Idle: LOG << "OP" << "Idle"; break;
    case CommitToTop: LOG << "OP" << "CommitToTop"; break;
    case CommitToRegion: LOG << "OP" << "CommitToRegion"; break;
    case Collapsing: LOG << "OP" << "Collapsing"; break;
    }
}

/*!
 * \qmltype BottomEdge
 * \instantiates UCBottomEdge
 * \inqmlmodule Ubuntu.Components 1.3
 * \inherits StyledItem
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.3
 * \brief A component to handle bottom edge gesture and content.
 *
 * The component provides bottom edge content handling. The bottom egde feature
 * is typically composed of a hint and some content. The contentUrl is committed
 * (i.e. fully shown) when the drag is completed after it has been dragged for
 * a certain amount, that is 30% of the height of the BottomEdge. The contentUrl
 * can be anything, defined by \l contentUrl or \l contentComponent.
 *
 * As the name suggests, the component automatically anchors to the bottom of its
 * parent and takes the width of the parent. The drag is detected within the parent
 * area, and the height drives till what extent the bottom edge content should be
 * exposed on \l commit call. The content is centered into a panel which is dragged from
 * the bottom of the BottomEdge. The content must specify its width and height.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 *
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(70)
 *
 *     Page {
 *         id: page
 *         title: "BottomEdge"
 *
 *         BottomEdge {
 *             height: parent.height - units.gu(20)
 *             hint.text: "My bottom edge"
 *             contentComponent: Rectangle {
 *                 width: page.width
 *                 height: page.height
 *                 color: UbuntuColors.green
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * \note The content is specified either through \l contentUrl or \l contentComponent,
 * where \l contentComponent has precedence over \l contentUrl.
 *
 * There can be situations when the content depends on the progress of the drag.
 * There are two possibilities to follow this, depending on the use case. The
 * \l dragProgress provides live updates about the fraction of the drag.
 * \qml
 * BottomEdge {
 *     id: bottomEdge
 *     height: parent.height
 *     hint.text: "progression"
 *     contentComponent: Rectangle {
 *         width: bottomEdge.width
 *         height: bottomEdge.height
 *         color: Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1);
 *     }
 * }
 * \endqml
 *
 * The other use case is when the content needs to be completely different in certain
 * regions of the area. These regions can be defined through BottomEdgeRegion elements
 * listed in the \l regions property.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 *
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(70)
 *
 *     Page {
 *         title: "BottomEdge"
 *
 *         BottomEdge {
 *             id: bottomEdge
 *             height: parent.height - units.gu(20)
 *             hint.text: "My bottom edge"
 *             contentComponent: Rectangle {
 *                 width: bottomEdge.width
 *                 height: bottomEdge.height
 *                 color: bottomEdge.activeRegion ?
 *                          bottomEdge.activeRegion.color : UbuntuColors.green
 *             }
 *             regions: [
 *                 BottomEdgeRegion {
 *                     from: 0.4
 *                     to: 0.6
 *                     property color color: UbuntuColors.red
 *                 },
 *                 BottomEdgeRegion {
 *                     from: 0.6
 *                     to: 1.0
 *                     property color color: UbuntuColors.silk
 *                 }
 *             ]
 *         }
 *     }
 * }
 * \endqml
 * \note Custom regions override the default declared ones. Therefore there must
 * be one region which has its \l {BottomEdgeRegion::to}{to} limit set to 1.0
 * otherwise the content will not be committed at all.
 * \note Regions can also be declared as child elements the same way as resources.
 *
 * The BottomEdge takes ownership over the custom BottomEdgeRegions, therefore
 * we cannot 'reuse' regions declared in other BottomEdge components, as those
 * will be destroyed together with the reusing BottomEdge component. The following
 * scenario only works if the \e customRegion is not used in any other regions.
 * \qml
 * Page {
 *     BottomEdge {
 *         id: bottomEdge
 *         hint.text: "reusing regions"
 *         // put your content and setup here
 *
 *         regions: [customRegion]
 *     }
 *
 *     BottomEdgeRegion {
 *         id: customRegion
 *         from: 0.2
 *     }
 * }
 * \endqml
 * \sa BottomEdgeRegion
 *
 * \section2 Page As Content
 * BottomEdge accepts any component to be set as content. Also it can detect
 * whether the content has a PageHeader component declared, and will inject a
 * collapse navigation action automatically. In case the content has no header,
 * the collapse must be provided by the content itself by calling the \l collapse
 * function.
 * \qml
 * BottomEdge {
 *     id: bottomEdge
 *     height: parent.height
 *     hint.text: "Sample collapse"
 *     contentComponent: Rectangle {
 *         width: bottomEdge.width
 *         height: bottomEdge.height
 *         color: Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1);
 *         Button {
 *             text: "Collapse"
 *             onClicked: bottomEdge.collapse()
 *         }
 *     }
 * }
 * \endqml
 * Alternatively you can put a PageHeader component in your custom content
 * as follows:
 * \qml
 * BottomEdge {
 *     id: bottomEdge
 *     height: parent.height
 *     hint.text: "Injected collapse"
 *     contentComponent: Rectangle {
 *         width: bottomEdge.width
 *         height: bottomEdge.height
 *         color: Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1);
 *         PageHeader {
 *             title: "Fancy content"
 *         }
 *     }
 * }
 * \endqml
 *
 * \section2 Styling
 * Similar to the other components the default style is expected to be defined
 * in the theme's \e BottomEdgeStyle. However the style is not parented to the
 * BottomEdge itself, but to the BottomEdge's parent item. When loaded, the style
 * does not fill the parent but its bottom anchor is set to the bottom of the
 * BottomEdge. Beside this the hint is also parented to the style instance. Custom
 * styles are expected to implement the BottomEgdeStyle API.
 */

/*!
 * \qmlsignal BottomEdge::commitStarted()
 * Signal emitted when the content commit is started.
 */

/*!
 * \qmlsignal BottomEdge::commitCompleted()
 * Signal emitted when the content commit is completed.
 */

/*!
 * \qmlsignal BottomEdge::collapseStarted()
 * Signal emitted when the content collapse is started.
 */

/*!
 * \qmlsignal BottomEdge::collapseCompleted()
 * Signal emitted when the content collapse is completed.
 */

UCBottomEdge::UCBottomEdge(QQuickItem *parent)
    : UCStyledItemBase(*(new UCBottomEdgePrivate), parent)
{
    Q_D(UCBottomEdge);
    d->init();
}
UCBottomEdge::~UCBottomEdge()
{
}

void UCBottomEdge::initializeComponent()
{
    Q_D(UCBottomEdge);
    // initialize hint
    d->hint->setParentItem(this);
    d->hint->init();

    // hint click() always commits
    connect(d->hint, SIGNAL(clicked()), this, SLOT(commit()), Qt::DirectConnection);

    // follow drag progress
    connect(d->hint->swipeArea(), &UCSwipeArea::distanceChanged, [=](qreal distance) {
        d->updateProgressionStates(distance);
    });

    // follow swipe end
    connect(d->hint->swipeArea(), &UCSwipeArea::draggingChanged, [=](bool dragging) {
        if (!dragging) {
            d->onDragEnded();
        }
    });

    // filter hint for mouse events
    d->hint->installEventFilter(this);
}

void UCBottomEdge::onParentHeightChanged()
{
    Q_D(UCBottomEdge);
    if (d->parentItem) {
        setImplicitHeight(d->parentItem->height());
    }
}

void UCBottomEdge::classBegin()
{
    UCStyledItemBase::classBegin();
    initializeComponent();
}

void UCBottomEdgePrivate::completeComponentInitialization()
{
    UCStyledItemBasePrivate::completeComponentInitialization();
    Q_Q(UCBottomEdge);
    // trigger default region content loading; the default region content is
    // always pre-loaded
    defaultRegion->loadRegionContent();

    // fix the hint's style version as that has no qmlContext of its own
    // and thus import version check will fail; setting the context for
    // the hint using this component's hint won't work either as this
    // component's context does not contain the properties from the hint.
    UCBottomEdgeHintPrivate *hintPrivate = UCBottomEdgeHintPrivate::get(hint);
    hintPrivate->styleVersion = styleVersion;
    // also set the qml data as hint does not have that either
    QQmlData::get(hint, true);
    QQmlEngine::setContextForObject(hint, new QQmlContext(qmlContext(q), hint));
    // finally complete hint creation
    hintPrivate->completeComponentInitialization();
    // and validate regions, leave out the first one as that supposed to be added first
    // mimic the top limit of the regions list like we would add them one by one
    for (int i = 1; i < regions.size(); ++i) {
        UCBottomEdgeRegion *region = regions[i];
        validateRegion(region, i);
    }
}

void UCBottomEdge::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged) {
        Q_D(UCBottomEdge);
        // disconnect from old parent
        if (d->oldParentItem) {
            disconnect(d->oldParentItem, &QQuickItem::heightChanged,
                       this, &UCBottomEdge::onParentHeightChanged);
        }
        QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
        if (data.item) {
            const QQuickAnchorLine left = QQuickItemPrivate::get(data.item)->left();
            const QQuickAnchorLine right = QQuickItemPrivate::get(data.item)->right();
            const QQuickAnchorLine bottom = QQuickItemPrivate::get(data.item)->bottom();
            anchors->setLeft(left);
            anchors->setRight(right);
            anchors->setBottom(bottom);
            QQuickItemPrivate::get(data.item)->addItemChangeListener(d, QQuickItemPrivate::Children);
            // follow implicitHeight
            connect(data.item, &QQuickItem::heightChanged,
                    this, &UCBottomEdge::onParentHeightChanged);
            onParentHeightChanged();
        } else {
            anchors->resetLeft();
            anchors->resetRight();
            anchors->resetBottom();
        }
        if (d->bottomPanel) {
            d->bottomPanel->setParentItem(data.item);
        }
    }
    UCStyledItemBase::itemChange(change, data);
}

bool UCBottomEdge::eventFilter(QObject *target, QEvent *event)
{
    Q_D(UCBottomEdge);

    switch (event->type()) {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        d->mousePressed = d->hint->contains(mouse->localPos());
        LOG << "drag with mouse";
        break;
    }
    case QEvent::MouseButtonRelease:
        if (d->mousePressed) {
            d->onDragEnded();
        }
        d->mousePressed = false;
        break;
    case QEvent::MouseMove:
    {
        if (d->mousePressed) {
            QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
            qreal mouseItemY = mapFromScene(mouse->windowPos()).y();
            qreal distance = abs(height() - mouseItemY);
            d->updateProgressionStates(distance);
        }
        break;
    }
    case QEvent::KeyPress: {
        QKeyEvent *keyPress = static_cast<QKeyEvent*>(event);
        switch (keyPress->key()) {
            case Qt::Key_Escape:
                collapse();
            default:
                break;
        }
    }
    default: break;
    }
    return UCStyledItemBase::eventFilter(target, event);
}

/*!
 * \qmlproperty BottomEdgeHint BottomEdge::hint
 * The property holds the component to display the hint for the bottom edge element.
 */
UCBottomEdgeHint *UCBottomEdge::hint() const
{
    Q_D(const UCBottomEdge);
    return d->hint;
}

/*!
 * \qmlproperty real BottomEdge::dragProgress
 * \readonly
 * The property specifies the proggress of the drag within [0..1] interval.
 */
qreal UCBottomEdge::dragProgress()
{
    Q_D(UCBottomEdge);
    return d->dragProgress;
}

/*!
 * \qmlproperty DragDirection BottomEdge::dragDirection
 * \readonly
 * The property reports the current direction of the drag. The direction is flipped
 * when the drag passes the drag threshold.
 * \table
 * \header
 *  \li DragDirection
 *  \li Description
 * \row
 *  \li Undefined
 *  \li Default. The drag is not performed or the direction is not detected.
 * \row
 *  \li Upwards
 *  \li The drag is performed from bottom up or it passed the drag threshold from
 *      from the last point the drag was going downwards.
 * \row
 *  \li Downwards
 *  \li The drag is performed from up to bottom or it passed the drag threshold from
 *      from the last point the drag was going upwards.
 * \endtable
 *
 * Defaults to \e Undefined
 */
UCBottomEdge::DragDirection UCBottomEdge::dragDirection() const
{
    Q_D(const UCBottomEdge);
    return d->dragDirection;
}

/*!
 * \qmlproperty Status BottomEdge::status
 * \readonly
 * The property reports the actual state of the bottom edge. It can have the
 * following values:
 * \table
 * \header
 *  \li Status
 *  \li Description
 * \row
 *  \li Hidden
 *  \li The bottom edge is hidden. This does not contain the hint states.
 * \row
 *  \li Revealed
 *  \li The Bottom edge content is revealed. The state can be reached only if the
 *      \l hint is in "Active" state.
 * \row
 *  \li Committed
 *  \li The bottom edge content is fully exposed.
 * \endtable
 * \note Once \e Commited status is set, no further draging is possible on the content.
 */
UCBottomEdge::Status UCBottomEdge::status() const
{
    Q_D(const UCBottomEdge);
    return d->status;
}
void UCBottomEdgePrivate::setStatus(UCBottomEdge::Status status)
{
    if (status == this->status) {
        return;
    }
    this->status = status;
    // logging
    switch (status) {
        case UCBottomEdge::Hidden: LOG << "STATUS" << "Hidden"; break;
        case UCBottomEdge::Revealed: LOG << "STATUS" << "Revealed"; break;
        case UCBottomEdge::Committed: LOG << "STATUS" << "Committed"; break;
    }

    if (bottomPanel) {
        bottomPanel->setConsumeMouse(status > UCBottomEdge::Hidden);
    }

    Q_EMIT q_func()->statusChanged(this->status);

    // show content
    if (status > UCBottomEdge::Hidden) {
        setCurrentContent();
    } else {
        resetCurrentContent(nullptr);
    }
}

/*!
 * \qmlproperty url BottomEdge::contentUrl
 * The property holds the url to the document defining the content of the bottom
 * edge. The property behaves the same way as Loader's \e source property.
 */
QUrl UCBottomEdge::contentUrl() const
{
    Q_D(const UCBottomEdge);
    return d->defaultRegion->m_url;
}
void UCBottomEdge::setContent(const QUrl &url)
{
    Q_D(UCBottomEdge);
    d->defaultRegion->setUrl(url);
}

/*!
 * \qmlproperty Component BottomEdge::contentComponent
 * The property holds the component defining the content of the bottom edge. The
 * property behaves the same way as Loader's \e sourceComponent property.
 */
QQmlComponent *UCBottomEdge::contentComponent() const
{
    Q_D(const UCBottomEdge);
    return d->defaultRegion->m_component;
}
void UCBottomEdge::setContentComponent(QQmlComponent *component)
{
    Q_D(UCBottomEdge);
    d->defaultRegion->setComponent(component);
}

/*!
 * \qmlproperty Item BottomEdge::contentItem
 * \readonly
 * The property holds the item created either from \l contentUrl or \l contentComponent
 * properties.
 */
QQuickItem *UCBottomEdge::contentItem() const
{
    Q_D(const UCBottomEdge);
    return d->currentContentItem.data();
}

/*!
 * \qmlmethod void BottomEdge::commit()
 * The function forces the bottom edge content to be fully exposed. Emits
 * \l commitStarted and \l commitCompleted signals to notify the start and the
 * completion of the commit operation. It is safe to call commit() multiple times.
 */
void UCBottomEdge::commit()
{
    Q_D(UCBottomEdge);
    d->commit(1.0);
}

/*!
 * \qmlmethod void BottomEdge::collapse()
 * The function forces the bottom edge content to be hidden. Emits \l collapseStarted
 * and \l collapseCompleted signals to notify the start and the completion of the
 * collapse operation.
 */
void UCBottomEdge::collapse()
{
    Q_D(UCBottomEdge);
    if (d->operationStatus == UCBottomEdgePrivate::Collapsing || d->status == UCBottomEdge::Hidden) {
        LOG << "redundant collapse call";
        return;
    }
    d->setOperationStatus(UCBottomEdgePrivate::Collapsing);
    Q_EMIT collapseStarted();
    bool animated = d->bottomPanel && d->bottomPanel->m_panelAnimation;
    if (animated) {
        connect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                this, &UCBottomEdge::unlockOperation, Qt::UniqueConnection);
    }
    // set the setStatus first to Revealed
    if (d->status == UCBottomEdge::Committed) {
        d->setStatus(Revealed);
    }
    d->setDragProgress(0.0);
    if (!animated) {
        unlockOperation(false);
    }
}

/*!
 * \qmlproperty list<BottomEdgeRegion> BottomEdge::regions
 * The property holds the custom regions configured for the BottomEdge. The default
 * configuration contains one region, which commits the content when reached. The
 * defaults can be restored by setting an empty list to the property or by
 * calling regions.clear().
 * See \l BottomEdgeRegion.
 */
QQmlListProperty<UCBottomEdgeRegion> UCBottomEdge::regions()
{
    Q_D(UCBottomEdge);
    return QQmlListProperty<UCBottomEdgeRegion>(this, &d->regions,
                                                 regions_append,
                                                 regions_count,
                                                 regions_at,
                                                 regions_clear);
}

void UCBottomEdge::regions_append(QQmlListProperty<UCBottomEdgeRegion> *regions, UCBottomEdgeRegion *region)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(regions->object));
    bottomEdge->appendRegion(region);
}

int UCBottomEdge::regions_count(QQmlListProperty<UCBottomEdgeRegion> *regions)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(regions->object));
    return bottomEdge->regions.size();
}

UCBottomEdgeRegion *UCBottomEdge::regions_at(QQmlListProperty<UCBottomEdgeRegion> *regions, int index)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(regions->object));
    Q_ASSERT((index >= 0) && (index < bottomEdge->regions.size()));
    return bottomEdge->regions.at(index);
}

void UCBottomEdge::regions_clear(QQmlListProperty<UCBottomEdgeRegion> *regions)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(regions->object));
    bottomEdge->clearRegions(true);
}

/*!
 * \qmlproperty BottomEdgeRegion BottomEdge::activeRegion
 * \readonly
 * Specifies the current active region.
 */
UCBottomEdgeRegion *UCBottomEdge::activeRegion()
{
    Q_D(UCBottomEdge);
    return d->activeRegion;
}

/*!
 * \qmlproperty bool BottomEdge::preloadContent
 * If set, all the contents set in the component and in regions will be loaded
 * in the background, so it will be available before it is revealed.
 */
bool UCBottomEdge::preloadContent() const
{
    return d_func()->preloadContent;
}
void UCBottomEdge::setPreloadContent(bool value)
{
    Q_D(UCBottomEdge);
    if (d->preloadContent == value) {
        return;
    }
    d->preloadContent = value;

    if (d->preloadContent) {
        // we load all region's content, but we skip teh default one,
        // as default one is always preloaded
        for (int i = 0; i < d->regions.size(); i++) {
            UCBottomEdgeRegion *region = d->regions[i];
            if (region == d->defaultRegion) {
                continue;
            }
            region->loadRegionContent();
        }
    } else {
        // discard all inactive regions content, except default one
        for (int i = 0; i < d->regions.size(); i++) {
            UCBottomEdgeRegion *region = d->regions[i];
            if (region != d->activeRegion && region != d->defaultRegion) {
                region->discardRegionContent();
            }
        }
    }

    Q_EMIT preloadContentChanged();
}

/*
 * Set the current content. The content is taken from the region if active, or foced
 * is set. If the region has no item, the logic falls back to use the default region's
 * content.
 */
void UCBottomEdgePrivate::setCurrentContent()
{
    QQuickItem *newContent = nullptr;
    if (activeRegion) {
        newContent = activeRegion->regionContent();
        LOG << "ACTIVE REGION CONTENT" << activeRegion->objectName() << newContent;
    }
    if (!newContent) {
        newContent = defaultRegion->regionContent();
        LOG << "USING DEFAULT" << newContent;
    }
    resetCurrentContent(newContent);
}
/*
 * Set the content to the default region one.
 */
void UCBottomEdgePrivate::resetCurrentContent(QQuickItem *newContent)
{
    if (newContent == currentContentItem) {
        // same content, no need to reset to
        return;
    }
    if (currentContentItem) {
        // detach current content, the content will be deleted by the exiting region
        currentContentItem->setVisible(false);
        currentContentItem->setParentItem(nullptr);
    }

    if (!newContent) {
        // try to use the default one
        newContent = defaultRegion->regionContent();
    }

    // if it is still null, leave - we may not have default content at all!
    if (!newContent) {
        return;
    }

    LOG << "RESET CONTENT TO" << newContent;
    currentContentItem = newContent;
    currentContentItem->setParentItem(bottomPanel->m_panel);
    QQuickAnchors *anchors = QQuickItemPrivate::get(currentContentItem.data())->anchors();
    anchors->setHorizontalCenter(QQuickItemPrivate::get(bottomPanel->m_panel)->horizontalCenter());
    currentContentItem->setVisible(true);
    Q_EMIT q_func()->contentItemChanged();
    patchContentItemHeader();
}

#include "moc_ucbottomedge.cpp"
