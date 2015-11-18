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

#include "ucbottomedge_p.h"
#include "ucbottomedgestyle.h"
#include "ucbottomedgerange.h"
#include "ucbottomedgehint.h"
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
#include "gestures/ucswipearea.h"
#include <QtQuick/private/qquickanimation_p.h>

UCBottomEdgePrivate::UCBottomEdgePrivate()
    : UCStyledItemBasePrivate()
    , activeRange(Q_NULLPTR)
    , hint(new UCBottomEdgeHint)
    , contentComponent(Q_NULLPTR)
    , bottomPanel(Q_NULLPTR)
    , previousPanelY(0.0)
    , state(UCBottomEdge::Hidden)
    , operationStatus(Idle)
    , dragDirection(UCBottomEdge::Undefined)
    , defaultRangesReset(false)
{
}

void UCBottomEdgePrivate::init()
{
    Q_Q(UCBottomEdge);
    // initialize hint
    QQml_setParent_noEvent(hint, q);
    hint->setParentItem(q);
    QObject::connect(hint, SIGNAL(clicked()), q, SLOT(commit()), Qt::DirectConnection);

    // create default stages
    createDefaultRanges();

    // set the style name
    styleDocument = QStringLiteral("BottomEdgeStyle");
}

// overload default data property so we can filter out the ranges declared
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
    // if the object is a range, add to the ranges as well
    UCBottomEdgeRange *range = qobject_cast<UCBottomEdgeRange*>(object);
    if (range) {
        UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(data->object));
        bottomEdge->appendRange(range);
    }
}

void UCBottomEdgePrivate::overload_data_clear(QQmlListProperty<QObject> *data)
{
    // clear ranges as well
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(data->object));
    bottomEdge->clearRanges(false);
    QQuickItemPrivate::data_clear(data);
}

// appends a BottomEdgeRange to the list; clears the default ranges before appends the
// custom ones
void UCBottomEdgePrivate::appendRange(UCBottomEdgeRange *range)
{
    Q_Q(UCBottomEdge);
    // the range must be owned by the component, we cannot "reuse" declared ranges
    Q_ASSERT(range && range->parent() == q);
    if (!defaultRangesReset) {
        defaultRangesReset = true;
        qDeleteAll(ranges);
        ranges.clear();
    }
    ranges.append(range);
    // take ownership!
    QQmlEngine::setObjectOwnership(range, QQmlEngine::CppOwnership);
    range->attachToBottomEdge(q);
}

// clears the custom ranges list and restores the default ones
void UCBottomEdgePrivate::clearRanges(bool destroy)
{
    if (!defaultRangesReset) {
        return;
    }
    if (destroy) {
        qDeleteAll(ranges);
    }
    ranges.clear();
    defaultRangesReset = false;
    createDefaultRanges();
}

// creates the default ranges(s)
void UCBottomEdgePrivate::createDefaultRanges()
{
    Q_Q(UCBottomEdge);
    // add the default stages
    UCBottomEdgeRange *commitRange = new UCBottomEdgeRange(q);
    // for testing purposes
    commitRange->setObjectName("default_bottomedgerange");
    // enters in this stage when drag ratio reaches 30% of the area
    commitRange->m_from = 0.33;
    commitRange->m_to = 1.0;

    ranges.append(commitRange);
}

// update state, drag direction and activeRange during drag
void UCBottomEdgePrivate::updateProgressionStates()
{
    detectDirection(bottomPanel->m_panel->y());
    if (isLocked()) {
        // there is an operation ongoing, do not update drag and activeRange
        return;
    }
    Q_Q(UCBottomEdge);
    qreal progress = q->dragProgress();
    if (progress > 0) {
        // the content is revealed
        setState(UCBottomEdge::Revealed);
    }

    // go through the ranges
    Q_FOREACH(UCBottomEdgeRange *range, ranges) {
        if (range->contains(progress)) {
            if (setActiveRange(range)) {
                break;
            }
        } else if (activeRange == range) {
            setActiveRange(Q_NULLPTR);
        }
    }
}

// set teh active range
bool UCBottomEdgePrivate::setActiveRange(UCBottomEdgeRange *range)
{
    if (activeRange == range) {
        return false;
    }
    if (activeRange) {
        activeRange->exit();
    }
    activeRange = range;
    if (activeRange) {
        activeRange->enter();
    }
    Q_EMIT q_func()->activeRangeChanged();
    return true;
}

