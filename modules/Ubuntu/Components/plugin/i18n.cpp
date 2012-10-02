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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include "i18n.h"

#include <QtDeclarative/QDeclarativeExtensionPlugin>

UbuntuI18n::UbuntuI18n(QObject* parent) {
    printf("Constructing UbuntuI18n object");
}

UbuntuI18n::~UbuntuI18n() {
    printf("Destroying UbuntuI18n object");
}

QString UbuntuI18n::tr(QString text) {
    if (text == "") return "no string?";
    return text;
}
