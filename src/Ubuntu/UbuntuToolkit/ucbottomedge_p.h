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

#include "ucstyleditembase_p.h"
#include <QtCore/QPointer>
#include <QtQuick/private/qquickitemchangelistener_p.h>
#include <QtCore/QLoggingCategory>

class QQuickAbstractAnimation;

UT_NAMESPACE_BEGIN

class UCBottomEdgeHint;
class UCBottomEdgeRegion;
class UCBottomEdgePrivate;
class UBUNTUTOOLKIT_EXPORT UCBottomEdge : public UCStyledItemBase
{
    Q_OBJECT
    Q_ENUMS(Status DragDirection)

    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCBottomEdgeHint)* hint READ hint CONSTANT FINAL)
    Q_PROPERTY(qreal dragProgress READ dragProgress NOTIFY dragProgressChanged FINAL)
    Q_PROPERTY(DragDirection dragDirection READ dragDirection NOTIFY dragDirectionChanged FINAL)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged FINAL)
    Q_PROPERTY(QUrl contentUrl READ contentUrl WRITE setContent NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent *contentComponent READ contentComponent WRITE setContentComponent NOTIFY contentComponentChanged FINAL)
    Q_PROPERTY(QQuickItem* contentItem READ contentItem NOTIFY contentItemChanged FINAL)
    Q_PROPERTY(QQmlListProperty<UT_PREPEND_NAMESPACE(UCBottomEdgeRegion)> regions READ regions NOTIFY regionsChanged FINAL)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCBottomEdgeRegion)* activeRegion READ activeRegion NOTIFY activeRegionChanged FINAL)
    Q_PROPERTY(bool preloadContent READ preloadContent WRITE setPreloadContent NOTIFY preloadContentChanged FINAL DESIGNABLE false)

    // overloaded data property to catch regions
    Q_PRIVATE_PROPERTY(UCBottomEdge::d_func(), QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    enum Status {
        Hidden,
        Revealed,
        Committed
    };
    enum DragDirection {
        Undefined,
        Upwards,
        Downwards
    };

    explicit UCBottomEdge(QQuickItem *parent = 0);
    virtual ~UCBottomEdge();

    UCBottomEdgeHint *hint() const;
    qreal dragProgress();
    DragDirection dragDirection() const;
    Status status() const;
    QUrl contentUrl() const;
    void setContent(const QUrl &url);
    QQmlComponent *contentComponent() const;
    void setContentComponent(QQmlComponent *component);
    QQuickItem *contentItem() const;
    void setFillWindow(bool fill);
    QQmlListProperty<UCBottomEdgeRegion> regions();
    UCBottomEdgeRegion *activeRegion();
    bool preloadContent() const;
    void setPreloadContent(bool value);

Q_SIGNALS:
    void dragProgressChanged(qreal dragProgress);
    void dragDirectionChanged(UT_PREPEND_NAMESPACE(UCBottomEdge::DragDirection) direction);
    void statusChanged(UT_PREPEND_NAMESPACE(UCBottomEdge::Status) status);
    void contentChanged(const QUrl url);
    void contentComponentChanged(QQmlComponent *component);
    void contentItemChanged();
    void regionsChanged();
    void activeRegionChanged(UT_PREPEND_NAMESPACE(UCBottomEdgeRegion) *activeRegion);
    void preloadContentChanged();

    void commitStarted();
    void commitCompleted();
    void collapseStarted();
    void collapseCompleted();

public Q_SLOTS:
    void commit();
    void collapse();

protected:
    static void regions_append(QQmlListProperty<UCBottomEdgeRegion> *sections, UCBottomEdgeRegion *section);
    static int regions_count(QQmlListProperty<UCBottomEdgeRegion> *sections);
    static UCBottomEdgeRegion *regions_at(QQmlListProperty<UCBottomEdgeRegion> *sections, int index);
    static void regions_clear(QQmlListProperty<UCBottomEdgeRegion> *sections);

    void initializeComponent();
    void classBegin() override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    bool eventFilter(QObject *target, QEvent *event) override;

    void unlockOperation(bool running);
    void onParentHeightChanged();

    Q_DECLARE_PRIVATE(UCBottomEdge)

private:
    friend class tst_BottomEdge;
};

UT_NAMESPACE_END

Q_DECLARE_METATYPE(UT_PREPEND_NAMESPACE(UCBottomEdge)::Status)
Q_DECLARE_METATYPE(UT_PREPEND_NAMESPACE(UCBottomEdge)::DragDirection)

Q_DECLARE_LOGGING_CATEGORY(ucBottomEdge)

#define LOG     qCDebug(ucBottomEdge) << "[BottomEdge]"

#endif // UCBOTTOMEDGE_H
