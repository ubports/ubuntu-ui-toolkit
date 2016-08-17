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

#if !defined(UBUNTUTOOLKIT_NO_NAMESPACE)

#define UT_NAMESPACE_BEGIN          namespace UbuntuToolkit {
#define UT_NAMESPACE_END            }
#define UT_PREPEND_NAMESPACE(name)  UbuntuToolkit::name
#define UT_USE_NAMESPACE            using namespace UbuntuToolkit;
#define UT_FORWARD_DECLARE_CLASS(name) \
    UT_NAMESPACE_BEGIN class name; UT_NAMESPACE_END \
    using UT_PREPEND_NAMESPACE(name);
#define UT_FORWARD_DECLARE_STRUCT(name) \
    UT_NAMESPACE_BEGIN struct name; UT_NAMESPACE_END \
    using UT_PREPEND_NAMESPACE(name);

#else // no namespace

#define UT_NAMESPACE_BEGIN
#define UT_NAMESPACE_END
#define UT_PREPEND_NAMESPACE(name)  name
#define UT_USE_NAMESPACE
#define UT_FORWARD_DECLARE_CLASS(name) class name;
#define UT_FORWARD_DECLARE_STRUCT(name) struct name;

#endif

#endif // UBUNTUTOOLKITGLOBAL_H

