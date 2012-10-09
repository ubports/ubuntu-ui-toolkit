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
 */

#include "i18n.h"

// libc
#include <libintl.h>


#include <stdio.h>

UbuntuI18n::UbuntuI18n(QObject* parent) : QObject(parent)
{
    qDebug("creating UbuntuI18n object");
    _domain = QString("gallery");
    _localeDir = QString("/usr/share/locale");
}

UbuntuI18n::~UbuntuI18n()
{
    // nothing to do
}

void UbuntuI18n::init(QString domain, QString localeDir)
//void UbuntuI18n::init(const char *domain, const char *localeDir)
{

    qDebug("UbuntuI18n.init(" + domain.toUtf8() + ", "+localeDir.toUtf8()+");");
//    printf(_domain.toUtf8().constData());
//    qDebug("UbuntuI18n.init("+domain+", "+localeDir+");");
//    qDebug("UbuntuI18n.init CALLED:");
//    qDebug(domain);
//    qDebug(localeDir);
    setlocale(LC_ALL, "");
//    bindtextdomain(domain.toUtf8().constData(), localeDir.toUtf8().constData());
//    textdomain(domain.toUtf8().constData());

//    bindtextdomain(domain, localeDir);
//    textdomain(domain);

        bindtextdomain("gallery", "/usr/share/locale");
        textdomain("gallery");
}

QString UbuntuI18n::domain() {
    return _domain;
}

QString UbuntuI18n::localeDir() {
    return _localeDir;
}

void UbuntuI18n::setDomain(QString domain) {
    qDebug("Setting domain to "+domain.toUtf8());
    _domain = domain;
    this->init(_domain, _localeDir);
//    this->init("gallery", "/usr/share/locale");
}

void UbuntuI18n::setLocaleDir(QString localeDir) {
    _localeDir = localeDir;
//    this->init(_domain, _localeDir);
}

QString UbuntuI18n::tr(const QString& text, const QString& domain)
{
    if (domain.isNull()) {
        return QString::fromUtf8(dgettext(NULL, text.toUtf8().constData()));
    } else {
        return QString::fromUtf8(dgettext(domain.toUtf8().constData(), text.toUtf8().constData()));
    }
}

QString UbuntuI18n::tr(const QString& singular, const QString& plural, int n, const QString& domain)
{
    if (domain.isNull()) {
        return QString::fromUtf8(dngettext(NULL, singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    } else {
        return QString::fromUtf8(dngettext(domain.toUtf8().constData(), singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    }
}