// updates the dragDirection property
void UCBottomEdgePrivate::detectDirection(qreal currentPanelY)
{
    if (!previousPanelY) {
        previousPanelY = currentPanelY;
    }

    UCBottomEdge::DragDirection newDirection = dragDirection;
    qreal delta = previousPanelY - currentPanelY;
    bool deltaPassed = abs(delta) >= qApp->styleHints()->startDragDistance();
    if (!deltaPassed) {
        return;
    }

    previousPanelY = currentPanelY;
    newDirection = (delta > 0) ? UCBottomEdge::Upwards : UCBottomEdge::Downwards;
    setDragDirection(newDirection);
}

// internal dragDirection property setter
void UCBottomEdgePrivate::setDragDirection(UCBottomEdge::DragDirection direction)
{
    if (dragDirection != direction) {
        dragDirection = direction;
        Q_EMIT q_func()->dragDirectionChanged();
    }
}


// positions the bottom edge panel holding the content to the given height percentage
void UCBottomEdgePrivate::positionPanel(qreal position)
{
    // use QQmlProperty so the bindings on y get broken when we set the value
    // otherwise AdaptivePageLayout relayouting makes bottom edge to collapse
    // also makes Behavior to run on the property
    Q_Q(UCBottomEdge);
    qreal height = q->height();
    QQmlProperty::write(bottomPanel->m_panel, "y", height - height * position, qmlContext(bottomPanel->m_panel));
}

// inject collapse action into the content if the content has a PageHeader
Q_DECLARE_METATYPE(QQmlListProperty<UCAction>)
void UCBottomEdgePrivate::patchContentItemHeader()
{
    // ugly, as it can be, as we don't have teh PageHeader in cpp to detect the type
    UCHeader *header = bottomPanel->m_contentItem ? bottomPanel->m_contentItem->findChild<UCHeader*>() : Q_NULLPTR;
    if (!header || !QuickUtils::inherits(header, "PageHeader")) {
        return;
    }

    // get the navigationActions and inject an action there
    QVariant list(header->property("navigationActions"));
    QQmlListProperty<UCAction> actions = list.value< QQmlListProperty<UCAction> >();
    QList<UCAction*> *navigationActions = reinterpret_cast<QList<UCAction*>*>(actions.data);

    // clear the actions first
    navigationActions->clear();

    // inject the action
    UCAction *collapse = new UCAction(header);
    QQmlEngine::setObjectOwnership(collapse, QQmlEngine::objectOwnership(header));
    collapse->setIconName("down");
    QObject::connect(collapse, &UCAction::triggered, q_func(), &UCBottomEdge::collapse, Qt::DirectConnection);
    navigationActions->append(collapse);

    // invoke PageHeader.navigationActionsChanged signal
    int signal = header->metaObject()->indexOfSignal("navigationActionsChanged()");
    if (signal >= 0) {
        header->metaObject()->invokeMethod(header, "navigationActionsChanged");
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

        // connect style stuff
        QObject::connect(bottomPanel, &UCBottomEdgeStyle::contentItemChanged, [=]() {
            patchContentItemHeader();
        });
        QObject::connect(bottomPanel, &UCBottomEdgeStyle::contentItemChanged,
                         q, &UCBottomEdge::contentItemChanged, Qt::DirectConnection);
        QObject::connect(bottomPanel->m_panel, &QQuickItem::yChanged,
                         q, &UCBottomEdge::dragProggressChanged, Qt::DirectConnection);
        QObject::connect(bottomPanel->m_panel, &QQuickItem::yChanged, [=]() {
            updateProgressionStates();
        });
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
 * is typically composed of a hint and some content. The content is committed
 * (i.e. fully shown) when the drag is completed after it has been dragged for
 * a certain amount. The content can be anything, defined by \l content or
 * \l contentComponent.
 *
 * As the name suggests, the component automatically anchors to the bottom of its
 * parent and takes the width of the parent. The drag is detected within the parent
 * area.
 *
 * The height of the BottomEdge drives till what extent the bottom edge content
 * should be exposed. The content is centered into a panel which is dragged from
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
 * \note \l content, \l contentComponent and \l contentItem is equivalent to
 * Loader's \e source, \e sourceComponent and \e item properties. See Loader
 * documentation for further details.
 *
 * There can be situations when the content depends on the progress of the drag
 * or when the drag is in different regions of the area. The first can be achieved
 * by tracking the \l dragProgress.
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
 * An other case is when the content needs to be completely different in certain
 * regions of the area. These regions can be defined through BottomEdgeRange elements
 * listed in the \l ranges property.
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
 *                 color: bottomEdge.activeRange ?
 *                          bottomEdge.activeRange.color : UbuntuColors.green
 *             }
 *             ranges: [
 *                 BottomEdgeRange {
 *                     from: 0.4
 *                     to: 0.6
 *                     property color color: UbuntuColors.red
 *                 },
 *                 BottomEdgeRange {
 *                     from: 0.6
 *                     to: 1.0
 *                     property color color: UbuntuColors.lightGrey
 *                 }
 *             ]
 *         }
 *     }
 * }
 * \endqml
 * \note Ranges can also be declared as child elements the same way as resources.
 * \sa BottomEdgeRange
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
 *             text: "Fancy content"
 *         }
 *     }
 * }
 * \endqml
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

