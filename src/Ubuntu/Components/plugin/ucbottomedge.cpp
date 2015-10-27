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
#include "quickutils.h"
#include <QtQml/QQmlEngine>
#include <QtGui/QScreen>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>

#include "plugin.h"

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
 * is typically composed of a hint and some content. The content is typically
 * committed (i.e. fully shown) when the drag is completed after it has been
 * dragged for a certain amount. The content can be anything, defined by
 * \l content or \l contentComponent.
 *
 * As the name suggests, the component automatically anchors to the bottom of its
 * parent and takes the width of the parent. The drag is detected within the parent
 * area, but the content can be shown either full window or within the parent area -
 * see \l fillWindow property.
 * The height of the BottomEdge specifies how much of the window or parent area
 * should the exposed content ocupy. The content is centered into a panel which
 * is dragged from the bottom of the parent component.
 *
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
 * \note \l content, \l contentComponent and \l contentItem is equovalent to
 * Loader's \e source, \e sourceComponent and \e item properties. See Loader
 * documentation for further details.
 *
 * There can be situations when the there should be different contents shown
 * depending on the progress of the drag or when different stages of the drag
 * is reacted. This can be achieved by tracking the \l dragProgress.
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
 *
 *
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
 * Signal emitted when teh content collapse is completed.
 */

UCBottomEdge::UCBottomEdge(QQuickItem *parent)
    : QQuickItem(parent)
    , m_hint(Q_NULLPTR)
    , m_contentComponent(Q_NULLPTR)
    , m_bottomPanel(Q_NULLPTR)
    , m_defaultCommitStage(0.33) // 30% of the swipable area
    , m_state(Hidden)
    , m_fillWindow(false)
{
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
        } else {
            anchors->resetLeft();
            anchors->resetRight();
            anchors->resetBottom();
        }
    }
    QQuickItem::itemChange(change, data);
}

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

void UCBottomEdge::createPanel(QQmlComponent *component)
{
    QQmlContext *context = new QQmlContext(qmlContext(this));
    context->setContextProperty("bottomEdge", this);
    m_bottomPanel = static_cast<QQuickItem*>(component->beginCreate(context));
    Q_ASSERT(m_bottomPanel);
    QQml_setParent_noEvent(m_bottomPanel, this);
    if (m_fillWindow) {
        m_bottomPanel->setParentItem(QuickUtils::instance().rootObject());
    } else {
        m_bottomPanel->setParentItem(parentItem());
    }
    m_panelItem = m_bottomPanel->property("panelItem").value<QQuickItem*>();
    m_loader = m_bottomPanel->property("contentLoader").value<QQuickItem*>();
    component->completeCreate();
    component->deleteLater();
    // anchor hint to panel
    anchorHintToPanel();

    connect(m_loader, SIGNAL(itemChanged()), this, SIGNAL(contentItemChanged()), Qt::DirectConnection);
    connect(m_panelItem, &QQuickItem::yChanged, this, &UCBottomEdge::dragProggressChanged);
    // follow drag progress to detect when can we set to CanCommit status
    connect(this, &UCBottomEdge::dragProggressChanged, [=]() {
        this->updateProgressionStates();
    });    
}

void UCBottomEdge::anchorHintToPanel()
{
    if (m_panelItem && m_hint) {
        m_hint->setParentItem(m_panelItem.data());
        QQuickAnchors *anchors = QQuickItemPrivate::get(m_hint)->anchors();
        anchors->setBottom(QQuickItemPrivate::get(m_panelItem)->top());
    } else if (m_hint) {
        m_hint->setParentItem(this);
    }
}

void UCBottomEdge::updateProgressionStates()
{
    qreal progress = dragProgress();

    if (progress <= 0.0) {
        setState(UCBottomEdge::Hidden);
    } else if (progress < m_defaultCommitStage) {
        setState(UCBottomEdge::Revealed);
    } else if (progress < 1.0) {
        setState(UCBottomEdge::CanCommit);
        if (m_hint) {
            m_hint->setState("Active");
        }
    } else {
        setState(UCBottomEdge::Committed);
        if (m_hint) {
            m_hint->setState("Locked");
        }
    }
}

/*!
 * \qmlproperty Item BottomEdge::hint
 * The property holds the component to display the hint for the bottom edge element.
 * The component can be a BottomEdgeHint or any custom component. In case of custom
 * component is used, the component must make sure it implements the "Active" state.
 * BottomEdge automatically takes ownership over the hint set, and will delete the
 * previously set component upon change.
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
        // anchor hint to the panel
        anchorHintToPanel();
        // connect state change from the hint to keep it in sync
        // we can use lambda as the changed hint component will be deleted
        // so the connection will be removed as well
        connect(m_hint, &QQuickItem::stateChanged, [=](const QString &state) {
            if (state == QStringLiteral("Active")) {
//                setState(UCBottomEdge::Active);
            }
        });
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
    Q_ASSERT(m_panelItem);
    return 1.0 - (m_panelItem->y() / height());
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
 *  \li The bottom edge reached the stage from where it can commit.
 * \row
 *  \li Committed
 *  \li The bottom edge content is fully exposed.
 * \endtable
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
 * \qmlproperty bool BottomEdge::fillWindow
 * The property specifies whether the bottom edge content should be rendered
 * within the whole window or only the parent area. Defaults to \e false.
 */
void UCBottomEdge::setFillWindow(bool fill)
{
    if (m_fillWindow == fill) {
        return;
    }
    m_fillWindow = fill;
    if (m_bottomPanel && m_state == Hidden) {
        if (m_fillWindow) {
            m_bottomPanel->setParentItem(QuickUtils::instance().rootObject());
        } else {
            m_bottomPanel->setParentItem(this);
        }
    }
    Q_EMIT fillWindowChanged();
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
    m_bottomPanel->setState("Committing");
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
    m_bottomPanel->setState("Collapsing");
}
