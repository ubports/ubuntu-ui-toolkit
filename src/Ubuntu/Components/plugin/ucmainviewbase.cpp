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

#include "ucmainviewbase.h"
#include "ucmainviewbase_p.h"
#include "ucnamespace.h"

#include "ucactionmanager.h"
#include "ucactioncontext.h"
#include "ucapplication.h"
#include "uctheme.h"
#include "i18n.h"

#include <ColorUtils>

#include <QCoreApplication>

/*!
  \internal
  \qmlabstract MainViewBase
  \inqmlmodule Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief The base class for MainView and MultiColumnView.
*/
UCMainViewBasePrivate::UCMainViewBasePrivate()
    : m_actionManager(nullptr),
      m_actionContext(nullptr),
      m_flags(0),
      m_anchorToKeyboard(false)
{
}

void UCMainViewBasePrivate::init()
{
    Q_Q(UCMainViewBase);

    //need to init here because the q pointer is null in constructor
    m_actionManager = new UCActionManager(q);
    m_actionContext = new UCPopupContext(q);

    m_actionContext->setObjectName(QStringLiteral("RootContext"));
    m_actionContext->setActive(true);
    q->setActive(true);

    QObject::connect(m_actionManager,&UCActionManager::quit, [](){
        // FIXME Wire this up to the application lifecycle management API instead of quit().
        qApp->quit();
    });
}

void UCMainViewBasePrivate::_q_headerColorBinding(const QColor &col)
{
    Q_Q(UCMainViewBase);

    if (m_headerColor == col)
        return;

    m_headerColor = col;
    Q_EMIT q->headerColorChanged(col);
}

void UCMainViewBasePrivate::_q_footerColorBinding(const QColor &col)
{
    Q_Q(UCMainViewBase);

    if (m_footerColor == col)
        return;

    m_footerColor = col;
    Q_EMIT q->footerColorChanged(col);
}

void UCMainViewBasePrivate::doAutoTheme()
{
    Q_Q(UCMainViewBase);
    UCTheme *theme = q->getTheme();
    if (!theme)
        return;

    if (m_backgroundColor != theme->getPaletteColor("normal", "background")) {
        QString themeName = UbuntuToolkit::ColorUtils::luminance(m_backgroundColor) >= 0.85 ? QStringLiteral("Ambiance")
                                                                   : QStringLiteral("SuruDark");

        // only change the theme if the current one is a system one.
        if (theme->name().startsWith(QStringLiteral("Ubuntu.Components.Themes"))) {
            q->getTheme()->setName(QString(QStringLiteral("Ubuntu.Components.Themes.%1")).arg(themeName));
        }
    }
}

UCMainViewBase::UCMainViewBase(QQuickItem *parent)
    : UCPageTreeNode(*(new UCMainViewBasePrivate), parent)
{
    d_func()->init();
}

UCMainViewBase::UCMainViewBase(UCMainViewBasePrivate &dd, QQuickItem *parent)
    : UCPageTreeNode( dd, parent)
{
    d_func()->init();
}

/*!
  \qmlproperty string MainView::applicationName

  The property holds the application's name, which must be the same as the
  desktop file's name.
  The name also sets the name of the QCoreApplication and defaults for data
  and cache folders that work on the desktop and under confinement, as well as
  the default gettext domain.
  C++ code that writes files may use QStandardPaths::writableLocation with
  QStandardPaths::DataLocation or QStandardPaths::CacheLocation.
*/
QString UCMainViewBase::applicationName() const
{
    return d_func()->m_applicationName;
}

void UCMainViewBase::setApplicationName(QString applicationName)
{
    Q_D(UCMainViewBase);
    if (d->m_applicationName == applicationName)
        return;

    d->m_applicationName = applicationName;

    if (applicationName != "") {
        UbuntuI18n::instance()->setDomain(applicationName);
        UCApplication::instance()->setApplicationName(applicationName);
    }
    Q_EMIT applicationNameChanged(applicationName);
}

/*!
  \qmlproperty bool MainView::anchorToKeyboard

  The property holds if the application should automatically resize the
  contents when the input method appears

  The default value is false.
*/
bool UCMainViewBase::anchorToKeyboard() const
{
    return d_func()->m_anchorToKeyboard;
}

void UCMainViewBase::setAnchorToKeyboard(bool anchorToKeyboard)
{
    Q_D(UCMainViewBase);
    if (d->m_anchorToKeyboard == anchorToKeyboard)
        return;

    d->m_anchorToKeyboard = anchorToKeyboard;
    Q_EMIT anchorToKeyboardChanged(anchorToKeyboard);
}

