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

#ifndef UCBOTTOMEDGE_H
#define UCBOTTOMEDGE_H

#include <QtQuick/QQuickItem>
#include <QtCore/QPointer>

class QQuickFlickable;
class UCBottomEdge : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(QQuickItem* hint MEMBER m_hint WRITE setHint NOTIFY hintChanged FINAL)
    Q_PROPERTY(qreal dragProgress READ dragProgress NOTIFY dragProggressChanged FINAL)
    Q_PROPERTY(QList<qreal> stages MEMBER m_stages NOTIFY stagesChanged FINAL)
    Q_PROPERTY(int currentStageIndex MEMBER m_currentStageIndex NOTIFY currentStageIndexChanged FINAL)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged FINAL)
    Q_PROPERTY(QUrl content MEMBER m_contentUrl WRITE setContent NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent *contentComponent MEMBER m_contentComponent WRITE setContentComponent NOTIFY contentComponentChanged FINAL)
    Q_PROPERTY(QQuickItem* contentItem READ contentItem NOTIFY contentItemChanged FINAL)
public:
    enum Status {
        Hidden,
        Idle,
        Active,
        Revealed,
        CanCommit,
        Committed
    };
    explicit UCBottomEdge(QQuickItem *parent = 0);
    virtual ~UCBottomEdge();

    void setHint(QQuickItem *hint);
    qreal dragProgress();
    QList<qreal> stages();
    Status status() const
    {
        return m_status;
    }
    void setContent(const QUrl &url);
    void setContentComponent(QQmlComponent *component);
    QQuickItem *contentItem() const
    {
        return m_contentItem;
    }

Q_SIGNALS:
    void hintChanged();
    void dragProggressChanged();
    void stagesChanged();
    void currentStageIndexChanged(int index);
    void statusChanged(Status status);
    void contentChanged(const QUrl url);
    void contentComponentChanged(QQmlComponent *component);
    void contentItemChanged(QQuickItem *item);

    void commitStarted();
    void commitFinished();
    void collapseStarted();
    void collapseFinished();

public Q_SLOTS:
    void commit();
    void collapse();

protected:
    void classBegin();
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);

    void setStatus(UCBottomEdge::Status status);
    void setCurrentStageIndex(int index);
    void loadPanel();
    void createPanel(QQmlComponent *component);
    void anchorHintToPanel();
    void updateProgressionStates();

    QList<qreal> m_stages;
    QPointer<QQuickItem> m_panelItem;
    QUrl m_contentUrl;
    QQuickItem *m_hint;
    QQmlComponent *m_contentComponent;
    QQuickItem *m_contentItem;
    QQuickItem *m_bottomPanel;
    qreal m_defaultCommitStage;

    int m_currentStageIndex;
    Status m_status;
};

#endif // UCBOTTOMEDGE_H
