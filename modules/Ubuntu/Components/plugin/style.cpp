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

#include "style.h"

/*!
  \class Style
  \brief Style class is the root component of every styling component in UITK.
  It is a helper

*/

Style::Style(QObject *parent) :
    QObject(parent)
{
}

void Style::classBegin()
{

}

void Style::componentComplete()
{
    // build style class name
    m_styleClass = metaObject()->className();
    m_styleClass = m_styleClass.left(m_styleClass.indexOf("Style_QMLTYPE"));
    // check parents
    QObject *parent = this->parent();
    while (parent) {
        if (qobject_cast<Style*>(parent)) {
            QString clname = parent->metaObject()->className();
            clname = clname.left(clname.indexOf("Style_QMLTYPE"));
            m_styleClass.prepend('.');
            m_styleClass.prepend(clname);
        }
        parent = parent->parent();
    }
}

/*!
  \preliminary
  Returns the class name of the style object. QML objects attach "_QMLTYPE_XXXX" to the class
  name that needs to be removed in order to identify the actual type properly. The style class
  name is built up with relation to their parents, meaning that if a style is encapsulated in
  another style, the name will be a compound of the style's parent and itself.
*/
QString Style::styleClass() const
{
    return m_styleClass;
}

/*!
  \internal
*/
QStringList Style::states() const
{
    return m_states;
}

/*!
  \internal
*/
void Style::setStates(const QStringList &states)
{
    if (m_states != states) {
        m_states = states;
        emit statesChanged();
    }
}

QString Style::subClass() const
{
    return m_subClass;
}
void Style::setSubClass(const QString &sclass)
{
    if (m_subClass != sclass) {
        m_subClass = sclass;
        emit subClassChanged();
    }
}


QDeclarativeListProperty<QObject> Style::data()
{
    return QDeclarativeListProperty<QObject>(this, m_Data);
}
