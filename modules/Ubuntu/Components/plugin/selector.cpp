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

#include "selector_p.h"
#include "itemstyleattached.h"
#include "itemstyleattached_p.h"
#include "themeengine_p.h"
#include "quickutils.h"
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>
#include <QtQuick/QQuickItem>

SelectorNode::SelectorNode() :
    relationship(Descendant), ranking(RankNull)
{}

SelectorNode::SelectorNode(const SelectorNode &other):
    className(other.className), styleClass(other.styleClass),
    styleId(other.styleId), relationship(other.relationship), ranking(RankNull)
{
    updateRanking();
}

SelectorNode::SelectorNode(const SelectorNode &other, int ignore) :
    className(other.className), relationship(Descendant), ranking(RankNull)
{
    if ((ignore & NoStyleId) != NoStyleId)
        styleId = other.styleId;
    styleClass = other.styleClass;
    if ((ignore & NoDerivates) == NoDerivates)
        styleClass.remove(derivates());
    if ((ignore & NoRelation) != NoRelation)
        relationship = other.relationship;
    updateRanking();
}

/*!
    \internal
    Creates an instance of a SelectorNode by parsing the selectorString. The
    sensitivity parameter configures the node so that during string conversion
    and comparison ignores the relationship, the name both or none. This feature
    is used when building up QmlTheme selectorTable.
*/
SelectorNode::SelectorNode(const QString &selectorString) :
    relationship(Descendant), ranking(RankNull)
{
    styleClass = selectorString.toLower().trimmed();
    if (styleClass.startsWith('>')) {
        relationship = Child;
        styleClass.remove('>');
    }
    int idIndex = styleClass.indexOf('#');
    if (idIndex != -1) {
        styleId = styleClass.mid(idIndex + 1).toLower();
        styleClass = styleClass.left(idIndex);
        if (idIndex > 1 && styleClass[0] == '.')
            styleClass = styleClass.mid(1, idIndex - 1);
    } else if (styleClass[0] == '.')
        styleClass = styleClass.mid(1);
    updateRanking();
}
/*!
 * \internal
 * Creates a selector node with the given type, class, id and relation.
 */
SelectorNode::SelectorNode(const QString &stype, const QString &sclass, const QString &sid, SelectorNode::Relationship srelation) :
    className(stype), styleClass(sclass), styleId(sid), relationship(srelation), ranking(RankNull)
{
    updateRanking();
}

/*!
 * \internal
 * Creates a selector node filling information gathered from the given item.
 */
SelectorNode::SelectorNode(QQuickItem *item) :
    relationship(Descendant), ranking(RankNull)
{
    ItemStyleAttached *style = ThemeEnginePrivate::attachedStyle(item);
    className = QuickUtils::instance().className(item);
    styleClass = style->d_ptr->styleClass;
    styleId = style->d_ptr->styleId;
    updateRanking();
}

/*!
    \internal
    Converts a SelectorNode into string using "<relation> .<class>#<name>"
    format. Depending on the ignore flags set, may ignore the relationship, derivates
    and styleId. The default ignore flag is Nothing.
  */
//FIXME: remove ignore flags after the lookup is cleaned&fixed
QString SelectorNode::toString(int ignore) const
{
    QString result;
    if (((ignore & NoRelation) !=  NoRelation) &&
            (relationship == SelectorNode::Child))
        result += ">";
    if (!styleClass.isEmpty())
        result += "." + styleClass;
    else if (!className.isEmpty()) {
        result += '.' + className;
    }
    if ((ignore & NoDerivates) == NoDerivates)
        result.remove(derivates());
    if (((ignore & NoStyleId) !=  NoStyleId) && !styleId.isEmpty())
        result += "#" + styleId;
    return result;
}

/*!
 * \internal
 * The function returns the rank of the selector node based on the layer, which
 * is its index in the selector.
 * Each node has two bits reserved for ranking, where one bit specifies whether
 * the direct child separator is present and th eother bit specifies whether the
 * style ID is present. These bits are shifted leftwards with the layer -1 value,
 * layer 0 being the root node which is not in use.
 */
unsigned SelectorNode::rank()
{
    return ranking;
}