void UCBottomEdge::classBegin()
{
    UCStyledItemBase::classBegin();
    Q_D(UCBottomEdge);
    // initialize hint
    d->hint->init();
}

void UCBottomEdge::componentComplete()
{
    UCStyledItemBase::componentComplete();
    Q_D(UCBottomEdge);
    // fix the hint's style version as that has no qmlContext of its own
    // and thus import version check will fail; setting the context for
    // the hint using this component's hint won't work either as this
    // component's context does not contain the properties from the hint.
    UCStyledItemBasePrivate *hintPrivate = UCStyledItemBasePrivate::get(d->hint);
    hintPrivate->styleVersion = d->styleVersion;
    // also set the qml data as hitn does not have that either
    QQmlData::get(d->hint, true);
    QQmlEngine::setContextForObject(d->hint, new QQmlContext(qmlContext(this), d->hint));
    // finally complete hint creation
    hintPrivate->completeStyledItem();
}

void UCBottomEdge::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemVisibleHasChanged) {
    }
    if (change == ItemParentHasChanged) {
        Q_D(UCBottomEdge);
        QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
        if (data.item) {
            const QQuickAnchorLine left = QQuickItemPrivate::get(data.item)->left();
            const QQuickAnchorLine right = QQuickItemPrivate::get(data.item)->right();
            const QQuickAnchorLine bottom = QQuickItemPrivate::get(data.item)->bottom();
            anchors->setLeft(left);
            anchors->setRight(right);
            anchors->setBottom(bottom);
            QQuickItemPrivate::get(data.item)->addItemChangeListener(d, QQuickItemPrivate::Children);
        } else {
            anchors->resetLeft();
            anchors->resetRight();
            anchors->resetBottom();
        }
    }
    QQuickItem::itemChange(change, data);
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
    return (d->bottomPanel && d->bottomPanel->m_panel) ? 1.0 - (d->bottomPanel->m_panel->y() / height()) : 0.0;
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
 * \qmlproperty enum BottomEdge::state
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
 * \note Once \e RangeCommitted and \e Commited states are set, no further draging
 * is possible on the content.
 */
UCBottomEdge::State UCBottomEdge::state() const
{
    Q_D(const UCBottomEdge);
    return d->state;
}
void UCBottomEdgePrivate::setState(UCBottomEdge::State state)
{
    if (state == this->state) {
        return;
    }
    this->state = state;
    QString stateStr;
    switch (state) {
        case UCBottomEdge::Hidden: stateStr = "Hidden"; break;
        case UCBottomEdge::Revealed: stateStr = "Revealed"; break;
        case UCBottomEdge::Committed: stateStr = "Committed"; break;
    }

    if (bottomPanel) {
        bottomPanel->setConsumeMouse(state > UCBottomEdge::Hidden);
    }

    qDebug() << "STATE" << stateStr;
    Q_EMIT q_func()->stateChanged(this->state);
}

/*!
 * \qmlproperty url BottomEdge::content
 * The property holds the url to the document defining the content of the bottom
 * edge. The property behaves the same way as Loader's \e source property.
 */
QUrl UCBottomEdge::content() const
{
    Q_D(const UCBottomEdge);
    return d->contentUrl;
}
void UCBottomEdge::setContent(const QUrl &url)
{
    Q_D(UCBottomEdge);
    if (d->contentUrl == url) {
        return;
    }

    d->contentUrl = url;
    Q_EMIT contentChanged(d->contentUrl);
}

/*!
 * \qmlproperty Component BottomEdge::contentComponent
 * The property holds the component defining the content of the bottom edge. The
 * property behaves the same way as Loader's \e sourceComponent property.
 */
