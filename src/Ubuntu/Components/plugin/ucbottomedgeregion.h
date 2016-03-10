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

#ifndef UCBOTTOMEDGEREGION_H
#define UCBOTTOMEDGEREGION_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include <QtCore/QUrl>
#include <QtCore/QPointer>
#include <QtQml/QQmlIncubator>
#include <AsyncLoader>

class UCBottomEdge;
class QQmlComponent;
class PropertyChange;
class UCBottomEdgeRegion : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled MEMBER m_enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(qreal from MEMBER m_from WRITE setFrom NOTIFY fromChanged FINAL)
    Q_PROPERTY(qreal to MEMBER m_to WRITE setTo NOTIFY toChanged FINAL)
    Q_PROPERTY(QUrl contentUrl MEMBER m_url WRITE setUrl NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent* contentComponent MEMBER m_component WRITE setComponent NOTIFY contentComponentChanged FINAL)
public:
    explicit UCBottomEdgeRegion(QObject *parent = 0, bool isDefault = false);
    void attachToBottomEdge(UCBottomEdge *bottomEdge);

    // used internally
    void setUrl(const QUrl &url);
    void setComponent(QQmlComponent *component);
    void setFrom(qreal from);
    void setTo(qreal to);
    void setEnabled(bool enabled);
    bool contains(qreal dragRatio);
    void enter();
    void exit();
    const QRectF rect(const QRectF &bottomEdgeRect);

    // not exposed to QML
    bool isActive()
    {
        return m_active;
    }
    bool isDefault()
    {
        return m_default;
    }

    QQuickItem *regionContent();
    inline const UbuntuToolkit::AsyncLoader *loader()
    {
        return const_cast<const UbuntuToolkit::AsyncLoader*>(&m_loader);
    }

Q_SIGNALS:
    void enabledChanged();
    void fromChanged();
    void toChanged();
    void contentChanged(const QUrl &url);
    void contentComponentChanged(QQmlComponent *component);

    void entered();
    void exited();
    void dragEnded();

protected:
    enum LoadingType {
        LoadingUrl,
        LoadingComponent
    };
    UbuntuToolkit::AsyncLoader m_loader;
    QUrl m_url;
    QPointer<UCBottomEdge> m_bottomEdge;
    QQmlComponent *m_component;
    QQuickItem *m_contentItem;
    qreal m_from;
    qreal m_to;
    bool m_enabled:1;
    bool m_active:1;
    const bool m_default:1;

    friend class UCBottomEdge;
    friend class UCBottomEdgePrivate;
    friend class tst_BottomEdge;

    void loadRegionContent();
    void discardRegionContent();
    void loadContent(LoadingType type);
    Q_SLOT void onLoaderStatusChanged(UbuntuToolkit::AsyncLoader::LoadingStatus,QObject*);
};

#endif // UCBOTTOMEDGEREGION_H
