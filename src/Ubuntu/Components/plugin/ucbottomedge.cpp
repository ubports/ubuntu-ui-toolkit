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
#include "ucbottomedgesection.h"
#include <QtQml/QQmlEngine>
#include <QtGui/QScreen>
#include <QtQml/QQmlProperty>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>

#include "plugin.h"
#include <QtQuick/private/qquickanimation_p.h>

/*!
 * \qmltype BottomEdge
 * \instantiates UCBottomEdge
 * \inqmlmodule Ubuntu.Components 1.3
 * \inherits Item
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
    : QQuickItem(parent)
    , m_hint(Q_NULLPTR)
    , m_contentComponent(Q_NULLPTR)
    , m_bottomPanel(Q_NULLPTR)
    , m_commitPoint(1.0)
    , m_state(Hidden)
    , m_defaultSectionsReset(false)
{
    // create default stages
    createDefaultSections();
}
UCBottomEdge::~UCBottomEdge()
{
}

void UCBottomEdge::classBegin()
{
    QQuickItem::classBegin();
    loadPanel();
}

void UCBottomEdge::componentComplete()
{
    QQuickItem::componentComplete();
    // if we have content, we load the panel
    loadPanel();
}

void UCBottomEdge::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemVisibleHasChanged) {
    }
    if (change == ItemParentHasChanged) {
        QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
        if (data.item) {
            const QQuickAnchorLine left = QQuickItemPrivate::get(data.item)->left();
            const QQuickAnchorLine right = QQuickItemPrivate::get(data.item)->right();
            const QQuickAnchorLine bottom = QQuickItemPrivate::get(data.item)->bottom();
            anchors->setLeft(left);
            anchors->setRight(right);
            anchors->setBottom(bottom);
            QQuickItemPrivate::get(data.item)->addItemChangeListener(this, QQuickItemPrivate::Children);
        } else {
            anchors->resetLeft();
            anchors->resetRight();
            anchors->resetBottom();
        }
    }
    QQuickItem::itemChange(change, data);
}

// make sure the bottom edge panel is always the last one
void UCBottomEdge::itemChildAdded(QQuickItem *item, QQuickItem *)
{
    // make sure the BottomEdge's panel is the last one
    QQuickItem *last = item->childItems().last();
    if (m_bottomPanel && last != m_bottomPanel) {
        m_bottomPanel->stackAfter(last);
    }
}

// remove this to be change listener from the previous parent
void UCBottomEdge::itemChildRemoved(QQuickItem *item, QQuickItem *child)
{
    if (child == this) {
        QQuickItemPrivate::get(item)->removeItemChangeListener(this, QQuickItemPrivate::Children);
    }
}

// loads the panel asynchronously
void UCBottomEdge::loadPanel()
{
    if (!m_bottomPanel && (m_contentComponent || m_contentUrl.isValid())) {
        QUrl url(UbuntuComponentsPlugin::pluginUrl().resolved(QUrl("1.3/BottomEdgePanel.qml")));
        QQmlComponent *component = new QQmlComponent(qmlEngine(this), url, QQmlComponent::Asynchronous, this);
        if (component->isLoading()) {
            connect(component, &QQmlComponent::statusChanged, [=](QQmlComponent::Status status) {
                switch (status) {
                case QQmlComponent::Ready: {
                    this->createPanel(component);
                    break;
                }
                case QQmlComponent::Error: {
                    QString msg = component->errorString();
                    delete component;
                    qCritical() << qPrintable(msg);
                    break;
                }
                default: break;
                }
            });
        } else if (component->isError()) {
            QString msg = component->errorString();
            delete component;
            qCritical() << qPrintable(msg);
        } else {
            createPanel(component);
        }
    }
}

// creates the panel component
void UCBottomEdge::createPanel(QQmlComponent *component)
{
    QQmlContext *context = new QQmlContext(qmlContext(this));
    context->setContextProperty("bottomEdge", this);
    context->setContextObject(this);
    m_bottomPanel = static_cast<QQuickItem*>(component->beginCreate(context));
    Q_ASSERT(m_bottomPanel);
    QQml_setParent_noEvent(m_bottomPanel, this);
    // at this point this will be the last child, so no need to re-stack
    m_bottomPanel->setParentItem(parentItem());
    m_panelItem = m_bottomPanel->property("panelItem").value<QQuickItem*>();
    m_loader = m_bottomPanel->property("contentLoader").value<QQuickItem*>();
    component->completeCreate();
    component->deleteLater();
    // anchor hint to panel
    if (m_hint) {
        m_hint->setParentItem(this);
    }

    connect(m_loader, SIGNAL(itemChanged()), this, SIGNAL(contentItemChanged()), Qt::DirectConnection);
    connect(m_panelItem, &QQuickItem::yChanged, this, &UCBottomEdge::dragProggressChanged);
    // follow drag progress to detect when can we set to CanCommit status
    connect(this, &UCBottomEdge::dragProggressChanged, &UCBottomEdge::updateProgressionStates);
    m_panelAnimation = m_bottomPanel->property("panelAnimation").value<QQuickAbstractAnimation*>();
}

// attach hint instance to the bottom edge panel holding the content
// update state and sections during drag
void UCBottomEdge::updateProgressionStates()
{
    if (m_state >= SectionCommitted) {
        return;
    }
    qreal progress = dragProgress();
    if (progress > 0.0 && !m_lastSection) {
        setState(Revealed);
    }

    // go through the stages
    Q_FOREACH(UCBottomEdgeSection *section, m_sections) {
        if (section->dragInSection(progress)) {
            if (!m_lastSection) {
                section->enterSection();
                m_lastSection = section;
                Q_EMIT currentSectionChanged();
                break;
            }
        } else if (m_lastSection == section) {
            section->exitSection();
            m_lastSection.clear();
            Q_EMIT currentSectionChanged();
        }
    }
}

// creates the default section(s)
void UCBottomEdge::createDefaultSections()
{
    // add the default stages
    UCBottomEdgeSection *commitStage = new UCBottomEdgeSection(this);
    // for testing purposes
    commitStage->setObjectName("default_bottomedge_stage");
    // enters in this stage when drag ratio reaches 30% of the area
    commitStage->m_startsAt = 0.33;
    commitStage->m_endsAt = 1.0;

    m_sections.append(commitStage);
}

// positions the bottom edge panel holding the content to the given position
// position is a percentage of the height
void UCBottomEdge::positionPanel(qreal position)
{
    // use QQmlProperty so the bindings on y are broken,
    // otherwise AdaptivePageLayout relayouting makes bottom
    // edge to collapse
    // also makes Behavior to run on the property
    QQmlProperty::write(m_panelItem, "y", height() - height() * position, qmlContext(m_panelItem));
    if (position < 1.0 && m_state >= Revealed) {
        setState(SectionCommitted);
    }
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
void UCBottomEdge::setHint(QQuickItem *hint)
{
    if (hint == m_hint || m_state >= Revealed) {
        return;
    }
    if (m_hint) {
        m_hint->setParentItem(Q_NULLPTR);
        delete m_hint;
        m_hint = Q_NULLPTR;
    }
    m_hint = hint;
    // take ownership
    if (m_hint) {
        QQmlEngine::setObjectOwnership(m_hint, QQmlEngine::CppOwnership);
        QQml_setParent_noEvent(m_hint, this);
        m_hint->setParentItem(this);
        if (m_hint->metaObject()->indexOfSignal("clicked()") >= 0) {
            connect(m_hint, SIGNAL(clicked()), this, SLOT(commit()), Qt::DirectConnection);
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
    return !m_panelItem ? 0.0 : 1.0 - (m_panelItem->y() / height());
}

/*!
 * \qmlproperty enum BottomEdge::state
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
void UCBottomEdge::setState(UCBottomEdge::State state)
{
    if (state == m_state) {
        return;
    }
    m_state = state;
    QString stateStr;
    switch (state) {
        case Hidden: stateStr = "Hidden"; break;
        case Revealed: stateStr = "Revealed"; break;
        case CanCommit: stateStr = "CanCommit"; break;
        case SectionCommitted: stateStr = "SectionCommitted"; break;
        case Committed: stateStr = "Committed"; break;
    }

    qDebug() << "STATE" << stateStr;
    Q_EMIT stateChanged(m_state);
}

/*!
 * \qmlproperty url BottomEdge::content
 * The property holds the url to the document defining the content of the bottom
 * edge. The property behaves the same way as Loader's \e source property.
 */
