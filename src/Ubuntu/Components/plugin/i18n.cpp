/*
 * Copyright 2012-2015 Canonical Ltd.
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
#include "timeutils_p.h"
#include <QtCore/QDir>

namespace C {
#include <libintl.h>
#include <glib.h>
#include <glib/gi18n.h>
}

#include <stdlib.h>
#include <locale.h>

/*!
 * \qmltype i18n
 * \instantiates UbuntuI18n
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief i18n is a context property that provides internationalization support.
 *
 * i18n cannot be instantiated, and is already available as a context property.
 * It is based on \l {https://www.gnu.org/software/gettext/} {gettext}, and thus the standard gettext tools can be used for translating
 * a project. Example:
 * \qml
 * import QtQuick 2.4
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
    /*
     * setlocale
     * category = LC_ALL: All types of i18n: LC_MESSAGES, LC_NUMERIC, LC_TIME
     * locale = "": Lookup the environment for $LC_ALL, $LC_* and $LANG in that order
     * Returns: for example en_US.utf8, da_DK or POSIX
     *
     * Note: $LANGUAGE is implicitly respected by gettext() calls and
     *   defines the order of multiple locales
     */
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
QString UbuntuI18n::domain() const {
    return m_domain;
}

/*!
 * \qmlproperty string i18n::language
 * The language that is used for the translation. The default value is
 * the user's locale dending on $LC_ALL, $LC_MESSAGES and $LANG at the time
 * of running the application. See the gettext manual for details.
 */
