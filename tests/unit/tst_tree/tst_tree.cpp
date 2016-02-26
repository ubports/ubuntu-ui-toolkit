/*
 * Copyright 2016 Canonical Ltd.
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
 * Author: Benjamin Zeller <benjamin.zeller@canonical.com>
 */

#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QSharedPointer>
#include <QVariant>

#include <UbuntuToolkit/Tree>

class tst_Tree : public QObject
{
    Q_OBJECT

public:
    tst_Tree() {}

private Q_SLOTS:

    void test_populateTree () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode2Stem1 = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));
        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(0, rootNode, subNode2Stem1));

        QVERIFY(tree.index(rootNode) == 0);
        QVERIFY(tree.index(subNode1Stem1) == 1);
        QVERIFY(tree.index(rootStem2) == 2);
        QVERIFY(tree.index(subNode2Stem1) == 3);

    }

    void test_doubleAddNode () {
        UbuntuToolkit::Tree tree;

        QObject node;
        QVERIFY(tree.add(0, nullptr, &node));
        QVERIFY(!tree.add(0, nullptr, &node));
    }

    void test_addRootWithParent () {
        UbuntuToolkit::Tree tree;

        QObject parent;
        QObject node;
        QVERIFY(!tree.add(0, &parent, &node));
    }

    void test_addSubNodeWithoutParent () {
        UbuntuToolkit::Tree tree;

        QObject root;
        QObject node;
        QVERIFY(tree.add(0, nullptr, &root));
        QVERIFY(!tree.add(0, nullptr, &node));
    }

    void test_addSubNodeWithParentNotInTree () {
        UbuntuToolkit::Tree tree;

        QObject root;
        QObject someParent;
        QObject node;
        QVERIFY(tree.add(0, nullptr, &root));
        QVERIFY(!tree.add(0, &someParent, &node));
    }

    void test_parentNodes () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode2Stem1 = new QObject(&parent);

        QObject *rootStem2 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));
        QVERIFY(tree.add(0, rootNode, subNode2Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.parent(rootNode) == nullptr);
        QVERIFY(tree.parent(rootStem2) == rootNode);
        QVERIFY(tree.parent(subNode1Stem1) == rootNode);
        QVERIFY(tree.parent(subNode2Stem1) == rootNode);
        QVERIFY(tree.parent(subNode1Stem2) == rootStem2);
    }

    void test_pruneSingleStem () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        QList<QObject *> removed = tree.prune(2);
        QVERIFY(removed.contains(subNode1Stem3));
        QVERIFY(removed.contains(rootStem3));

        QVERIFY(tree.index(rootNode)  == 0);
        QVERIFY(tree.index(subNode1Stem1) == 1);

        QVERIFY(tree.index(rootStem2) == 2);
        QVERIFY(tree.index(rootStem3) == -1);
        QVERIFY(tree.index(subNode1Stem2) == 3);
        QVERIFY(tree.index(subNode1Stem3) == -1);
    }

    void test_pruneMultipleStems () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        QList<QObject *> removed = tree.prune(1);
        QVERIFY(removed.contains(subNode1Stem2));
        QVERIFY(removed.contains(rootStem2));
        QVERIFY(removed.contains(subNode1Stem3));
        QVERIFY(removed.contains(rootStem3));

        QVERIFY(tree.index(rootNode)  == 0);
        QVERIFY(tree.index(rootStem2) == -1);
        QVERIFY(tree.index(rootStem3) == -1);
        QVERIFY(tree.index(subNode1Stem1) == 1);
        QVERIFY(tree.index(subNode1Stem2) == -1);
        QVERIFY(tree.index(subNode1Stem3) == -1);
    }

    void test_pruneAllStems () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        QList<QObject *> removed = tree.prune(0);
        QVERIFY(removed.contains(subNode1Stem2));
        QVERIFY(removed.contains(rootStem2));
        QVERIFY(removed.contains(subNode1Stem3));
        QVERIFY(removed.contains(rootStem3));
        QVERIFY(removed.contains(subNode1Stem1));
        QVERIFY(removed.contains(rootNode));

        QVERIFY(tree.index(rootNode)  == -1);
        QVERIFY(tree.index(rootStem2) == -1);
        QVERIFY(tree.index(rootStem3) == -1);
        QVERIFY(tree.index(subNode1Stem1) == -1);
        QVERIFY(tree.index(subNode1Stem2) == -1);
        QVERIFY(tree.index(subNode1Stem3) == -1);
    }

    void test_chopNode () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode2Stem1 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));
        QVERIFY(tree.add(0, rootNode, subNode2Stem1));

        QList<QObject *> removed = tree.chop(QVariant::fromValue<QObject *>(subNode2Stem1), true);
        QVERIFY(removed.size() == 1);
        QVERIFY(removed.contains(subNode2Stem1));
        QVERIFY(tree.index(subNode2Stem1) == -1);

        //check if everything else is as expected
        QVERIFY(tree.index(rootNode) == 0);
        QVERIFY(tree.index(subNode1Stem1) == 1);
    }

    void test_chopNodeAndStemsExclusive () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        //chop the root of the second stem but exclude the node
        QList<QObject *> nodes = tree.chop(QVariant::fromValue<QObject *>(rootStem2), false);

        //the node should not be removed
        QVERIFY(!nodes.contains(rootStem2) && tree.index(rootStem2) >= 0);
        QVERIFY(nodes.contains(subNode1Stem2) && tree.index(subNode1Stem2) == -1);
        QVERIFY(nodes.contains(rootStem3) && tree.index(rootStem3) == -1);
        QVERIFY(nodes.contains(subNode1Stem3) && tree.index(subNode1Stem3) == -1);

        //the other nodes should still be there
        QVERIFY(tree.index(rootNode) >= 0);
        QVERIFY(tree.index(subNode1Stem1) >= 0);
    }

    void test_chopNodeAndStemsInclusive () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        //chop the root of the second stem but exclude the node
        QList<QObject *> nodes = tree.chop(QVariant::fromValue<QObject *>(rootStem2), true);

        //the node should be removed as well
        QVERIFY(nodes.contains(rootStem2) && tree.index(rootStem2) == -1);
        QVERIFY(nodes.contains(subNode1Stem2) && tree.index(subNode1Stem2) == -1);
        QVERIFY(nodes.contains(rootStem3) && tree.index(rootStem3) == -1);
        QVERIFY(nodes.contains(subNode1Stem3) && tree.index(subNode1Stem3) == -1);

        //the other nodes should still be there
        QVERIFY(tree.index(rootNode) >= 0);
        QVERIFY(tree.index(subNode1Stem1) >= 0);
    }

    void test_chopNullNode () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        //make sure the tree is not altered if the passed node is null
        QList<QObject *> nodes = tree.chop(QVariant::fromValue<QObject *>(nullptr));

        //nothing should be removed
        QVERIFY(nodes.isEmpty());

        QVERIFY(tree.index(rootStem2) >= 0);
        QVERIFY(tree.index(subNode1Stem2) >= 0);
        QVERIFY(tree.index(rootStem3) >= 0);
        QVERIFY(tree.index(subNode1Stem3) >= 0);
        QVERIFY(tree.index(rootNode) >= 0);
        QVERIFY(tree.index(subNode1Stem1) >= 0);
    }

    void test_chopInvalidNode () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        //passing the node as "undefined" from JS will call chop() with a invalid QVariant()
        //make sure that behaves and removes only the top() node
        QObject *nodeToRemove = tree.top();
        QList<QObject *> nodes = tree.chop(QVariant());

        //nothing should be removed
        QVERIFY(nodes.size() == 1);
        QVERIFY(nodes.contains(nodeToRemove));

        QVERIFY(tree.index(rootNode) == 0);
        QVERIFY(tree.index(subNode1Stem1) == 1);
        QVERIFY(tree.index(rootStem2) == 2);
        QVERIFY(tree.index(subNode1Stem2) == 3);
        QVERIFY(tree.index(rootStem3) == 4);
        QVERIFY(tree.index(subNode1Stem3) == -1);
    }

    void test_top () {
        UbuntuToolkit::Tree tree;

        //use as cleanup helper to delete all created objects
        QObject parent;

        QObject *rootNode  = new QObject(&parent);
        QObject *rootStem2 = new QObject(&parent);
        QObject *rootStem3 = new QObject(&parent);
        QObject *rootStem5 = new QObject(&parent);

        QObject *subNode1Stem1 = new QObject(&parent);
        QObject *subNode1Stem2 = new QObject(&parent);
        QObject *subNode1Stem3 = new QObject(&parent);

        QVERIFY(tree.add(0, nullptr, rootNode));
        QVERIFY(tree.add(0, rootNode, subNode1Stem1));

        QVERIFY(tree.add(1, rootNode, rootStem2));
        QVERIFY(tree.add(1, rootStem2, subNode1Stem2));

        QVERIFY(tree.add(2, rootNode, rootStem3));
        QVERIFY(tree.add(2, rootStem3, subNode1Stem3));

        QVERIFY(tree.add(4, rootNode, rootStem5));

        //top without args should return the last node that was added
        QVERIFY(tree.top() == rootStem5);

        //check each stems top
        QVERIFY(tree.top(0, true) == subNode1Stem1);
        QVERIFY(tree.top(1, true) == subNode1Stem2);
        QVERIFY(tree.top(2, true) == subNode1Stem3);

        QVERIFY(tree.top(3, false) == rootStem5);

        //get the nth node for the stem
        QVERIFY(tree.top(0, true, 0) == subNode1Stem1);
        QVERIFY(tree.top(0, true, 1) == rootNode);

        QVERIFY(tree.top(2, true, 0) == subNode1Stem3);
        QVERIFY(tree.top(2, true, 1) == rootStem3);

        //out of bounds
        QVERIFY(tree.top(0, true, 19) == nullptr);
    }
};

QTEST_MAIN(tst_Tree)

#include "tst_tree.moc"
