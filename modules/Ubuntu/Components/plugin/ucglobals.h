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
 * implementation. For instance when RW properties are declared, setter, getter and reset
 * macros are to be used. On read-only, as well as on list properties only getter need
 * to be declared. Reset methods need custom implementation.
 *
 * If the property holder members are declared in a private class, the corresponding
 * _PRIVATE macros should be used.
 *
 * Example:
 * - when the property holder members are in the private class:
 * in .h file
 *  class MyClass: public QObject {
 *      DECLARE_PROPERTY_PRIVATE_RO(qreal, rvalue)
 *      DECLARE_PROEPRTY_PRIVATE(QString, svalue)
 *      DECLARE_PROPERTY_PRIVATE_PTYPE(int, pvalue)
 *  }
 * in .cpp file
 *      PROPERTY_GETTER_PRIVATE(MyClass, qreal, rvalue)
 *      PROPERTY_SETTER_PRIVATE(MyClass, qreal, rvalue)
 *      CUSTOM_PROPERTY_RESET(MyClass, rvalue){}
 *
 *      PROPERTY_GETTER_PRIVATE(MyClass, QString, svalue)
 *      PROPERTY_SETTER_PRIVATE(MyClass, QStrong, svalue)
 *      CUSTOM_PROPERTY_RESET(MyClass, svalue){}
 *
 *      PROPERTY_GETTER_PRIVATE(MyClass, int, pvalue)
 *      PROPERTY_SETTER_PRIVATE(MyClass, int, pvalue, *(d->pvalue) = 0)
 *      CUSTOM_PROPERTY_RESET(MyClass, pvalue)
 *      {
 *          Q_D(MyClass);
 *          free(d->pvalue);
 *          d->pvalue = 0;
 *      }
 *
 */

/*--------------------------------------------------------------------------------------
 * Property declaration where the property holder member is declared in a private class.
 * Private classes must have "type member" or "type *member" property holders.
 --------------------------------------------------------------------------------------*/
/*
 * Property declaration of a normal/reference type, similar syntax to DECLARE_PROPERTY
 */