QString UbuntuI18n::language() const {
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

void UbuntuI18n::setDomain(const QString &domain) {
    m_domain = domain;
    C::textdomain(domain.toUtf8());
    /*
     The default is /usr/share/locale if we don't set a folder
     For click we use APP_DIR/share/locale
     e.g. /usr/share/click/preinstalled/com.example.foo/current/share/locale
     */
    QString appDir(getenv("APP_DIR"));
    if (!QDir::isAbsolutePath (appDir)) {
        appDir = "/usr";
    }
    QString localePath(QDir(appDir).filePath("share/locale"));
    C::bindtextdomain(domain.toUtf8(), localePath.toUtf8());
    Q_EMIT domainChanged();
}

void UbuntuI18n::setLanguage(const QString &lang) {
    m_language = lang;

    /*
     This is needed for LP: #1263163.

     LANGUAGE may be set to one or more languages for example "fi" or
     "sv:de". gettext prioritizes LANGUAGE over LC_ALL, LC_*, and
     LANG, so if the session has already set LANGUAGE, calls to
     gettext will only use that.  We must override it here so that
     future calls to gettext are done in the new language.

     This only affects the current process. It does not override the
     user's session LANGUAGE.
     */
    setenv("LANGUAGE", lang.toUtf8().constData(), 1);

    /*
     The inverse form of setlocale as used in the constructor, passing
     a valid locale string updates all category type defaults.
     */
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

/*!
 * \qmlmethod string i18n::ctr(string context, string text)
 * Translate \a text using gettext and return the translation.
 * \a context is only visible to the translator and helps disambiguating for very short texts
 */
QString UbuntuI18n::ctr(const QString& context, const QString& text)
{
    return dctr(QString(), context, text);
}

/*!
 * \qmlmethod string i18n::dctr(string domain, string context, string text)
 * Translate \a text using gettext. Uses the specified domain \a domain instead of i18n.domain.
 * \a context is only visible to the translator and helps disambiguating for very short texts
 */
QString UbuntuI18n::dctr(const QString& domain, const QString& context, const QString& text)
{
    if (domain.isNull()) {
        return QString::fromUtf8(C::g_dpgettext2(NULL, context.toUtf8(), text.toUtf8()));
    } else {
        return QString::fromUtf8(C::g_dpgettext2(domain.toUtf8(), context.toUtf8(), text.toUtf8()));
    }
}

/*!
 * \qmlmethod string i18n::tag(string text)
 * Mark \a text for translation at a later point. Typically this allows an API
 * to take the original string and pass it to dtr (or dgettext).
 *
 * \qml
 * import QtQuick 2.4
 * import UserMetrics 0.1
 *
 * Metric {
 *     name: "distance"
 *     format: i18n.tag("Distance covered today: %1 km")
 *     emptyFormat: i18n.tag("No running today")
 *     domain: "runner.forest"
 * }
 * \endqml
 *
 * The strings tagged for localzation above are passed to the implementation
 * of UserMetrics verbatim, as well as the domain of the app. Display and
 * translation of the strings will happen in the lockscreen, where the same
 * strings will be passed to i18n.tr.
 */
QString UbuntuI18n::tag(const QString& text)
{
    return text;
}

/*!
 * \qmlmethod string i18n::tag(string context, string text)
 * Mark \a text for translation at a later point. Typically this allows an API
 * to take the original string and pass it to dctr (or g_dpgettext2).
 * \a context is only visible to the translator and helps disambiguating for very short texts
 */
QString UbuntuI18n::tag(const QString& context, const QString& text)
{
    Q_UNUSED(context);
    return text;
}
/*!
 * \qmlmethod string i18n::relativeDateTime(datetime dateTime)
 * Translate a datetime based on proximity to current time.
 */
QString UbuntuI18n::relativeDateTime(const QDateTime& datetime)
{
    QDateTime relativeTo(QDateTime::currentDateTime());
    const date_proximity_t prox = getDateProximity(relativeTo, datetime);

    switch (prox)  {
        case DATE_PROXIMITY_NOW:
            /* TRANSLATORS: Time based "this is happening/happened now" */
            return dtr("ubuntu-ui-toolkit", "Now");

        case DATE_PROXIMITY_HOUR:
        {
            qint64 diff = datetime.toMSecsSinceEpoch() - relativeTo.toMSecsSinceEpoch();
            qint64 minutes = qRound(float(diff) / 60000);
            if (minutes < 0) {
                return dtr("ubuntu-ui-toolkit", "%1 minute ago", "%1 minutes ago", qAbs(minutes)).arg(qAbs(minutes));
            }
            return dtr("ubuntu-ui-toolkit", "%1 minute", "%1 minutes", minutes).arg(minutes);
        }

        case DATE_PROXIMITY_TODAY:
            /* en_US example: "1:00 PM" */
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
            return datetime.toString(isLocale12h() ? dtr("ubuntu-ui-toolkit", "h:mm ap"):
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
                                                     dtr("ubuntu-ui-toolkit", "HH:mm"));

        case DATE_PROXIMITY_YESTERDAY:
            /* en_US example: "Yesterday  13:00" */
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
            return datetime.toString(isLocale12h() ? dtr("ubuntu-ui-toolkit", "'Yesterday\u2003'h:mm ap") :
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
                                                     dtr("ubuntu-ui-toolkit", "'Yesterday\u2003'HH:mm"));

        case DATE_PROXIMITY_TOMORROW:
            /* en_US example: "Tomorrow  1:00 PM" */
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
            return datetime.toString(isLocale12h() ? dtr("ubuntu-ui-toolkit", "'Tomorrow\u2003'h:mm ap") :
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
                                                     dtr("ubuntu-ui-toolkit", "'Tomorrow\u2003'HH:mm"));

        case DATE_PROXIMITY_LAST_WEEK:
        case DATE_PROXIMITY_NEXT_WEEK:
            /* en_US example: "Fri  1:00 PM" */
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
            return datetime.toString(isLocale12h() ? dtr("ubuntu-ui-toolkit", "ddd'\u2003'h:mm ap") :
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
                                                     dtr("ubuntu-ui-toolkit", "ddd'\u2003'HH:mm"));

        case DATE_PROXIMITY_FAR_BACK:
        case DATE_PROXIMITY_FAR_FORWARD:
        default:
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
            return datetime.toString(isLocale12h() ? dtr("ubuntu-ui-toolkit", "ddd d MMM'\u2003'h:mm ap") :
            /* TRANSLATORS: Please translated these to your locale datetime format using the format specified by
               https://qt-project.org/doc/qt-5-snapshot/qdatetime.html#fromString-2 */
                                                     dtr("ubuntu-ui-toolkit", "ddd d MMM'\u2003'HH:mm"));
    }
    return datetime.toString(Qt::DefaultLocaleShortDate);
}
