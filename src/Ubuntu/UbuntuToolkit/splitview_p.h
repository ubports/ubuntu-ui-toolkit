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

#ifndef SPLITVIEW_P_H
#define SPLITVIEW_P_H

#include <QtQuick/private/qquickpositioners_p.h>

#include "ubuntutoolkitglobal.h"

UT_NAMESPACE_BEGIN

class ViewColumnPrivate;
class ViewColumn : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PRIVATE_PROPERTY(ViewColumn::d_func(), bool fillWidth MEMBER fillWidth WRITE setFillWidth NOTIFY fillWidthChanged)
    Q_PRIVATE_PROPERTY(ViewColumn::d_func(), qreal minimumWidth MEMBER minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)
    Q_PRIVATE_PROPERTY(ViewColumn::d_func(), qreal maximumWidth MEMBER maximumWidth WRITE setMaximumWidth NOTIFY maximumWidthChanged)
    Q_PRIVATE_PROPERTY(ViewColumn::d_func(), qreal preferredWidth MEMBER preferredWidth WRITE setPreferredWidth NOTIFY preferredWidthChanged)
public:
    explicit ViewColumn(QObject *parent = 0);

    bool resize(qreal delta);

Q_SIGNALS:
    void minimumWidthChanged();
    void maximumWidthChanged();
    void preferredWidthChanged();
    void fillWidthChanged();

protected:
    // from QQmlParserStatus
    void classBegin() override {}
    void componentComplete() override;
private:
    Q_DECLARE_PRIVATE(ViewColumn)
};

class SplitViewLayoutPrivate;
class SplitViewLayout : public QObject
{
    Q_OBJECT

    Q_PRIVATE_PROPERTY(SplitViewLayout::d_func(), bool when MEMBER when NOTIFY whenChanged)
#ifdef Q_QDOC
    Q_PRIVATE_PROPERTY(SplitViewLayout::d_func(), QQmlListProperty<ViewColumn> columns READ columns NOTIFY columnsChanged DESIGNABLE false)
#else
    Q_PRIVATE_PROPERTY(SplitViewLayout::d_func(), QQmlListProperty<UT_PREPEND_NAMESPACE(ViewColumn)> columns READ columns NOTIFY columnsChanged DESIGNABLE false)
#endif
    Q_CLASSINFO("DefaultProperty", "columns")
public:
    explicit SplitViewLayout(QObject *parent = 0);

Q_SIGNALS:
    void whenChanged();
    void columnsChanged();

private:
    Q_DECLARE_PRIVATE(SplitViewLayout)
};

class SplitView;
class SplitViewAttachedPrivate;
class SplitViewAttached : public QObject
{
    Q_OBJECT
    Q_PRIVATE_PROPERTY(SplitViewAttached::d_func(), int column READ getColumn NOTIFY columnChanged)
#ifdef Q_QDOC
    Q_PRIVATE_PROPERTY(SplitViewAttached::d_func(), SplitView* view READ view)
    Q_PRIVATE_PROPERTY(SplitViewAttached::d_func(), ViewColumn* columnConfig READ config NOTIFY columnChanged)
#else
    Q_PRIVATE_PROPERTY(SplitViewAttached::d_func(), UT_PREPEND_NAMESPACE(SplitView*) view READ view)
    Q_PRIVATE_PROPERTY(SplitViewAttached::d_func(), UT_PREPEND_NAMESPACE(ViewColumn*) columnConfig READ config NOTIFY columnChanged)
#endif
public:
    explicit SplitViewAttached(QObject *parent = 0);

    static SplitViewAttached *get(QQuickItem *item);

    void resize(qreal delta);

Q_SIGNALS:
    void columnChanged();

private:
    Q_DECLARE_PRIVATE(SplitViewAttached)
};

class SplitViewPrivate;
class SplitView : public QQuickBasePositioner
{
    Q_OBJECT
#ifdef Q_QDOC
    Q_PRIVATE_PROPERTY(SplitView::d_func(), QQmlListProperty<SplitViewLayout> layouts READ layouts NOTIFY layoutsChanged DESIGNABLE false)
    Q_PRIVATE_PROPERTY(SplitView::d_func(), SplitViewLayout *activeLayout READ getActiveLayout NOTIFY activeLayoutChanged)
#else
    Q_PRIVATE_PROPERTY(SplitView::d_func(), QQmlListProperty<UT_PREPEND_NAMESPACE(SplitViewLayout)> layouts READ layouts NOTIFY layoutsChanged DESIGNABLE false)
    Q_PRIVATE_PROPERTY(SplitView::d_func(), UT_PREPEND_NAMESPACE(SplitViewLayout) *activeLayout READ getActiveLayout NOTIFY activeLayoutChanged)
#endif
    Q_PRIVATE_PROPERTY(SplitView::d_func(), QQmlComponent *handleDelegate MEMBER handleDelegate WRITE setHandle NOTIFY handleDelegateChanged)
    // overrides
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing2 NOTIFY spacingChanged2)
public:
    explicit SplitView(QQuickItem *parent = 0);

    static UT_PREPEND_NAMESPACE(SplitViewAttached) *qmlAttachedProperties(QObject*);

Q_SIGNALS:
    void layoutsChanged();
    void activeLayoutChanged();
    void handleDelegateChanged();
    void spacingChanged2();

protected:
    SplitView(SplitViewPrivate &, QQuickItem *);
    ~SplitView();

    // property setters
    void setSpacing2(qreal spacing, bool reset = true);

    // from QQuickBasePositioner
    void doPositioning(QSizeF *contentSize) override;
    void reportConflictingAnchors() override;

    // overrides
    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void itemChange(ItemChange, const ItemChangeData &) override;
private:
    // QQuickBasePositionerPrivate is not an exported API, therefore we cannot derive from it
    SplitViewPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, SplitView)
    Q_PRIVATE_SLOT(d_func(), void changeLayout())
};

UT_NAMESPACE_END

QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(ViewColumn))
QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(SplitViewLayout))
QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(SplitView))
QML_DECLARE_TYPEINFO(UT_PREPEND_NAMESPACE(SplitView), QML_HAS_ATTACHED_PROPERTIES)

#endif // SPLITVIEW_P_H
