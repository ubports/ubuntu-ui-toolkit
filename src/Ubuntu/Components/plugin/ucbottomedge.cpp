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
#include "ucbottomedgesection.h"
#include "ucstyleditembase_p.h"
#include <QtQml/QQmlEngine>
#include <QtGui/QScreen>
#include <QtQml/QQmlProperty>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>

#include "plugin.h"
#include "ucnamespace.h"
#include <QtQuick/private/qquickanimation_p.h>

UCBottomEdgePrivate::UCBottomEdgePrivate()
    : UCStyledItemBasePrivate()
    , hint(Q_NULLPTR)
    , contentComponent(Q_NULLPTR)
    , bottomPanel(Q_NULLPTR)
    , commitPoint(1.0)
    , state(UCBottomEdge::Hidden)
    , defaultSectionsReset(false)
{
}

void UCBottomEdgePrivate::init()
{
    setStyleName("BottomEdgeStyle");
    // create default stages
    createDefaultSections();
}

// creates the default section(s)
void UCBottomEdgePrivate::createDefaultSections()
{
    Q_Q(UCBottomEdge);
    // add the default stages
    UCBottomEdgeSection *commitStage = new UCBottomEdgeSection(q);
    // for testing purposes
    commitStage->setObjectName("default_bottomedge_stage");
    // enters in this stage when drag ratio reaches 30% of the area
    commitStage->m_startsAt = 0.33;
    commitStage->m_endsAt = 1.0;

    sections.append(commitStage);
}

// attach hint instance to the bottom edge panel holding the content
// update state and sections during drag
void UCBottomEdgePrivate::updateProgressionStates()
{
    if (state >= UCBottomEdge::SectionCommitted) {
        return;
    }
    Q_Q(UCBottomEdge);
    qreal progress = q->dragProgress();
    if (progress > 0.0 && !lastSection) {
        setState(UCBottomEdge::Revealed);
    }

    // go through the stages
    Q_FOREACH(UCBottomEdgeSection *section, sections) {
        if (section->dragInSection(progress)) {
            if (!lastSection) {
                section->enterSection();
                lastSection = section;
                Q_EMIT q->currentSectionChanged();
                break;
            }
        } else if (lastSection == section) {
            section->exitSection();
            lastSection.clear();
            Q_EMIT q->currentSectionChanged();
        }
    }
}