bool SelectorNode::operator==(const SelectorNode &other) const
{
    QString myClass = (styleClass.isEmpty()) ? className : styleClass;
    QString otherClass = (other.styleClass.isEmpty()) ? other.className : other.styleClass;

    return (myClass == otherClass) && (styleId == other.styleId) && (relationship == other.relationship);
}

/*!
 * \internal
 * Returns the style type.
 * to QML.
 */
QString SelectorNode::type() const
{
    return className;
}

/*!
 * \internal
 * Returns the style class and its derivates in one string that is presentable
 * to QML.
 */
QString SelectorNode::getClass() const
{
    if (styleClass.indexOf('.') != -1)
        return styleClass.left(styleClass.indexOf('.'));
    return styleClass;
}

/*!
 * \internal
 * Returns the selectors the current one's properties are derived from.
 */
QString SelectorNode::derivates() const
{
    QString result;
    //cut the derivates from the style class
    int derivesIndex = styleClass.indexOf('.');
    if (derivesIndex != -1)
        result = styleClass.mid(derivesIndex);

    return result;
}

/*!
 * \internal
 * Returns the style ID;
 */
QString SelectorNode::id() const
{
    return styleId;
}

/*!
 * \internal
 * Retiurns the reference to the relationship.
 */
SelectorNode::Relationship SelectorNode::relation()
{
    return relationship;
}

/*!
  \internal
  Hash key for SelectorNode. Uses QString's hash function.
  */
uint qHash(const SelectorNode &key)
{
    return qHash(key.toString());
}


/*!
 * \internal
 * Converts a selector string into Selector object. Does not accept 32-node longer
 * selector;
 * Current support (ref: www.w3.org/TR/selector.html):
 *  - Type selectors, e.g: "Button"
 *  - Descendant selectors, e.g: "Dialog Button"
 *  - Child selectors, e.g: "Dialog>Button"
 *  - ID selectors, e.g: "Button#mySpecialButton"
 */
Selector::Selector(const QString &string)
{
    QString tmp(string.trimmed());
    // prepare for split
    if (tmp.contains('>')) {
        tmp.replace(QRegularExpression("[ ]*(>)[ ]*"), ">").replace('>', "|>");
    }
    tmp.replace(' ', '|');

    QStringList nodes = tmp.simplified().split('|');
    QStringListIterator inodes(nodes);
    inodes.toBack();
    while (inodes.hasPrevious()) {
        const QString &node = inodes.previous();
        if (node.isEmpty())
            continue;
        prepend(SelectorNode(node));
    }
}

/*!
 * \internal
 * Builds the selector for the given item.
 */
Selector::Selector(QQuickItem *item)
{
    SelectorNode::Relationship relation = SelectorNode::Child;
    QQuickItem *parent = item->parentItem();

    prepend(SelectorNode(item));

    while (parent) {
        if (!ThemeEnginePrivate::attachedStyle(parent))
            relation = SelectorNode::Descendant;
        else {
            // update relationship
            first().relationship = relation;
            prepend(SelectorNode(parent));
            relation = SelectorNode::Child;
        }
        parent = parent->parentItem();
    }
}

/*!
  \internal
  Converts a style path back to selector string.
*/
QString Selector::toString() const
{
    QString result;

    QListIterator<SelectorNode> i(*this);
    while (i.hasNext()) {
        SelectorNode node = i.next();
        result += ' ' + node.toString();
    }
    result.replace(" >", ">");
    return result.simplified();
}

/*!
 * \internal
 * The method calculates the ranking of the selector itself, which is a
 * combination of the node rankings. The node rankings being bitmasks each of
 * them consuming 2 bits implies that the maximum depth of the selector can be
 * not more than 32 nodes.
 */
int64_t Selector::rank() const
{
    int64_t result = 0;
    int nodes = size();
    SelectorNode node;
    int i;
    for (i = nodes - 1; i >= 0; --i) {
        node = at(i);
        result |= ((int64_t)node.rank()) << 2 * i;
    }
    return result;
}


/*!
  \internal
  Hash key for Selector. Uses QString's hash function.
  */
uint qHash(const Selector &key)
{
    return qHash(key.toString());
}

