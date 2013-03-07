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

#ifndef SELECTOR_P_H
#define SELECTOR_P_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QList>

// node of a selector
class SelectorNode {
    public:
    enum Relationship {
        Child,
        Descendant
    };
    enum IgnoreFlags {
        IgnoreNone = 0,
        NoRelation = 0x01,
        NoStyleId = 0x02,
        NoDerivates = 0x04,
        IgnoreAll = NoRelation | NoStyleId | NoDerivates};
    enum RankFlags {
        RankNull = 0,
        RankId = 0x01,
        RankChild = 0x02,
        RankFull = RankId | RankChild
    };

    SelectorNode();
    SelectorNode(const SelectorNode &other);
    SelectorNode(const SelectorNode &other, int ignore);
    SelectorNode(const QString &selectorString);
    SelectorNode(const QString &stype, const QString &sclass, const QString &sid, SelectorNode::Relationship srelation);
    inline bool isStrictRelationed() {
        return (relationship == Child);
    }
    inline bool isEmpty() {
        return className.isEmpty() && styleClass.isEmpty() && styleId.isEmpty();
    }

    QString toString(int ignore = IgnoreNone) const;
    unsigned rank();
    bool operator==(const SelectorNode &other) const;
    // getters
    QString type() const;
    QString getClass() const;
    QString derivates() const;
    QString id() const;
    Relationship relation();
private:
    // hide them so we don't mess them accidentaly
    QString className;
    QString styleClass;
    QString styleId;
    Relationship relationship;
    unsigned ranking;
    inline void updateRanking()
    {
        if (!styleId.isEmpty())
            ranking |= RankId;
        if (relationship == Child)
            ranking |= RankChild;
    }
};
uint qHash(const SelectorNode &key);

// selector type
class Selector : public QList<SelectorNode> {
public:
    inline Selector() {}
    inline Selector(const Selector& s) : QList<SelectorNode>(s){}
    Selector(const QString &string);
    virtual ~Selector() {}
    QString toString() const;
    int64_t rank() const;
};
Q_DECLARE_TYPEINFO(Selector, Q_MOVABLE_TYPE);
uint qHash(const Selector &key);

#endif // SELECTOR_P_H