// positions the bottom edge panel holding the content to the given position
// position is a percentage of the height
void UCBottomEdgePrivate::positionPanel(qreal position)
{
    // use QQmlProperty so the bindings on y are broken,
    // otherwise AdaptivePageLayout relayouting makes bottom
    // edge to collapse
    // also makes Behavior to run on the property
    Q_Q(UCBottomEdge);
    qreal height = q->height();
    QQmlProperty::write(bottomPanel->m_panel, "y", height - height * position, qmlContext(bottomPanel->m_panel));
    if (position < 1.0 && state >= UCBottomEdge::Revealed) {
        setState(UCBottomEdge::SectionCommitted);
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
        QQuickItem *newParent = parentItem ? parentItem->parentItem() : q;
        bottomPanel->setParentItem(newParent);
        // bring style item in front
        bottomPanel->setZ(0);

        // connect style stuff
        QObject::connect(bottomPanel, &UCBottomEdgeStyle::contentItemChanged,
                         q, &UCBottomEdge::contentItemChanged, Qt::DirectConnection);
        QObject::connect(bottomPanel->m_panel, &QQuickItem::yChanged,
                         q, &UCBottomEdge::dragProggressChanged, Qt::DirectConnection);
        // follow drag progress to detect when can we set to CanCommit status
        QObject::connect(q, &UCBottomEdge::dragProggressChanged, [this]() {
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
 * the bottom of the BottomEdge.
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
 *             height: parent.height - units.gu(20)
 *             hint: BottomEdgeHint {
 *                 text: "My bottom edge"
 *             }
 *             contentComponent: Rectangle {
 *                 anchors.fill: parent
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
 * There can be situations when the there should be different content shown
 * depending on the progress of the drag or when the drag is in different
 * sections of the area. The first can be achieved by tracking the \l dragProgress.
 * \qml
 * BottomEdge {
 *     id: bottomEdge
 *     height: parent.height
 *     hint: BottomEdgeHint {
 *         text: "progression"
 *     }
 *     contentComponent: Rectangle {
 *         anchors.fill: parent
 *         color: Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1);
 *     }
 * }
 * \endqml
 * There can be situations when the content may need to be sligtly different on
 * certain sections of the area. These sections can be defined through BottomEdgeSection
 * elements listed in the \l sections property.
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
 *             hint: BottomEdgeHint {
 *                 text: "My bottom edge"
 *             }
 *             contentComponent: Rectangle {
 *                 anchors.fill: parent
 *                 color: bottomEdge.currentSection ?
 *                          bottomEdge.currentSection.color : UbuntuColors.green
 *             }
 *             BottomEdgeSection {
 *                 startsAt: 0.4
 *                 endsAt: 0.6
 *                 property color color: UbuntuColors.red
 *             }
 *             BottomEdgeSection {
 *                 startsAt: 0.6
 *                 endsAt: commitPoint
 *                 property color color: UbuntuColors.lightGrey
 *             }
 *         }
 *     }
 * }
 * \endqml
 * BottomEdgeSection can also replace the complete bottom edge content if needed.
 * \sa BottomEdgeSection
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
 * \qmlproperty Item BottomEdge::hint
 * The property holds the component to display the hint for the bottom edge element.
 * The component can be a BottomEdgeHint or any custom component. In case of custom
 * component is used, the component must make sure it implements the "Active" and
 * "Locked" states. BottomEdge automatically takes ownership over the hint set, and
 * will delete the previously set component upon change.
 * \note Changing the hint during an ongoing bottom edge swipe is omitted.
 */
QQuickItem *UCBottomEdge::hint() const
{
    Q_D(const UCBottomEdge);
    return d->hint;
}
void UCBottomEdge::setHint(QQuickItem *hint)
{
    Q_D(UCBottomEdge);
    if (hint == d->hint || d->state >= Revealed) {
        return;
    }
    if (d->hint) {
        d->hint->setParentItem(Q_NULLPTR);
        delete d->hint;
        d->hint = Q_NULLPTR;
    }
    d->hint = hint;
    // take ownership
    if (d->hint) {
        QQmlEngine::setObjectOwnership(d->hint, QQmlEngine::CppOwnership);
        QQml_setParent_noEvent(d->hint, this);
        d->hint->setParentItem(this);
        if (d->hint->metaObject()->indexOfSignal("clicked()") >= 0) {
            connect(d->hint, SIGNAL(clicked()), this, SLOT(commit()), Qt::DirectConnection);
        }
    }
    Q_EMIT hintChanged();
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
 *  \li CanCommit
 *  \li The bottom edge reached the stage from where it can commit. If there are
 *      sections specified, the section having \l BottomEdgeSection::endsAt set
 *      to the \l commitPoint will turn the state on.
 * \row
 *  \li SectionCommitted
 *  \li The bottom edge content is exposed till the top of the current section.
 * \row
 *  \li Committed
 *  \li The bottom edge content is fully exposed.
 * \endtable
 * \note Once \e SectionCommitted and \e Commited states are set, no further draging
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
        case UCBottomEdge::CanCommit: stateStr = "CanCommit"; break;
        case UCBottomEdge::SectionCommitted: stateStr = "SectionCommitted"; break;
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
    Q_EMIT commitStarted();
    Q_D(UCBottomEdge);
    if (d->bottomPanel && d->bottomPanel->m_panelAnimation) {
        connect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                this, &UCBottomEdge::emitCommitCompleted);
    }
    d->positionPanel(d->commitPoint);
}

void UCBottomEdge::emitCommitCompleted(bool running)
{
    if (running) {
        return;
    }
    Q_D(UCBottomEdge);
    disconnect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
               this, &UCBottomEdge::emitCommitCompleted);
    d->setState(Committed);
    Q_EMIT commitCompleted();
}