void UCBottomEdge::setContent(const QUrl &url)
{
    if (m_contentUrl == url) {
        return;
    }

    m_contentUrl = url;
    Q_EMIT contentChanged(m_contentUrl);
}

/*!
 * \qmlproperty Component BottomEdge::contentComponent
 * The property holds the component defining the content of the bottom edge. The
 * property behaves the same way as Loader's \e sourceComponent property.
 */
void UCBottomEdge::setContentComponent(QQmlComponent *component)
{
    if (m_contentComponent == component) {
        return;
    }
    m_contentComponent = component;
    Q_EMIT contentComponentChanged(m_contentComponent);
}

/*!
 * \qmlproperty Item BottomEdge::contentItem
 * \readonly
 * The property holds the item created either from \l content or \l contentComponent
 * properties.
 */
QQuickItem *UCBottomEdge::contentItem() const
{
    return m_loader ? m_loader->property("item").value<QQuickItem*>() : Q_NULLPTR;
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
    if (m_panelAnimation) {
        connect(m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                this, &UCBottomEdge::emitCommitCompleted);
    }
    positionPanel(m_commitPoint);
}

void UCBottomEdge::emitCommitCompleted(bool running)
{
    if (running) {
        return;
    }
    disconnect(m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
               this, &UCBottomEdge::emitCommitCompleted);
    setState(Committed);
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
    if (m_panelAnimation) {
        connect(m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
                this, &UCBottomEdge::emitCollapseCompleted);
    }
    positionPanel(0.0);
}

