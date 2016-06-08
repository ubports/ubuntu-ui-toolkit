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
#ifndef UBUNTUTOOLKITGLOBAL_H
#define UBUNTUTOOLKITGLOBAL_H

#include <QtCore/QtGlobal>

#if defined(UBUNTUTOOLKIT_LIBRARY)
#  define UBUNTUTOOLKIT_EXPORT Q_DECL_EXPORT
#else
#  define UBUNTUTOOLKIT_EXPORT Q_DECL_IMPORT
#endif

#define BUILD_VERSION(major, minor)     ((((major) & 0x00FF) << 8) | ((minor) & 0x00FF))
#define LATEST_UITK_VERSION             BUILD_VERSION(1, 3)
#define MAJOR_VERSION(version)          ((version) >> 8)
#define MINOR_VERSION(version)          ((version) & 0x00FF)

#define UC_QML_DEPRECATION_WARNING(msg) \
    { \
        static bool loggedOnce = false; \
        if (!loggedOnce) { \
            if (QuickUtils::showDeprecationWarnings()) { \
                qmlInfo(this) << msg; \
            } \
        } \
    }

#endif // UBUNTUTOOLKITGLOBAL_H