/*!
 * \qmlmethod void BottomEdge::collapse()
 * The function forces the bottom edge content to be hidden. Emits \l collapseStarted
 * and \l collapseCompleted signals to notify the start and the completion of the
 * collapse operation.
 */
void UCBottomEdge::collapse()
{
    Q_EMIT collapseStarted();
    Q_D(UCBottomEdge);
    if (d->bottomPanel && d->bottomPanel->m_panelAnimation) {
        connect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                this, &UCBottomEdge::emitCollapseCompleted);
    }
    d->positionPanel(0.0);
}

void UCBottomEdge::emitCollapseCompleted(bool running)
{
    if (running) {
        return;
    }
    Q_D(UCBottomEdge);
    disconnect(d->bottomPanel->m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
               this, &UCBottomEdge::emitCollapseCompleted);
    d->setState(Hidden);
    Q_EMIT collapseCompleted();
}

/*!
 * \qmlproperty list<BottomEdgeSection> BottomEdge::sections
 * \default
 * The property holds the custom sections configured for the BottomEdge. The
 * defaults can be restored by setting an empty list to the property or by
 * calling sections.clear().
 * See \l BottomEdgeSection.
 */
QQmlListProperty<UCBottomEdgeSection> UCBottomEdge::sections()
{
    Q_D(UCBottomEdge);
    return QQmlListProperty<UCBottomEdgeSection>(this, &d->sections,
                                                 UCBottomEdgePrivate::sections_append,
                                                 Q_NULLPTR,
                                                 Q_NULLPTR,
                                                 UCBottomEdgePrivate::sections_clear);
}

void UCBottomEdgePrivate::sections_append(QQmlListProperty<UCBottomEdgeSection> *sections, UCBottomEdgeSection *section)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(sections->object));
    if (!bottomEdge->defaultSectionsReset) {
        bottomEdge->defaultSectionsReset = true;
        qDeleteAll(bottomEdge->sections);
        bottomEdge->sections.clear();
    }
    bottomEdge->sections.append(section);
    // take ownership!
    QQmlEngine::setObjectOwnership(section, QQmlEngine::CppOwnership);
    QQml_setParent_noEvent(section, sections->object);
    section->m_bottomEdge = static_cast<UCBottomEdge*>(sections->object);
    // adjust endsAt property value if not set yet
    if (section->m_endsAt <= 0.0) {
        section->m_endsAt = bottomEdge->commitPoint;
        Q_EMIT section->endsAtChanged();
    }
}

void UCBottomEdgePrivate::sections_clear(QQmlListProperty<UCBottomEdgeSection> *sections)
{
    UCBottomEdgePrivate *bottomEdge = UCBottomEdgePrivate::get(static_cast<UCBottomEdge*>(sections->object));
    if (!bottomEdge->defaultSectionsReset) {
        return;
    }
    qDeleteAll(bottomEdge->sections);
    bottomEdge->sections.clear();
    bottomEdge->defaultSectionsReset = false;
    bottomEdge->createDefaultSections();
}

/*!
 * \qmlproperty BottomEdgeSection BottomEdge::currentSection
 * \readonly
 * Specifies the current active section.
 */
UCBottomEdgeSection *UCBottomEdge::currentSection()
{
    Q_D(UCBottomEdge);
    return d->lastSection.data();
}

/*!
 * \qmlproperty real BottomEdge::commitPoint
 * Specifies the ratio the bottom edge content should be committed to. Same as
 * \l dragProgress, the value can be set between 0 and 1. Defaults to 1.
 */
qreal UCBottomEdge::commitPoint() const
{
    Q_D(const UCBottomEdge);
    return d->commitPoint;
}
void UCBottomEdge::setCommitPoint(qreal point)
{
    Q_D(UCBottomEdge);
    if (point == d->commitPoint || (point < 0.0 || point > 1.0)) {
        return;
    }
    d->commitPoint = point;
    Q_EMIT commitPointChanged();
}

#include "moc_ucbottomedge.cpp"
