/*
 * Copyright 2015 Canonical Ltd.
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

.pragma library

function Tree() {
    // list<object> of nodes
    var nodes = [];

    // list<int> of stems
    var stems = [];

    // list<int> of indices of parent nodes
    var parents = [];

    // int number of nodes in the tree
    var size = 0;

    // Return the index of the given node.
    // Returns -1 the node was not found.
    this.index = function(node) {
        return nodes.indexOf(node);
    }

    // Add newNode to the tree in the specified stem, with the specified parent node.
    // The root node should be added with parentNode null. All other nodes must be added
    // with a parentNode that is already in the tree.
    this.add = function(stem, parentNode, newNode) {
        if (this.index(newNode) !== -1) {
            throw "Cannot add the same node twice to a tree.";
        }
        var parentIndex = this.index(parentNode);
        if (size === 0) {
            // adding root node
            if (parentNode !== null) {
                throw "Root node must have parentNode null.";
            }
        } else {
            // adding non-root node
            if (parentNode === null) {
                throw "Only root node has parentNode null."
            }
            if (parentIndex === -1) {
                throw "Cannot add non-root node if parentNode is not in the tree.";
            }
        }
        nodes.push(newNode);
        stems.push(stem);
        parents.push(parentIndex);
        size++;
    }

    // Chops all nodes with an index higher than the given node.
    // If, and only if, (inclusive) then also chop the given node.
    //
    // Default values for node and inclusive are top() and true.
    // Returns a list that contains the nodes that were chopped.
    this.chop = function(node, inclusive) {
        node = typeof node !== 'undefined' ? node : this.top();
        inclusive = typeof inclusive !== 'undefined' ? inclusive : true
        var nodeIndex = this.index(node);
        if (nodeIndex >= 0) {
            if (inclusive) {
                size = nodeIndex;
            } else {
                size = nodeIndex + 1;
            }
            var oldNodes = nodes;
            nodes = nodes.slice(0, size);
            stems = stems.slice(0, size);
            parents = parents.slice(0, size);
            return oldNodes.slice(size);
        } else {
            // given node is not in the tree
            return [];
        }
    }

    // If exactMatch, return the (count)th node on top of the specified stem.
    // If !exactMatch, return the node with the (count)th highest index
    //                  for stem <= the returned node stem
    //
    // Returns the n'th node when traversing one or more stems from the
    //  top down. When exactMatch, only the specified stem is taken into account,
    //  and when !exactMatch the specified stem and all stems after that are
    //  taken into account.
    //
    // Default value for stem: 0
    // Default value for exactMatch: false
    // Default value for n: 0 (first node)
    //
    // Calling top() with no parameters returns top(0, false, 0) which is the
    //  last node that was added to the tree.
    //
    // Returns null if no matching node was found.
    this.top = function(stem, exactMatch, n) {
        stem = typeof stem !== 'undefined' ? stem : 0
        exactMatch = typeof exactMatch !== 'undefined' ? exactMatch : false
        n = typeof n !== 'undefined' ? n : 0

        var st;
        var count = n;
        for (var i = size-1; i >= 0; i--) {
            st = stems[i];
            if ((exactMatch && st === stem) || (!exactMatch && st >= stem)) {
                count--;
            }
            if (count < 0) {
                return nodes[i];
            }
        }
        return null;
    }

    // Return the parent node of the specified node in the tree
    this.parent = function(node) {
        var i = nodes.indexOf(node);
        if (i === -1) {
            throw "Specified node not found in tree.";
        } else if (i === 0) {
            throw "Root node has no parent node.";
        }
        return nodes[parents[i]];
    }
}
