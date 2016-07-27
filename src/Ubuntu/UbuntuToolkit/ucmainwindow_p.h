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

#ifndef UCMAINWINDOW_H
#define UCMAINWINDOW_H

#include <i18n_p.h>
#include <ucunits_p.h>

#include <QtQuick/QQuickWindow>

UT_NAMESPACE_BEGIN

class UCMainWindowPrivate;
class UCPopupContext;
class UCAction;

class UBUNTUTOOLKIT_EXPORT UCMainWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName NOTIFY applicationNameChanged)
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCUnits)* units READ units NOTIFY unitsChanged)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UbuntuI18n)* i18n READ i18n NOTIFY i18nChanged)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCPopupContext)* actionContext READ actionContext NOTIFY actionContextChanged)
#else
    Q_PROPERTY(UCUnits* units READ units NOTIFY unitsChanged)
    Q_PROPERTY(UbuntuI18n* i18n READ i18n NOTIFY i18nChanged)
    Q_PROPERTY(UCPopupContext* actionContext READ actionContext NOTIFY actionContextChanged)
#endif

public:
    UCMainWindow(QWindow *parent = nullptr);


    QString applicationName() const;
    void setApplicationName(QString applicationName);

    UCUnits* units() const;
    UbuntuI18n* i18n() const;

    UCPopupContext* actionContext() const;

Q_SIGNALS:
    void applicationNameChanged(QString applicationName);
    void i18nChanged();
    void unitsChanged();
#ifndef Q_QDOC
    void actionContextChanged(UT_PREPEND_NAMESPACE(UCPopupContext)* actionContext);
#else
    void actionContextChanged(UCPopupContext* actionContext);
#endif

protected:
    UCMainWindow(UCMainWindowPrivate &dd, QQuickItem *parent);

private:
    Q_DECLARE_PRIVATE(UCMainWindow)
};

UT_NAMESPACE_END

#endif // UCMAINWINDOW_H
