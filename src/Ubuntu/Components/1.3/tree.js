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
            if (this.index(parentNode) === -1) {
                throw "Cannot add non-root node if parentNode is not in the tree.";
            }
        }
        nodes.push(newNode);
        stems.push(stem);
        parents.push(parentNode);
        size++;
    }

    // Remove all nodes from the specified stem and higher stems.
    //
    // Returns the removed nodes.
    this.prune = function(stem) {
        var newNodes = [];
        var newStems = [];
        var newParents = [];
        var removedNodes = [];
        for (var i = 0; i < nodes.length; i++) {
            if (stems[i] < stem) {
                newNodes.push(nodes[i]);
                newStems.push(stems[i]);
                newParents.push(parents[i]);
            } else {
                removedNodes.push(nodes[i]);
            }
        }
        nodes = newNodes;
        stems = newStems;
        parents = newParents;
        size = nodes.length;
        return removedNodes;
    }

    // Chops all nodes with an index higher than the given node which
    //  are in the same stem or a higher stem.
    //
    // If, and only if, (inclusive) then also chop the given node.
    //
    // Default values for node and inclusive are top() and true.
    // Returns a list that contains the nodes that were chopped.
    this.chop = function(node, inclusive) {
        node = typeof node !== 'undefined' ? node : this.top();
        inclusive = typeof inclusive !== 'undefined' ? inclusive : true
        var nodeIndex = this.index(node);
        if (nodeIndex < 0) {
            // given node is not in the tree.
            return [];
        }
        if (inclusive) {
            size = nodeIndex;
        } else {
            size = nodeIndex + 1;
        }

        // Nodes with index(node) >= nodeIndex && stem >= stems[nodeIndex];
        var badNodes = []; // to fill below

        // Nodes with index(node) >= nodeIndex (any stem).
        // Potential bad nodes to be removed.
        var uglyNodes = nodes.slice(size);
        var uglyStems = stems.slice(size); // to check below
        var uglyParents = parents.slice(size);

        var stem = stems[nodeIndex];
        // Good nodes, with index(node) < nodeIndex && stem < stems[nodeIndex]:
        nodes = nodes.slice(0, size);
        stems = stems.slice(0, size);
        parents = parents.slice(0, size);

        // Add nodes with index(node) > nodeIndex && stem < stems[nodeIndex] back:
        for (var i = 0; i < uglyNodes.length; i++) {
            if (uglyStems[i] < stem) {
                // Because the stem of the parentNode <= stem of the node,
                //  the node that is added back has the parentNode in nodes.
                nodes.push(uglyNodes[i]);
                stems.push(uglyStems[i]);
                parents.push(uglyParents[i]);
                size++;
            } else {
                badNodes.push(uglyNodes[i]);
            }
        }
        return badNodes;
    }

    // Returns the n'th node when traversing one or more stems from the
    //  top down. When exactMatch, only the specified stem is traversed, and
    //  when !exactMatch the specified stem and all higher stems are traversed.
    //
    // Returns null if no matching node was found.
    //
    // Default value for stem: 0
    // Default value for exactMatch: false
    // Default value for n: 0 (first node)
    //
    // Calling top() with no parameters returns top(0, false, 0) which is the
    //  last node that was added to the tree.
    this.top = function(stem, exactMatch, n) {
        stem = typeof stem !== 'undefined' ? stem : 0
        exactMatch = typeof exactMatch !== 'undefined' ? exactMatch : false
        n = typeof n !== 'undefined' ? n : 0

        var st;
        var count = n;
        for (var i = size - 1; i >= 0; i--) {
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
        return parents[i];
    }
}
