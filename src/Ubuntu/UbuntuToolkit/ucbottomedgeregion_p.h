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
#include <ubuntutoolkitglobal.h>

class QQmlComponent;

UT_NAMESPACE_BEGIN

class UCBottomEdge;
class PropertyChange;
class UCBottomEdgeRegionPrivate;
class UBUNTUTOOLKIT_EXPORT UCBottomEdgeRegion : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(qreal from READ from WRITE setFrom NOTIFY fromChanged FINAL)
    Q_PROPERTY(qreal to READ to WRITE setTo NOTIFY toChanged FINAL)
    Q_PROPERTY(QUrl contentUrl READ url WRITE setUrl NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent* contentComponent READ component WRITE setComponent NOTIFY contentComponentChanged FINAL)
public:
    explicit UCBottomEdgeRegion(QObject *parent = 0);

    // used internally
    QUrl url() const;
    void setUrl(const QUrl &url);
    QQmlComponent *component() const;
    void setComponent(QQmlComponent *component);
    qreal from() const;
    void setFrom(qreal from);
    qreal to() const;
    void setTo(qreal to);
    bool enabled() const;
    void setEnabled(bool enabled);

    // support API
    bool contains(qreal dragRatio);
    virtual bool canCommit(qreal dragRatio);
    void enter();
    void exit();
    const QRectF rect(const QRectF &bottomEdgeRect);

    // not exposed to QML
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
    UCBottomEdgeRegion(UCBottomEdgeRegionPrivate &dd, QObject *parent);
private:
    Q_DECLARE_PRIVATE(UCBottomEdgeRegion)
    Q_PRIVATE_SLOT(d_func(), void onLoaderStatusChanged(AsyncLoader::LoadingStatus,QObject*))
};

UT_NAMESPACE_END

#endif // UCBOTTOMEDGEREGION_H
