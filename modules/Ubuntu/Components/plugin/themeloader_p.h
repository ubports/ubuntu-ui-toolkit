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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef THEMELOADER_P_H
#define THEMELOADER_P_H

class QQmlEngine;
class StyleTreeNode;

class ThemeLoader {
public:
    ThemeLoader() : m_engine(0) {}
    virtual ~ThemeLoader(){}
    virtual StyleTreeNode *loadTheme(const QUrl &path, QStringList &themeFiles) = 0;
protected:
    QQmlEngine *m_engine;
};

#endif // THEMELOADER_P_H
