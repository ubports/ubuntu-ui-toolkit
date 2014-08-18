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

/***************************************************************************************
 * Property declaration macros automating and simplifying the property getter/setter
 * declarations.
 * These macros should be used together in .h and .cpp files separately, where the property
 * holder members are declared in the private class.
 * Place the DECLARE_ macros in the class declaration, and depending whether RO
 * or RW properties are to be declared, use the _SETTER and _GETTER macros in the
 * implementation. For instance when RW properties are declared, setter and getter
 * macros are to be used. On read-only, as well as on list properties only getter needs
 * to be declared.
 *
 * Setter macros can accept custom code which is executed after the property holder has
 * been set to the new value. In any other cases a custom setter implementation must be
 * provided. In this case one of the CUSTOM_SETTER macros can be used to provide the
 * prototype of the method.
 *
 * If the property holder members are declared in a private class, the corresponding
 * _PRIVATE_ macros should be used.
 *
 * Example:
 * - when the property holder members are in the private class:
 * in .h file
 *  class MyClass: public QObject {
 *      DECLARE_PROPERTY(qreal, rname)
 *      DECLARE_PROPERTY(QString, sname)
 *      DECLARE_PROPERTY(int, pname)
 *  }
 * in .cpp file
 *      SIMPLE_PROPERTY(MyClass, qreal, rname)
 *      SIMPLE_PROPERTY(MyClass, QString, sname)
 *
 *      PROPERTY_GETTER(MyClass, int, pvalue)
 *      PROPERTY_SETTER_PTYPE(MyClass, int, pvalue, *(d->pvalue) = 0)
 *
 */

/*--------------------------------------------------------------------------------------
 * Property declaration where the property holder member is declared in a private class.
 * Private classes must have "type member" or "type *member" property holders.
 --------------------------------------------------------------------------------------*/
/*
 * Property declaration of a normal/reference type, similar syntax to DECLARE_PROPERTY
 */
