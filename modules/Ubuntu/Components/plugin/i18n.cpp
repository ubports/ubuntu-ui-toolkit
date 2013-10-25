/*
 * Copyright 2012 Canonical Ltd.
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
 * Author: Tim Peeters <tim.peeters@canonical.om>
 */

#include "i18n.h"
#include <QtCore/QStandardPaths>

namespace C {
#include <libintl.h>
}

#include <QtQml>
#include <stdlib.h>
#include <locale.h>

/*!
 * \qmltype i18n
 * \instantiates UbuntuI18n
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief i18n is a context property that provides internationalization support.
 *
 * i18n cannot be instantiated, and is already available as a context property.
 * It is based on \l {https://www.gnu.org/software/gettext/} {gettext}, and thus the standard gettext tools can be used for translating
 * a project. Example:
 * \qml
 * import QtQuick 2.0
 * import Ubunut.Components 0.1
 *
 * Item {
 *      width: units.gu(40)
 *      height: units.gu(50)
 *
 *      Button {
 *          anchors.centerIn: parent
 *          text: i18n.tr("Press me")
 *      }
 * }
 * \endqml
 */
UbuntuI18n::UbuntuI18n(QObject* parent) : QObject(parent)
{
    m_domain = "";
    m_language = setlocale(LC_ALL, "");
}

/*!
 * \qmlproperty string i18n::domain
 * The gettext domain to be used for the translation. The default domain
 * is the applicationName specified in the application's \l MainView, or the empty string "" if
 * no applicationName was given or no \l MainView is used.
 * Use dtr() functions instead of tr() to use a different domain for a single translation
 * that ignores i18n.domain.
 */
QString UbuntuI18n::domain() {
    return m_domain;
}

/*!
 * \qmlproperty string i18n::language
 * The language that is used for the translation. The default value is the value of
 * environment variable LANGUAGE at the time when the QML application is started.
 * The value can be a list, for example "de_DE eo en".
 */
QString UbuntuI18n::language() {
    return m_language;
}

/**
 * \qmlmethod void i18n::bindtextdomain(string domain_name, string dir_name)
 * Specify that the domain_name message catalog can be found
 * in dir_name rather than in the system locale data base.
 */
void UbuntuI18n::bindtextdomain(const QString& domain_name, const QString& dir_name) {
    C::bindtextdomain(domain_name.toUtf8(), dir_name.toUtf8());
    Q_EMIT domainChanged();
}

void UbuntuI18n::setDomain(const QString& domain) {
    m_domain = domain;
    C::textdomain(domain.toUtf8());
    /*
     Look for locale folder as per XDG basedir spec
     The default is /usr/share/locale if we don't set a folder
     */
    QString localePath (QStandardPaths::locate(QStandardPaths::GenericDataLocation,
        "locale", QStandardPaths::LocateDirectory));
    if (!localePath.isEmpty())
        C::bindtextdomain(domain.toUtf8(), localePath.toUtf8());
    Q_EMIT domainChanged();
}

void UbuntuI18n::setLanguage(const QString &lang) {
    m_language = lang;
    setlocale(LC_ALL, lang.toUtf8());
    Q_EMIT languageChanged();
}

/*!
 * \qmlmethod string i18n::tr(string text)
 * Translate \a text using gettext and return the translation.
 */
QString UbuntuI18n::tr(const QString& text)
{
    return QString::fromUtf8(C::gettext(text.toUtf8()));
}

/*!
 * \qmlmethod string i18n::tr(string singular, string plural, int n)
 * Translate the given input string \a singular or \a plural (depending on the number of items \a n)
 * using gettext. Should be called like this:
 *          tr("%n file", "%n files", count)
 */
QString UbuntuI18n::tr(const QString &singular, const QString &plural, int n)
{
    return QString::fromUtf8(C::ngettext(singular.toUtf8(), plural.toUtf8(), n));
}

/*!
 * \qmlmethod string i18n::dtr(string domain, string text)
 * Translate \a text using gettext. Uses the specified domain \a domain instead of i18n.domain.
 */
QString UbuntuI18n::dtr(const QString& domain, const QString& text)
{
    if (domain.isNull()) {
        return QString::fromUtf8(C::dgettext(NULL, text.toUtf8()));
    } else {
        return QString::fromUtf8(C::dgettext(domain.toUtf8(), text.toUtf8()));
    }
}

/*!
 * \qmlmethod string i18n::dtr(string domain, string singular, string plural, int n)
 * Translate the given text using gettext. Should be called like this:
 *          tr(domain, "%n file", "%n files", count)
 * Uses \a domain for the domain instead of i18n.domain, and \a singular or \a plural
 * as input for the translation depending on the number of items \a n.
 */
QString UbuntuI18n::dtr(const QString& domain, const QString& singular, const QString& plural, int n)
{
    if (domain.isNull()) {
        return QString::fromUtf8(C::dngettext(NULL, singular.toUtf8(), plural.toUtf8(), n));
    } else {
        return QString::fromUtf8(C::dngettext(domain.toUtf8(), singular.toUtf8(), plural.toUtf8(), n));
    }
}