QQmlComponent *UCBottomEdge::contentComponent() const
{
    Q_D(const UCBottomEdge);
    return d->contentComponent;
}
void UCBottomEdge::setContentComponent(QQmlComponent *component)
{
    Q_D(UCBottomEdge);
    if (d->contentComponent == component) {
        return;
    }
    d->contentComponent = component;
    Q_EMIT contentComponentChanged(d->contentComponent);
}

/*!
 * \qmlproperty Item BottomEdge::contentItem
 * \readonly
 * The property holds the item created either from \l content or \l contentComponent
 * properties.
 */
QQuickItem *UCBottomEdge::contentItem() const
{
    Q_D(const UCBottomEdge);
    return d->bottomPanel ? d->bottomPanel->m_contentItem : Q_NULLPTR;
}

/*!
 * \qmlmethod void BottomEdge::commit()
 * The function forces the bottom edge content to be fully exposed. Emits
 * \l commitStarted and \l commitCompleted signals to notify the start and the
 * completion of the commit operation.
 */
void UCBottomEdge::commit()
{
    Q_D(UCBottomEdge);
    d->setOperationStatus(UCBottomEdgePrivate::CommitToTop);
    Q_EMIT commitStarted();
    bool animated = d->bottomPanel && d->bottomPanel->m_panelAnimation;
    if (animated) {
        connect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                this, &UCBottomEdge::unlockOperation);
    }
    d->positionPanel(1.0);
    if (!animated) {
        unlockOperation(false);
    }
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
    d->setOperationStatus(UCBottomEdgePrivate::Collapsing);
    Q_EMIT collapseStarted();
    bool animated = d->bottomPanel && d->bottomPanel->m_panelAnimation;
    if (animated) {
        connect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                this, &UCBottomEdge::unlockOperation);
    }
    d->positionPanel(0.0);
    if (!animated) {
        unlockOperation(false);
    }
}

// yet internal, commits to the top of the range
void UCBottomEdge::commitToRange(UCBottomEdgeRange *range)
{
    Q_D(UCBottomEdge);
    if (!range || d->isLocked()) {
        return;
    }

    bool animated = d->bottomPanel && d->bottomPanel->m_panelAnimation;
    if (animated) {
        QObject::connect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                         this, &UCBottomEdge::unlockOperation);
    }
    d->setOperationStatus(UCBottomEdgePrivate::CommitToTop);
    d->positionPanel(range->m_to);
    if (!animated) {
        unlockOperation(false);
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
        d->setState(UCBottomEdge::Committed);
        Q_EMIT commitCompleted();
        break;
    case UCBottomEdgePrivate::Collapsing:
        d->setState(UCBottomEdge::Hidden);
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

/*!
 * \qmlproperty list<BottomEdgeRange> BottomEdge::ranges
 * The property holds the custom ranges configured for the BottomEdge. The default
 * configuration contains one range, which commits the content when reached. The
 * defaults can be restored by setting an empty list to the property or by
 * calling ranges.clear().
 * See \l BottomEdgeRange.
 */
QQmlListProperty<UCBottomEdgeRange> UCBottomEdge::ranges()
{
    Q_D(UCBottomEdge);
    return QQmlListProperty<UCBottomEdgeRange>(this, &d->ranges,
                                                 ranges_append,
                                                 ranges_count,
                                                 ranges_at,
                                                 ranges_clear);
}

void UCBottomEdge::ranges_append(QQmlListProperty<UCBottomEdgeRange> *ranges, UCBottomEdgeRange *range)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(ranges->object));
    bottomEdge->appendRange(range);
}

int UCBottomEdge::ranges_count(QQmlListProperty<UCBottomEdgeRange> *ranges)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(ranges->object));
    return bottomEdge->ranges.size();
}

UCBottomEdgeRange *UCBottomEdge::ranges_at(QQmlListProperty<UCBottomEdgeRange> *ranges, int index)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(ranges->object));
    Q_ASSERT((index >= 0) && (index < bottomEdge->ranges.size()));
    return bottomEdge->ranges.at(index);
}

void UCBottomEdge::ranges_clear(QQmlListProperty<UCBottomEdgeRange> *ranges)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(ranges->object));
    bottomEdge->clearRanges(true);
}

/*!
 * \qmlproperty BottomEdgeRange BottomEdge::activeRange
 * \readonly
 * Specifies the current active range.
 */
UCBottomEdgeRange *UCBottomEdge::activeRange()
{
    Q_D(UCBottomEdge);
    return d->activeRange;
}

#include "moc_ucbottomedge.cpp"
