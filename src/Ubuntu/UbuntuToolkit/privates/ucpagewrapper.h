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
 */

#ifndef UCPAGEWRAPPER_H
#define UCPAGEWRAPPER_H

#include "ucpagetreenode.h"

UT_NAMESPACE_BEGIN

class UCPageWrapperPrivate;
class UBUNTUTOOLKIT_EXPORT UCPageWrapper : public UCPageTreeNode
{
    Q_OBJECT
    Q_PROPERTY(QVariant reference READ reference WRITE setReference NOTIFY referenceChanged)
    Q_PROPERTY(QQuickItem* object READ object WRITE setObject NOTIFY objectChanged)
    Q_PROPERTY(bool canDestroy READ canDestroy NOTIFY canDestroyChanged)
    Q_PROPERTY(int column READ column WRITE setColumn NOTIFY columnChanged)
    Q_PROPERTY(QQuickItem* parentPage READ parentPage WRITE setParentPage NOTIFY parentPageChanged)
    Q_PROPERTY(QQuickItem* parentWrapper READ parentWrapper WRITE setParentWrapper NOTIFY parentWrapperChanged)
    Q_PROPERTY(QQuickItem* pageHolder READ pageHolder WRITE setPageHolder NOTIFY pageHolderChanged)
    Q_PROPERTY(QObject* incubator READ incubator NOTIFY incubatorChanged)
    Q_PROPERTY(bool synchronous READ synchronous WRITE setSynchronous NOTIFY synchronousChanged)
    Q_PROPERTY(QVariant properties READ properties WRITE setProperties NOTIFY propertiesChanged)

    //overrides
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible2 NOTIFY visibleChanged2 FINAL)

    // FIXME Re-expose property that would be inaccessible due to a QML bug
    // https://bugs.launchpad.net/ubuntu/+source/qtdeclarative-opensource-src/+bug/1389721
    Q_PROPERTY(UCTheme *theme READ getTheme2 WRITE setTheme2 RESET resetTheme2 NOTIFY themeChanged2 FINAL)
public:
    explicit UCPageWrapper(QQuickItem *parent = 0);
    virtual ~UCPageWrapper();

    QVariant reference() const;
    void setReference(const QVariant &reference);

    QQuickItem *object() const;
    void setObject(QQuickItem *object);

    bool canDestroy() const;

    int column() const;
    void setColumn(int column);

    QQuickItem *parentWrapper() const;
    void setParentWrapper(QQuickItem* parentWrapper);

    QQuickItem *pageHolder() const;
    void setPageHolder(QQuickItem* pageHolder);

    bool synchronous() const;
    void setSynchronous(bool synchronous);

    Q_INVOKABLE bool childOf (QQuickItem *page);

    QVariant properties() const;
    void setProperties(const QVariant &properties);

    QQuickItem *parentPage() const;
    void setParentPage(QQuickItem* parentPage);

    QObject *incubator() const;

    Q_INVOKABLE void destroyObject ();

    // QQuickItem interface
    void itemChange(ItemChange change, const ItemChangeData &data) override;

    //override QQuickItem properties
    void setVisible2(bool visible);

    UCTheme *getTheme2();
    void setTheme2(UCTheme * theme);
    void resetTheme2();

Q_SIGNALS:
    void referenceChanged(const QVariant &reference);
    void objectChanged(QObject* object);
    void canDestroyChanged(bool canDestroy);
    void columnChanged(int column);
    void parentWrapperChanged(QQuickItem* parentWrapper);
    void pageHolderChanged(QQuickItem* pageHolder);
    void synchronousChanged(bool synchronous);
    void propertiesChanged(const QVariant &properties);
    void pageLoaded();
    void parentPageChanged(QQuickItem* parentPage);
    void incubatorChanged(QObject* incubator);
    void visibleChanged2();
    void themeChanged2();

protected:
    UCPageWrapper(UCPageWrapperPrivate &dd, QQuickItem *parent);

private:
    Q_DECLARE_PRIVATE(UCPageWrapper)
    Q_PRIVATE_SLOT(d_func(), void nextStep())
    Q_PRIVATE_SLOT(d_func(), void onActiveChanged())
};

UT_NAMESPACE_END

#endif // UCPAGEWRAPPER_H
