/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef UCGLOBALS_H
#define UCGLOBALS_H

/*
 * Property declaration macros automating and simplifying the property getter/setter
 * declarations.
 */

#define DECLARE_PROPERTY(type, member, ...) \
    public: \
    void set_##member(const type &_arg_##member) \
    { \
        if (_arg_##member != m_##member) { \
            m_##member = _arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    } \
    type get_##member() const \
    { \
        return m_##member; \
    } \
    Q_SIGNALS: \
        void member##Changed(); \
    private: \
        type m_##member; \
        Q_PROPERTY(type member READ get_##member WRITE set_##member NOTIFY member##Changed)

#define DECLARE_PROPERTY_PTR(type, member, ...) \
    public: \
    void set_##member(type *_arg_##member) \
    { \
        if (_arg_##member != m_##member) { \
            m_##member = _arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    } \
    type *get_##member() const \
    { \
        return m_##member; \
    } \
    Q_SIGNALS: \
        void member##Changed(); \
    private: \
        type *m_##member; \
        Q_PROPERTY(type* member READ get_##member WRITE set_##member NOTIFY member##Changed)

/*
 * The following macros should be used together in .h and .cpp files separately,
 * where the property holder members are declared in the private class.
 * Place the DECLARE_ macros in the class declaration, and depending whether RO
 * or RW properties are to be declared, use the _SETTER and _GETTER macros in the
 * implementation. For instance when RW properties are declared, both getter and
 * setter macros are to be used.
 */

// declaration macros
#define DECLARE_PROPERTY_PRIVATE_RW(type, member, ...) \
    public: \
    void set_##member(type _arg_##member); \
    type get_##member() const; \
    Q_SIGNALS: \
        void member##Changed(); \
    private: \
        Q_PROPERTY(type member READ get_##member WRITE set_##member NOTIFY member##Changed __VA_ARGS__)


#define DECLARE_PROPERTY_PRIVATE_RO(type, member, ...) \
    public: \
    type get_##member() const; \
    Q_SIGNALS: \
        void member##Changed(); \
    public: \
        Q_PROPERTY(type member READ get_##member NOTIFY member##Changed __VA_ARGS__)


#define DECLARE_PROPERTY_PRIVATE_RO_NOSIGNAL(type, member, ...) \
    public: \
    type get_##member() const; \
    public: \
        Q_PROPERTY(type member READ get_##member __VA_ARGS__)


#define DECLARE_LISTPROPERTY_PRIVATE(type, member) \
    public: \
    QQmlListProperty<type> get_##member(); \
    private: \
        Q_PROPERTY(QQmlListProperty<type> member READ get_##member)

// implementation macros
#define PROPERTY_SETTER(_class, type, member, ...) \
    void _class::set_##member(type _arg_##member) \
    { \
        Q_D(_class); \
        if (_arg_##member != d->member) { \
            d->member = _arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

#define PROPERTY_GETTER(_class, type, member) \
    type _class::get_##member() const \
    { \
        Q_D(const _class); \
        return d->member; \
    }

#define LISTPROPERTY_GETTER(_class, type, member) \
    QQmlListProperty<type> _class::get_##member() \
    { \
        Q_D(_class); \
        return QQmlListProperty<type>(this, d->member); \
    }

#endif // UCGLOBALS_H