#define DECLARE_PRIVATE_PROPERTY(type, member, ...) \
    public: \
        type get_##member() const; \
        void set_##member(const type &arg_##member); \
    Q_SIGNALS: \
        void member##Changed(); \
    public: \
    Q_PROPERTY(type member READ get_##member WRITE set_##member NOTIFY member##Changed __VA_ARGS__)

/*
 * Property declaration of a pointer type, similar syntax to DECLARE_PROEPRTY_PTYPE
 */
#define DECLARE_PRIVATE_PROPERTY_PTYPE(type, member, ...) \
    public: \
        type *get_##member() const; \
        void set_##member(type *arg_##member); \
    Q_SIGNALS: \
        void member##Changed(); \
    public: \
        Q_PROPERTY(type* member READ get_##member WRITE set_##member NOTIFY member##Changed __VA_ARGS__)

/*
 * Declares read-only property. It can handle both normal and pointer types.
 * Usage:
 *  DECLARE_PRIVATE_PROPERTY(bool, bvalue, DESIGNABLE false)
 *  DECLARE_PRIVATE_PROPERTY(QObject*, object)
 */
#define DECLARE_PRIVATE_READONLY_PROPERTY(type, member, ...) \
    public: \
        type get_##member() const; \
    Q_SIGNALS: \
        void member##Changed(); \
    public: \
    Q_PROPERTY(type member READ get_##member NOTIFY member##Changed __VA_ARGS__)

/*
 * List property declaration.
 */
#define DECLARE_PRIVATE_LISTPROPERTY(type, member) \
    public: \
        QQmlListProperty<type> get_##member(); \
        Q_PROPERTY(QQmlListProperty<type> member READ get_##member)

//implementations

/*
 * Getter for both normal and pointer types
 * Usage:
 *  PROPERTY_PRIVATE_GETTER(MyClass, bool, bvalue)
 *  PROPERTY_PRIVATE_GETTER(MyClass, QObject*, object)
 */
#define PROPERTY_PRIVATE_GETTER(_class, type, member) \
    type _class::get_##member() const \
    { \
        Q_D(const _class); \
        return d->member; \
    }

/*
 * Same for list properties.
 */
#define LISTPROPERTY_PRIVATE_GETTER(_class, type, member) \
    QQmlListProperty<type> _class::get_##member() \
    { \
        Q_D(_class); \
        return QQmlListProperty<type>(this, d->member); \
    }

/*
 * Setter for normal/reference types. The variadic arguments can hold custom
 * implementation for the setter which is executed after the property holder
 * has been set to the new value.
 */
#define PROPERTY_PRIVATE_SETTER(_class, type, member, ...) \
    void _class::set_##member(const type &arg_##member) \
    { \
        Q_D(_class); \
        if (arg_##member != d->member) { \
            d->member = arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

/*
 * Same for pointer types.
 */
#define PROPERTY_PRIVATE_SETTER_PTYPE(_class, type, member, ...) \
    void _class::set_##member(type *arg_##member) \
    { \
        Q_D(_class); \
        if (arg_##member != d->member) { \
            d->member = arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }


/*--------------------------------------------------------------------------------------
 * Property declarations where the property holder member is declared in the same class
 */

/*
 * Property declaration of a normal/reference type.
 * Usage:
 *  DECLARE_PROPERTY(bool, bvalue)
 *      will result in
 *  Q_PROPERTY(bool bvalue .....)
 */
#define DECLARE_PROPERTY(type, member, ...) \
    DECLARE_PRIVATE_PROPERTY(type, member, __VA_ARGS__) \
    private: \
        type m_##member;

/*
 * Property declaration of a pointer type
 * Usage:
 *  DECLARE_PROPERTY(QObject, object)
 *      will result in
 *  Q_PROPERTY(QObject *object .....)
 */
#define DECLARE_PROPERTY_PTYPE(type, member, ...) \
    DECLARE_PRIVATE_PROPERTY_PTYPE(type, member, __VA_ARGS__) \
    private: \
        type *m_##member;
/*
 * Read-only property declaration.
 */
#define DECLARE_READONLY_PROPERTY(type, member, ...) \
    DECLARE_PRIVATE_READONLY_PROPERTY(type, member, __VA_ARGS__) \
    private: \
        type m_##member;
/*
 * List property declaration
 */
#define DECLARE_LISTPROPERTY(type, member) \
    DECLARE_PRIVATE_LISTPROPERTY(type, member) \
    private: \
        QList<type*> m_##member;

/*
 * Getter for both normal and pointer types
 * Usage:
 *  PROPERTY_GETTER(MyClass, bool, bvalue)
 *  PROPERTY_GETTER(MyClass, QObject*, object)
 */
#define PROPERTY_GETTER(_class, type, member) \
    type _class::get_##member() const \
    { \
        return m_##member; \
    }

/*
 * Setter for normal/reference types. The variadic arguments can hold custom
 * implementation for the setter which is executed after the property holder
 * has been set to the new value.
 */
#define PROPERTY_SETTER(_class, type, member, ...) \
    void _class::set_##member(const type &arg_##member) \
    { \
        if (arg_##member != m_##member) { \
            m_##member = arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

/*
 * Same for pointer types
 */
#define PROPERTY_SETTER_PTYPE(_class, type, member, ...) \
    void _class::set_##member(type *arg_##member) \
    { \
        if (arg_##member != m_##member) { \
            m_##member = arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

/*
 * Getter for list properties.
 */
#define LISTPROPERTY_GETTER(_class, type, member) \
    QQmlListProperty<type> _class::get_##member() \
    { \
        return QQmlListProperty<type>(this, m_##member); \
    }


/*---------------------------------------------------------------------------------------
 * Getter/Setter headlines, where the implementation requires custom code.
 */
#define CUSTOM_PROPERTY_GETTER(_class, type, member) \
    type _class::get_##member() const

#define CUSTOM_LISTPROPERTY_GETTER(_class, type, member) \
    QQmlListProperty<type> _class::get_##member()

#define CUSTOM_PROPERTY_SETTER(_class, type, member) \
    void _class::set_##member(const type &arg_##member)

#define CUSTOM_PROPERTY_SETTER_PTYPE(_class, type, member) \
    void _class::set_##member(type *arg_##member)

/*---------------------------------------------------------------------------------------
 * Definitions for simple property implementations, to be used in .cpp files.
 */
#define SIMPLE_PROPERTY(_class, type, member, ...) \
    PROPERTY_GETTER(_class, type, member) \
    PROPERTY_SETTER(_class, type, member, __VA_ARGS__)

#define SIMPLE_PROPERTY_PTYPE(_class, type, member, ...) \
    PROPERTY_GETTER(_class, type*, member) \
    PROPERTY_SETTER_PTYPE(_class, type, member, __VA_ARGS__)


#define SIMPLE_PRIVATE_PROPERTY(_class, type, member, ...) \
    PROPERTY_PRIVATE_GETTER(_class, type, member) \
    PROPERTY_PRIVATE_SETTER(_class, type, member, __VA_ARGS__)

#define SIMPLE_PRIVATE_PROPERTY_PTYPE(_class, type, member, ...) \
    PROPERTY_PRIVATE_GETTER(_class, type*, member) \
    PROPERTY_PRIVATE_SETTER_PTYPE(_class, type, member, __VA_ARGS__)

#endif // UCGLOBALS_H
