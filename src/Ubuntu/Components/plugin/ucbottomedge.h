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

#include "ucstyleditembase.h"
#include <QtCore/QPointer>
#include <QtQuick/private/qquickitemchangelistener_p.h>

class UCBottomEdgeSection;
class QQuickAbstractAnimation;
class UCBottomEdgePrivate;
class UCBottomEdge : public UCStyledItemBase
{
    Q_OBJECT
    Q_ENUMS(State)

    Q_PROPERTY(QQuickItem* hint READ hint WRITE setHint NOTIFY hintChanged FINAL)
    Q_PROPERTY(qreal dragProgress READ dragProgress NOTIFY dragProggressChanged FINAL)
    Q_PROPERTY(State state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(QUrl content READ content WRITE setContent NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent *contentComponent READ contentComponent WRITE setContentComponent NOTIFY contentComponentChanged FINAL)
    Q_PROPERTY(QQuickItem* contentItem READ contentItem NOTIFY contentItemChanged FINAL)
    Q_PROPERTY(QQmlListProperty<UCBottomEdgeSection> sections READ sections FINAL)
    Q_PROPERTY(UCBottomEdgeSection* currentSection READ currentSection NOTIFY currentSectionChanged FINAL)
    Q_PROPERTY(qreal commitPoint READ commitPoint WRITE setCommitPoint NOTIFY commitPointChanged FINAL)
    Q_CLASSINFO("DefaultProperty", "sections")

public:
    enum State {
        Hidden,
        Revealed,
        CanCommit,
        SectionCommitted,
        Committed
    };
    explicit UCBottomEdge(QQuickItem *parent = 0);
    virtual ~UCBottomEdge();

    QQuickItem *hint() const;
    void setHint(QQuickItem *hint);
    qreal dragProgress();
    State state() const;
    QUrl content() const;
    void setContent(const QUrl &url);
    QQmlComponent *contentComponent() const;
    void setContentComponent(QQmlComponent *component);
    QQuickItem *contentItem() const;
    void setFillWindow(bool fill);
    QQmlListProperty<UCBottomEdgeSection> sections();
    UCBottomEdgeSection *currentSection();
    qreal commitPoint() const;
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
    void itemChange(ItemChange change, const ItemChangeData &data);

    void emitCommitCompleted(bool running);
    void emitCollapseCompleted(bool running);

    Q_DECLARE_PRIVATE(UCBottomEdge)
};

#endif // UCBOTTOMEDGE_H
