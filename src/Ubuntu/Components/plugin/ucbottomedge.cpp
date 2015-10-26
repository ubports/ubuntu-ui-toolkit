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
 * dragged more than one third of the screen area. However there can be cases
 * when no content is required and/or the use case requires multiple steps before
 * the content can be fully shown.
 *
 *
 */

/*!
 * \qmlsignal BottomEdge::commitStarted()
 */

/*!
 * \qmlsignal BottomEdge::commitFinished()
 */

/*!
 * \qmlsignal BottomEdge::collapseStarted()
 */

/*!
 * \qmlsignal BottomEdge::collapseFinished()
 */

UCBottomEdge::UCBottomEdge(QQuickItem *parent)
    : QQuickItem(parent)
    , m_hint(Q_NULLPTR)
    , m_contentComponent(Q_NULLPTR)
    , m_contentItem(Q_NULLPTR)
    , m_bottomPanel(Q_NULLPTR)
    , m_defaultCommitStage(0.33) // 30% of the swipable area
    , m_currentStageIndex(-1)
    , m_status(Idle)
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
        QQmlComponent *component = new QQmlComponent(qmlEngine(this), url, QQmlComponent::Asynchronous);
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
//    m_bottomPanel->setParentItem(QuickUtils::instance().rootObject());
    m_bottomPanel->setParentItem(this);
    m_panelItem = m_bottomPanel->property("panelItem").value<QQuickItem*>();
    component->completeCreate();
    component->deleteLater();
    // anchor hint to panel
    anchorHintToPanel();

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
    qreal finalStage = (m_stages.size() > 0) ? m_stages.last() : m_defaultCommitStage;
    qreal progress = dragProgress();

    if (progress <= 0.0) {
        setStatus(UCBottomEdge::Hinted);
        setCurrentStageIndex(-1);
    } else if (progress < finalStage) {
        setStatus(UCBottomEdge::Revealed);
        // check stages and trigger index changes
        for (int i = m_stages.size() - 1; i >= 0; i--) {
            if (progress >= m_stages[i]) {
                setCurrentStageIndex(i);
                break;
            }
        }
    } else if (progress < 1.0) {
        setStatus(UCBottomEdge::CanCommit);
        if (m_hint) {
            m_hint->setState("Hinted");
        }
    } else {
        setStatus(UCBottomEdge::Committed);
        if (m_hint) {
            m_hint->setState("Locked");
        }
    }
}

/*!
 * \qmlproperty Item BottomEdge::hint
 * The property holds the component to display the hint for the bottom edge element.
 * The component can be a BottomEdgeHint or any custom component. In case of custom
 * component is used, the component must make sure it implements the states of the
 * BottomEdgeHint component, as that drives the first three \l status flags. BottomEdge
 * automatically takes ownership over the hint set, and will delete the previously set
 * component upon change.
 * \note Changing the hint during an ongoing bottom edge swipe (status set to \e Hinted
 * and onwards) is omitted.
 */
void UCBottomEdge::setHint(QQuickItem *hint)
{
    if (hint == m_hint || m_status >= Hinted) {
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
        // anchor hint to the panel
        anchorHintToPanel();
        // connect state change from the hint to keep it in sync
        // we can use lambda as the changed hint component will be deleted
        // so the connection will be removed as well
        connect(m_hint, &QQuickItem::stateChanged, [=](const QString &state) {
            if (state == QStringLiteral("Idle") || state.isEmpty()) {
                setStatus(UCBottomEdge::Idle);
            }
            if (state == QStringLiteral("Hinted")) {
                setStatus(UCBottomEdge::Hinted);
            }
            if (state == QStringLiteral("Faded")) {
                setStatus(UCBottomEdge::Faded);
            }
        });
    }
    Q_EMIT hintChanged();
}

/*!
 * \qmlproperty real BottomEdge::dragProgress
 * \readonly
 */
qreal UCBottomEdge::dragProgress()
{
    Q_ASSERT(m_panelItem);
    return 1.0 - (m_panelItem->y() / height());
}

/*!
 * \qmlproperty list<real> BottomEdge::stages
 */
QList<qreal> UCBottomEdge::stages()
{
    return m_stages;
}
/*!
 * \qmlproperty int BottomEdge::currentStageIndex
 */
void UCBottomEdge::setCurrentStageIndex(int index)
{
    if (m_currentStageIndex == index) {
        return;
    }
    m_currentStageIndex = index;
    Q_EMIT currentStageIndexChanged(index);
}

/*!
 * \qmlproperty enum BottomEdge::status
 * The property reports the actual status of the bottom edge. It can have the
 * following values:
 * \table
 * \header
 *  \li Status
 *  \li Description
 * \row
 *  \li Faded
 *  \li The bottom edge hint is faded out. \l flickable
 * \endtable
 */
void UCBottomEdge::setStatus(UCBottomEdge::Status status)
{
    if (status == m_status) {
        return;
    }
    // the first 3 statuses can be interchanged, after which the statuses are linearly
    // changerable, therefore cannot go back from CanCommit to Hinted and below straight
    // must go through Revealed and then Hinted and/or Idle.
    if (m_status > Revealed && status <= Hinted) {
        return;
    }
    m_status = status;
    QString statusStr;
    switch (status) {
        case Faded: statusStr = "Faded"; break;
        case Idle: statusStr = "Idle"; break;
        case Hinted: statusStr = "Hinted"; break;
        case Revealed: statusStr = "Revealed"; break;
        case CanCommit: statusStr = "CanCommit"; break;
        case Committed: statusStr = "Committed"; break;
    }

    qDebug() << "STATUS" << statusStr;
    Q_EMIT statusChanged(m_status);
}

/*!
 * \qmlproperty url BottomEdge::content
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
 */

/*!
 * \qmlmethod void BottomEdge::commit()
 */
void UCBottomEdge::commit()
{
    Q_EMIT commitStarted();
    m_bottomPanel->setState("Committing");
}

/*!
 * \qmlmethod void BottomEdge::collapse()
 */
void UCBottomEdge::collapse()
{
    Q_EMIT collapseStarted();
    m_bottomPanel->setState("Collapsing");
}
