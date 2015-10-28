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
#include <QtQuick/private/qquickitemchangelistener_p.h>

class UCBottomEdgeSection;
class QQuickAbstractAnimation;
class UCBottomEdge : public QQuickItem, protected QQuickItemChangeListener
{
    Q_OBJECT
    Q_ENUMS(State)

    Q_PROPERTY(QQuickItem* hint MEMBER m_hint WRITE setHint NOTIFY hintChanged FINAL)
    Q_PROPERTY(qreal dragProgress READ dragProgress NOTIFY dragProggressChanged FINAL)
    Q_PROPERTY(State state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(QUrl content MEMBER m_contentUrl WRITE setContent NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent *contentComponent MEMBER m_contentComponent WRITE setContentComponent NOTIFY contentComponentChanged FINAL)
    Q_PROPERTY(QQuickItem* contentItem READ contentItem NOTIFY contentItemChanged FINAL)
    Q_PROPERTY(bool fillWindow MEMBER m_fillWindow WRITE setFillWindow NOTIFY fillWindowChanged FINAL)
    Q_PROPERTY(QQmlListProperty<UCBottomEdgeSection> sections READ sections FINAL)
    Q_PROPERTY(UCBottomEdgeSection* currentSection READ currentSection NOTIFY currentSectionChanged FINAL)
    Q_PROPERTY(qreal commitPoint MEMBER m_commitPoint WRITE setCommitPoint NOTIFY commitPointChanged FINAL)
    Q_CLASSINFO("DefaultProperty", "sections")

public:
    enum State {
        Hidden,
        Revealed,
        CanCommit,
        Committed
    };
    explicit UCBottomEdge(QQuickItem *parent = 0);
    virtual ~UCBottomEdge();

    void setHint(QQuickItem *hint);
    qreal dragProgress();
    State state() const
    {
        return m_state;
    }
    void setState(UCBottomEdge::State state);
    void setContent(const QUrl &url);
    void setContentComponent(QQmlComponent *component);
    QQuickItem *contentItem() const;
    void setFillWindow(bool fill);
    QQmlListProperty<UCBottomEdgeSection> sections();
    UCBottomEdgeSection *currentSection();
    void setCommitPoint(qreal point);

Q_SIGNALS:
    void hintChanged();
    void dragProggressChanged();
    void stateChanged(State state);
    void contentChanged(const QUrl url);
    void contentComponentChanged(QQmlComponent *component);
    void contentItemChanged();
    void fillWindowChanged();
    void currentSectionChanged();
    void commitPointChanged();

    void commitStarted();
    void commitCompleted();
    void collapseStarted();
    void collapseCompleted();

public Q_SLOTS:
    void commit();
    void collapse();

protected:
    void classBegin();
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);
    void itemChildAdded(QQuickItem *item, QQuickItem *child);
    void itemChildRemoved(QQuickItem *item, QQuickItem *child);

    void loadPanel();
    void createPanel(QQmlComponent *component);
    void onPanelParentChildrenChanged();
    void anchorHintToPanel();
    void updateProgressionStates();
    void createDefaultSections();

    // panel positioning
    void positionPanel(qreal position);
    void emitCommitCompleted(bool running);
    void emitCollapseCompleted(bool running);

    static void sections_append(QQmlListProperty<UCBottomEdgeSection> *sections, UCBottomEdgeSection *section);
    static void sections_clear(QQmlListProperty<UCBottomEdgeSection> *sections);

    QList<UCBottomEdgeSection*> m_sections;
    QPointer<QQuickItem> m_panelItem;
    QPointer<QQuickItem> m_loader;
    QPointer<QQuickAbstractAnimation> m_panelAnimation;
    QPointer<UCBottomEdgeSection> m_lastSection;
    QUrl m_contentUrl;
    QQuickItem *m_hint;
    QQmlComponent *m_contentComponent;
    QQuickItem *m_bottomPanel;
    qreal m_commitPoint;
    State m_state;
    bool m_fillWindow:1;
    bool m_defaultSectionsReset:1;

    friend class UCBottomEdgeSection;
};

#endif // UCBOTTOMEDGE_H
