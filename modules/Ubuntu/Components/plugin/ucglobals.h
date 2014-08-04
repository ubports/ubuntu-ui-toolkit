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

/*
 * The following macros should be used together in .h and .cpp files separately,
 * where the property holder members are declared in the private class.
 * Place the DECLARE_ macros in the class declaration, and depending whether RO
 * or RW properties are to be declared, use the _SETTER and _GETTER macros in the
 * implementation. For instance when RW properties are declared, both getter and
 * setter macros are to be used.
 */

// proeprty declaration where the property holder is declared in the same class
#define DECLARE_PROPERTY(type, member, ...) \
    DECLARE_PROPERTY_PRIVATE(type, member, __VA_ARGS__) \
    private: \
        type m_##member;

#define DECLARE_PROPERTY_PTYPE(type, member, ...) \
    DECLARE_PROPERTY_PRIVATE_PTYPE(type, member, __VA_ARGS__) \
    private: \
        type *m_##member;

// property declarations where the property holder is declared in a private class
// private classes must declare "type member" or "type *member" property holders.
#define DECLARE_PROPERTY_PRIVATE(type, member, ...) \
    public: \
        Q_PROPERTY(type member READ get_##member WRITE set_##member RESET reset_##member NOTIFY member##Changed __VA_ARGS__) \
        type get_##member() const; \
        void set_##member(const type &_arg_##member); \
        void reset_##member(); \
    Q_SIGNALS: \
        void member##Changed();

#define DECLARE_PROPERTY_PRIVATE_PTYPE(type, member, ...) \
    public: \
        Q_PROPERTY(type* member READ get_##member WRITE set_##member RESET reset_##member NOTIFY member##Changed __VA_ARGS__) \
        type *get_##member() const; \
        void set_##member(type *_arg_##member); \
        void reset_##member(); \
    Q_SIGNALS: \
        void member##Changed();

// can handle both normal and ptypes
#define DECLARE_PROPERTY_PRIVATE_RO(type, member, ...) \
    public: \
        type get_##member() const; \
    Q_SIGNALS: \
        void member##Changed(); \
    public: \
    Q_PROPERTY(type member READ get_##member NOTIFY member##Changed __VA_ARGS__)


// #implementations
#define PROPERTY_GETTER(_class, type, member) \
    type _class::get_##member() const \
    { \
        return m_##member; \
    }

// the variadic arguments can hold custom implementation for the setter
// which is executed after the property hilder has been set to the new value
#define PROPERTY_SETTER(_class, type, member, ...) \
    void _class::set_##member(const type &_arg_##member) \
    { \
        if (_arg_##member != m_##member) { \
            reset_##member(); \
            m_##member = _arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

#define PROPERTY_SETTER_PTYPE(_class, type, member, ...) \
    void _class::set_##member(type *_arg_##member) \
    { \
        if (_arg_##member != m_##member) { \
            reset_##member(); \
            m_##member = _arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

#define LISTPROPERTY_GETTER(_class, type, member) \
    QQmlListProperty<type> _class::get_##member() \
    { \
        return QQmlListProperty<type>(this, m_##member); \
    }

// when proeprty holder is in private class
#define PROPERTY_GETTER_PRIVATE(_class, type, member) \
    type _class::get_##member() const \
    { \
        Q_D(const _class); \
        return d->member; \
    }

#define LISTPROPERTY_GETTER_PRIVATE(_class, type, member) \
    QQmlListProperty<type> _class::get_##member() \
    { \
        Q_D(_class); \
        return QQmlListProperty<type>(this, d->member); \
    }

#define PROPERTY_SETTER_PRIVATE(_class, type, member, ...) \
    void _class::set_##member(const type &_arg_##member) \
    { \
        Q_D(_class); \
        if (_arg_##member != d->member) { \
            d->member = _arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

// property reset must be implemented separately and cannot be automated.
#define PROPERTY_RESET(_class, member) \
    void _class::reset_##member()


// declares property implementation for simple properties (getter/setter/reset)
#define SIMPLE_PROPERTY(_class, type, member, ...) \
    PROPERTY_GETTER(_class, type, member) \
    PROPERTY_SETTER(_class, type, member, __VA_ARGS__) \
    PROPERTY_RESET(_class, member){}

#endif // UCGLOBALS_H
