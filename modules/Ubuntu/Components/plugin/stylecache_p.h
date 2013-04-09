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

#ifndef STYLECACHE_P_H
#define STYLECACHE_P_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QList>
#include "selector_p.h"

// style rule tree
class QQmlComponent;
class StyleCache {
public:
    enum MatchResult {
        NoMatch,
        Match,
        ExactMatch
    };

public:
    class StyleData {
    public:
        QQmlComponent *style;
        QQmlComponent *delegate;
    public:
        StyleData(StyleData *parent);
        StyleData(const SelectorNode &node, QQmlComponent *style, QQmlComponent *delegate, StyleData *parent);
        ~StyleData();

        Selector selector() const;
        void clear();
        void add(const Selector &selector, QQmlComponent *style, QQmlComponent *delegate);
        MatchResult lookup(const Selector &selector, StyleData **match, int64_t &matchRank, int64_t searchRank);
    private:
        MatchResult test(SelectorNode &nextNode, const Selector &leftover, StyleData **match, int64_t &matchRank, int64_t searchRank);
        MatchResult setMatch(StyleData **match, int64_t &matchRank, int64_t searchRank);
        friend class StyleCache;

        StyleData *parent;
        SelectorNode node;
        QHash<SelectorNode, StyleData*> children;
        const unsigned short depth;
    };

public:
    StyleCache();
    ~StyleCache();
    void clear();
    void addStyleRule(const Selector &selector, QQmlComponent *style, QQmlComponent *delegate);
    StyleCache::StyleData *match(const Selector &selector);

    // the following property is just for benchmarking
    bool enableStyleCache;

private:
    StyleData *styles;
    QHash<Selector, StyleData*> cache;
};


#endif // STYLECACHE_P_H