void UCBottomEdge::emitCollapseCompleted(bool running)
{
    if (running) {
        return;
    }
    disconnect(m_panelAnimation, &QQuickAbstractAnimation::runningChanged,
               this, &UCBottomEdge::emitCollapseCompleted);
    setState(Hidden);
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
    return QQmlListProperty<UCBottomEdgeSection>(this, &m_sections,
                                                 sections_append,
                                                 Q_NULLPTR,
                                                 Q_NULLPTR,
                                                 sections_clear);
}

void UCBottomEdge::sections_append(QQmlListProperty<UCBottomEdgeSection> *sections, UCBottomEdgeSection *section)
{
    UCBottomEdge *bottomEdge = static_cast<UCBottomEdge*>(sections->object);
    if (!bottomEdge->m_defaultSectionsReset) {
        bottomEdge->m_defaultSectionsReset = true;
        qDeleteAll(bottomEdge->m_sections);
        bottomEdge->m_sections.clear();
    }
    bottomEdge->m_sections.append(section);
    // take ownership!
    QQmlEngine::setObjectOwnership(section, QQmlEngine::CppOwnership);
    QQml_setParent_noEvent(section, bottomEdge);
    section->m_bottomEdge = bottomEdge;
    // adjust endsAt property value if not set yet
    if (section->m_endsAt <= 0.0) {
        section->m_endsAt = bottomEdge->m_commitPoint;
        Q_EMIT section->endsAtChanged();
    }
}

void UCBottomEdge::sections_clear(QQmlListProperty<UCBottomEdgeSection> *sections)
{
    UCBottomEdge *bottomEdge = static_cast<UCBottomEdge*>(sections->object);
    if (!bottomEdge->m_defaultSectionsReset) {
        return;
    }
    qDeleteAll(bottomEdge->m_sections);
    bottomEdge->m_sections.clear();
    bottomEdge->m_defaultSectionsReset = false;
    bottomEdge->createDefaultSections();
}

/*!
 * \qmlproperty BottomEdgeSection BottomEdge::currentSection
 * \readonly
 * Specifies the current active section.
 */
UCBottomEdgeSection *UCBottomEdge::currentSection()
{
    return m_lastSection.data();
}

/*!
 * \qmlproperty real BottomEdge::commitPoint
 * Specifies the ratio the bottom edge content should be committed to. Same as
 * \l dragProgress, the value can be set between 0 and 1. Defaults to 1.
 */
void UCBottomEdge::setCommitPoint(qreal point)
{
    if (point == m_commitPoint || (point < 0.0 || point > 1.0)) {
        return;
    }
    m_commitPoint = point;
    Q_EMIT commitPointChanged();
}
