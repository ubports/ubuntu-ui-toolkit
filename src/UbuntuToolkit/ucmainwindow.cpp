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

#include "ucmainwindow_p_p.h"

#include "ucactionmanager_p.h"
#include "ucactioncontext_p.h"
#include "ucapplication_p.h"
#include "uctheme_p.h"
#include "i18n_p.h"
#include "quickutils_p.h"

#include <QCoreApplication>

UT_NAMESPACE_BEGIN

/*!
  \internal
  \qmlabstract MainWindow
  \inqmlmodule Ubuntu.Components.Labs
  \ingroup ubuntu
  \brief MainWindow is an alternate window-based root Item providing \l units
  and \l i18n as native properties, an \ actionContext and an \l applicationName.
  Unlike \l MainView there is no built-in header.

  The simplest way to use a MainWindow is to include a single \l Page object:
  \qml
      import QtQuick 2.4
      import Ubuntu.Components 1.3
      import Ubuntu.Components.Labs 1.3

      MainWindow {
          minimumWidth: units.gu(48)
          minimumHeight: units.gu(60)

          Page {
	      anchors.fill: parent
              header: PageHeader {
                  title: "Simple page"
              }
              Button {
                  anchors {
                      horizontalCenter: parent.horizontalCenter
                      top: pageHeader.bottom
                      topMargin: units.gu(5)
                  }
                  width: units.gu(15)
                  text: "Push me"
                  onClicked: print("Click!")
              }
          }
      }
  \endqml
  Anchors need to be set, there's no automatic fill like with \l MainView.

  Do not include multiple Pages directly, but use \l AdaptivePageLayout
  inside MainWindow to navigate between several Pages.

  If the \l Page inside the MainWindow includes a Flickable, set the flickable property of
  the PageHeader to automatically hide and show the header when the user scrolls up or down:
  \qml
      import QtQuick 2.4
      import Ubuntu.Components 1.3
      import Ubuntu.Components.Labs 1.3

      MainWindow {
          minimumWidth: units.gu(48)
          maximumHeight: units.gu(60)

          Page {
              anchors.fill: parent
              header: PageHeader {
                  title: "Page with Flickable"
                  flickable: myFlickable
              }

              Flickable {
                  id: myFlickable
                  anchors.fill: parent
                  contentHeight: column.height

                  Column {
                      id: column
                      Repeater {
                          model: 100
                          Label {
                              text: "line "+index
                          }
                      }
                  }
              }
          }
      }
  \endqml
  The same header behavior is automatic when using a ListView instead of a Flickable in the above
  example.

  The examples above show how to include a single \l Page inside a MainWindow, but more
  advanced application structures are possible using \l AdaptivePageLayout.
**/
UCMainWindowPrivate::UCMainWindowPrivate()
    : m_actionContext(nullptr),
      m_units(nullptr)
{
}

void UCMainWindowPrivate::init()
{
    Q_Q(UCMainWindow);

    //need to init here because the q pointer is null in constructor
    m_actionContext = new UCPopupContext(q);

    m_actionContext->setObjectName(QStringLiteral("RootContext"));
    m_actionContext->setActive(true);
}

UCMainWindow::UCMainWindow(QWindow *parent)
    : QQuickWindow(parent)
{
    QObject::connect(UbuntuI18n::instance(this), SIGNAL(domainChanged()),
                     this, SIGNAL(i18nChanged()));
    QObject::connect(UbuntuI18n::instance(this), SIGNAL(languageChanged()),
                     this, SIGNAL(i18nChanged()));
}

/*!
  \qmlproperty string MainWindow::applicationName

  The property holds the application's name, which must be the same as the
  desktop file's name.
  The name also sets the name of the QCoreApplication and defaults for data
  and cache folders that work on the desktop and under confinement, as well as
  the default gettext domain.
  C++ code that writes files may use QStandardPaths::writableLocation with
  QStandardPaths::DataLocation or QStandardPaths::CacheLocation.
*/
QString UCMainWindow::applicationName() const
{
    return d_func()->m_applicationName;
}

void UCMainWindow::setApplicationName(QString applicationName)
{
    Q_D(UCMainWindow);
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
  \qmlproperty Units MainWindow::units

  Grid units for this particular window - unlike the global context property
  by the same name.
*/
UCUnits* UCMainWindow::units()
{
    Q_D(UCMainWindow);

    if (!d->m_units) {
        d->m_units = new UCUnits(this);
        QObject::connect(d->m_units, SIGNAL(gridUnitChanged()),
                         this, SIGNAL(unitsChanged()));
    }
    return d->m_units;
}

/*!
  \qmlproperty Units MainWindow::i18n

  The property holds its breath for documentation.
*/
UbuntuI18n* UCMainWindow::i18n() const
{
    return UbuntuI18n::instance();
}

/*!
  \qmlproperty ActionContext MainWindow::actionContext
  \readonly
  \since Ubuntu.Components 1.3
  The action context of the MainWindow.
  */
UCPopupContext *UCMainWindow::actionContext() const
{
    return d_func()->m_actionContext;
}

UT_NAMESPACE_END

#include "moc_ucmainwindow_p.cpp"