// deprecated
QColor UCMainViewBase::headerColor() const
{
    return d_func()->m_headerColor;
}

void UCMainViewBase::setHeaderColor(QColor headerColor)
{
    Q_D(UCMainViewBase);

    //disable binding to background color
    d->m_flags |= UCMainViewBasePrivate::CustomHeaderColor;
    d->_q_headerColorBinding(headerColor);
    UC_QML_DEPRECATION_WARNING("MainView.headerColor is deprecated in Ubuntu.Components 1.3.")
}

/*!
  \qmlproperty color MainView::backgroundColor
  Color of the background.

  Example:
  \qml
      import QtQuick 2.4
      import Ubuntu.Components 1.3

      MainView {
          width: units.gu(40)
          height: units.gu(60)
          backgroundColor: UbuntuColors.blue
      }
  \endqml
*/
QColor UCMainViewBase::backgroundColor() const
{
    return d_func()->m_backgroundColor;
}

void UCMainViewBase::setBackgroundColor(QColor backgroundColor)
{
    Q_D(UCMainViewBase);

    if (d->m_backgroundColor == backgroundColor)
        return;

    d->m_backgroundColor = backgroundColor;

    Q_EMIT backgroundColorChanged(backgroundColor);

    if (!(d->m_flags & UCMainViewBasePrivate::CustomHeaderColor))
        d->_q_headerColorBinding(d->m_backgroundColor);
    if (!(d->m_flags & UCMainViewBasePrivate::CustomFooterColor))
        d->_q_footerColorBinding(d->m_backgroundColor);

    // FIXME: Define the background colors in MainViewStyle and get rid of the properties
    //  in MainViewBase. That removes the need for auto-theming.
    d->doAutoTheme();
}

// deprecated
QColor UCMainViewBase::footerColor() const
{
    return d_func()->m_footerColor;
}

void UCMainViewBase::setFooterColor(QColor footerColor)
{
    Q_D(UCMainViewBase);

    //disable binding to background color
    d->m_flags |= UCMainViewBasePrivate::CustomFooterColor;
    d->_q_footerColorBinding(footerColor);
    UC_QML_DEPRECATION_WARNING("MainView.footerColor is deprecated in Ubuntu.Components 1.3.")
}

/*!
  \qmlproperty list<Action> MainView::actions
  \readonly
  \deprecated

  A global list of actions that will be available to the system (including HUD)
  as long as the application is running. For actions that are not always available to the
  system, but only when a certain \l Page is active, see the actions property of \l Page.
*/
QQmlListProperty<UCAction> UCMainViewBase::actions() const
{
    return d_func()->m_actionManager->actions();
}

/*!
  \qmlproperty ActionManager MainView::actionManager
  \readonly
  \deprecated

  The ActionManager that supervises the global and local ActionContexts.
  The \l actions property should be used preferably since it covers most
  use cases. The ActionManager is accessible to have a more refined control
  over the actions, e.g. if one wants to add/remove actions dynamically, create
  specific action contexts, etc.
*/
UCActionManager *UCMainViewBase::actionManager() const
{
    return d_func()->m_actionManager;
}

/*!
  \qmlproperty ActionContext MainView::actionContext
  \readonly
  \since Ubuntu.Components 1.3
  The action context of the MainView.
  */
UCPopupContext *UCMainViewBase::actionContext() const
{
    return d_func()->m_actionContext;
}

void UCMainViewBase::componentComplete()
{
    Q_D(UCMainViewBase);
    UCPageTreeNode::componentComplete();
    d->setStyleName(QStringLiteral("MainViewStyle"));
    d->doAutoTheme();

    if (d->m_actionContext)
        d->m_actionContext->componentComplete();

    if (d->m_actionManager)
        d->m_actionManager->componentComplete();
}

void UCMainViewBase::classBegin()
{
    Q_D(UCMainViewBase);
    UCPageTreeNode::classBegin();

    QQmlContext *contextForThis = qmlContext(this);
    if (d->m_actionManager) {
        QQmlEngine::setContextForObject(d->m_actionManager, contextForThis);
        d->m_actionManager->classBegin();
    }

    if (d->m_actionContext) {
        QQmlEngine::setContextForObject(d->m_actionContext, contextForThis);
        d->m_actionContext->classBegin();
    }
}
