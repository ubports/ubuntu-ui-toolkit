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

#ifndef UCBOTTOMEDGEREGION_P
#define UCBOTTOMEDGEREGION_P

#include "ucbottomedgeregion.h"
#include <AsyncLoader>
#include <QtCore/private/qobject_p.h>

using namespace UbuntuToolkit;

class UCBottomEdgeRegionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UCBottomEdgeRegion)
public:
    enum LoadingType {
        LoadingUrl,
        LoadingComponent
    };

    UCBottomEdgeRegionPrivate();
    void init();

    static UCBottomEdgeRegionPrivate *get(UCBottomEdgeRegion *that)
    {
        return that->d_func();
    }

    void attachToBottomEdge(UCBottomEdge *bottomEdge);
    virtual void loadRegionContent();
    virtual void discardRegionContent();
    void loadContent(LoadingType type);

    void onLoaderStatusChanged(AsyncLoader::LoadingStatus,QObject*);

    UbuntuToolkit::AsyncLoader loader;
    QUrl url;
    QPointer<UCBottomEdge> bottomEdge;
    QQmlComponent *component;
    QQuickItem *contentItem;
    qreal from;
    qreal to;
    bool enabled:1;
    bool active:1;
};

class DefaultRegionPrivate;
class DefaultRegion : public UCBottomEdgeRegion
{
    Q_OBJECT
public:
    DefaultRegion(UCBottomEdge *parent);
    bool canCommit(qreal dragRatio) Q_DECL_OVERRIDE;
private:
    Q_DECLARE_PRIVATE(DefaultRegion)
};

class DefaultRegionPrivate : public UCBottomEdgeRegionPrivate
{
    Q_DECLARE_PUBLIC(DefaultRegion)
public:
    DefaultRegionPrivate();
    void loadRegionContent() Q_DECL_OVERRIDE;
    void discardRegionContent() Q_DECL_OVERRIDE;
};

#endif // UCBOTTOMEDGEREGION_P