#define DECLARE_PROPERTY_PRIVATE(type, member, ...) \
    public: \
        Q_PROPERTY(type member READ get_##member WRITE set_##member RESET reset_##member NOTIFY member##Changed __VA_ARGS__) \
        type get_##member() const; \
        void set_##member(const type &arg_##member); \
        void reset_##member(); \
    Q_SIGNALS: \
        void member##Changed();

/*
 * Property declaration of a pointer type, similar syntax to DECLARE_PROEPRTY_PTYPE
 */
#define DECLARE_PROPERTY_PRIVATE_PTYPE(type, member, ...) \
    public: \
        Q_PROPERTY(type* member READ get_##member WRITE set_##member RESET reset_##member NOTIFY member##Changed __VA_ARGS__) \
        type *get_##member() const; \
        void set_##member(type *arg_##member); \
        void reset_##member(); \
    Q_SIGNALS: \
        void member##Changed();

/*
 * Declares read-only property. It can handle both normal and pointer types.
 * Usage:
 *  DECLARE_PROPERTY_PRIVATE(bool, bvalue, DESIGNABLE false)
 *  DECLARE_PROPERTY_PRIVATE(QObject*, object)
 */
#define DECLARE_PROPERTY_PRIVATE_RO(type, member, ...) \
    public: \
        type get_##member() const; \
    Q_SIGNALS: \
        void member##Changed(); \
    public: \
    Q_PROPERTY(type member READ get_##member NOTIFY member##Changed __VA_ARGS__)

/*
 * List property declaration.
 */
#define DECLARE_LISTPROPERTY_PRIVATE(type, member) \
    public: \
        QQmlListProperty<type> get_##member(); \
        Q_PROPERTY(QQmlListProperty<type> member READ get_##member)

//implementations

/*
 * Getter for both normal and pointer types
 * Usage:
 *  PROPERTY_GETTER_PRIVATE(MyClass, bool, bvalue)
 *  PROPERTY_GETTER_PRIVATE(MyClass, QObject*, object)
 */
#define PROPERTY_GETTER_PRIVATE(_class, type, member) \
    type _class::get_##member() const \
    { \
        Q_D(const _class); \
        return d->member; \
    }

/*
 * Same for list properties.
 */
#define LISTPROPERTY_GETTER_PRIVATE(_class, type, member) \
    QQmlListProperty<type> _class::get_##member() \
    { \
        Q_D(_class); \
        return QQmlListProperty<type>(this, d->member); \
    }

/*
 * Setter for normal/reference types. The variadic arguments can hold custom
 * implementation for the setter which is executed after the property holder
 * has been set to the new value. The reset method is called prior to the new
 * value is set.
 */
#define PROPERTY_SETTER_PRIVATE(_class, type, member, ...) \
    void _class::set_##member(const type &arg_##member) \
    { \
        Q_D(_class); \
        if (arg_##member != d->member) { \
            reset_##member(); \
            d->member = arg_##member; \
            __VA_ARGS__; \
            Q_EMIT member##Changed(); \
        } \
    }

/*
 * Same for pointer types.
 */
#define PROPERTY_SETTER_PRIVATE_PTYPE(_class, type, member, ...) \
    void _class::set_##member(type arg_##member) \
    { \
        Q_D(_class); \
        if (arg_##member != d->member) { \
            reset_##member(); \
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
    DECLARE_PROPERTY_PRIVATE(type, member, __VA_ARGS__) \
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
    DECLARE_PROPERTY_PRIVATE_PTYPE(type, member, __VA_ARGS__) \
    private: \
        type *m_##member;
/*
 * Read-only property declaration.
 */
#define DECLARE_PROPERTY_RO(type, member, ...) \
    DECLARE_PROPERTY_PRIVATE_RO(type, member, __VA_ARGS__) \
    private: \
        type m_##member;
/*
 * List property declaration
 */
#define DECLARE_LISTPROPERTY(type, member) \
    DECLARE_LISTPROPERTY_PRIVATE(type, member) \
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
 * has been set to the new value. The reset method is called prior to the new
 * value is set.
 */
#define PROPERTY_SETTER(_class, type, member, ...) \
    void _class::set_##member(const type &arg_##member) \
    { \
        if (arg_##member != m_##member) { \
            reset_##member(); \
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
            reset_##member(); \
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
 * Getter/Setter/Reset headlines, where the implementation requires custom code.
 */
#define CUSTOM_PROPERTY_GETTER(_class, type, member) \
    type _class::get_##member() const

#define CUSTOM_LISTPROPERTY_GETTER(_class, type, member) \
    QQmlListProperty<type> _class::get_##member()

#define CUSTOM_PROPERTY_SETTER(_class, type, member) \
    void _class::set_##member(const type &arg_##member)

#define CUSTOM_PROPERTY_SETTER_PTYPE(_class, type, member) \
    void _class::set_##member(type *arg_##member)

#define CUSTOM_PROPERTY_RESET(_class, member) \
    void _class::reset_##member()


/*---------------------------------------------------------------------------------------
 * Definitions for simple property implementations, to be used in .cpp files.
 */
#define SIMPLE_PROPERTY(_class, type, member, ...) \
    PROPERTY_GETTER(_class, type, member) \
    PROPERTY_SETTER(_class, type, member, __VA_ARGS__) \
    CUSTOM_PROPERTY_RESET(_class, member){}

#define SIMPLE_PROPERTY_PTYPE(_class, type, member, ...) \
    PROPERTY_GETTER(_class, type*, member) \
    PROPERTY_SETTER_PTYPE(_class, type, member, __VA_ARGS__) \
    CUSTOM_PROPERTY_RESET(_class, member){}

#define SIMPLE_PROPERTY_RO(_class, type, member, ...) \
    PROPERTY_GETTER(_class, type, member)


#define SIMPLE_PROPERTY_PRIVATE(_class, type, member, ...) \
    PROPERTY_GETTER_PRIVATE(_class, type, member) \
    PROPERTY_SETTER_PRIVATE(_class, type, member, __VA_ARGS__) \
    CUSTOM_PROPERTY_RESET(_class, member){}

#define SIMPLE_PROPERTY_PTYPE(_class, type, member, ...) \
    PROPERTY_GETTER(_class, type*, member) \
    PROPERTY_SETTER_PTYPE(_class, type, member, __VA_ARGS__) \
    CUSTOM_PROPERTY_RESET(_class, member){}

#define SIMPLE_PROPERTY_PRIVATE_RO(_class, type, member, ...) \
    PROPERTY_GETTER_PRIVATE(_class, type, member)

#endif // UCGLOBALS_H
