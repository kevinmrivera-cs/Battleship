#include "TestHarness.h"
#include "kmr/ds/AVLTree.h"
#include "kmr/ds/ArrayStack.h"
#include "kmr/ds/CircularArrayQueue.h"
#include "kmr/ds/LinkedBinaryTree.h"
#include "kmr/ds/LinkedQueue.h"
#include "kmr/ds/MyArrayList.h"
#include "kmr/ds/MyBinarySearchTree.h"
#include "kmr/ds/MyHashTable.h"
#include "kmr/ds/MyLinkedList.h"
#include "kmr/ds/MyOrderedList.h"
#include "kmr/ds/MyPriorityQueue.h"
#include "kmr/ds/MyTrie.h"
#include "kmr/ds/RadixSort.h"
#include "kmr/ds/RedBlackTree.h"

#include <string>
#include <vector>

using namespace kmr::ds;

int main() {
    kmr::test::Runner runner;

    runner.add("MyArrayList insert/remove/bounds", [] {
        MyArrayList<int> list;
        list.add(1); list.add(3); list.insert(2, 1);
        REQUIRE_EQ(list.toString(), std::string("[1, 2, 3]"));
        REQUIRE_EQ(list.remove(1), 2);
        REQUIRE_EQ(list.size(), std::size_t{2});
        REQUIRE_THROWS(list.get(99));
    });

    runner.add("MyLinkedList cursor and removal", [] {
        MyLinkedList<std::string> list;
        list.add("a"); list.add("b"); list.insert("x", 1);
        REQUIRE_EQ(list.toString(), std::string("[a, x, b]"));
        list.first(); REQUIRE(list.current() && *list.current() == "a");
        list.next(); REQUIRE(list.current() && *list.current() == "x");
        REQUIRE_EQ(list.remove(1), std::string("x"));
    });

    runner.add("MyOrderedList binary search", [] {
        MyOrderedList<int> list;
        list.add(5); list.add(1); list.add(3);
        REQUIRE_EQ(list.toString(), std::string("[1, 3, 5]"));
        REQUIRE(list.binarySearch(3) != nullptr);
        REQUIRE(list.binarySearch(4) == nullptr);
    });

    runner.add("MyPriorityQueue min heap", [] {
        MyPriorityQueue<int> queue;
        queue.insert(5); queue.insert(1); queue.insert(3); queue.insert(2);
        REQUIRE_EQ(queue.removeMin(), 1);
        REQUIRE_EQ(queue.removeMin(), 2);
        REQUIRE_EQ(queue.min(), 3);
    });

    runner.add("Queues FIFO", [] {
        CircularArrayQueue<int> a(2);
        a.enqueue(1); a.enqueue(2); a.enqueue(3);
        REQUIRE_EQ(a.dequeue(), 1); REQUIRE_EQ(a.dequeue(), 2); REQUIRE_EQ(a.dequeue(), 3);
        LinkedQueue<int> l;
        l.enqueue(4); l.enqueue(5);
        REQUIRE_EQ(l.dequeue(), 4); REQUIRE_EQ(l.front(), 5);
    });

    runner.add("ArrayStack LIFO", [] {
        ArrayStack<int> stack;
        stack.push(1); stack.push(2);
        REQUIRE_EQ(stack.pop(), 2); REQUIRE_EQ(stack.top(), 1);
    });

    runner.add("LinkedBinaryTree inorder", [] {
        LinkedBinaryTree<int> tree;
        auto& root = tree.setRoot(2);
        tree.setLeft(root, 1); tree.setRight(root, 3);
        REQUIRE(tree.inOrder() == std::vector<int>({1,2,3}));
    });

    runner.add("BST predecessor removal", [] {
        MyBinarySearchTree<int> tree;
        for (int value : {5,3,7,2,4,6,8}) tree.add(value);
        REQUIRE(tree.remove(5));
        REQUIRE(tree.find(5) == nullptr);
        REQUIRE(tree.inOrder() == std::vector<int>({2,3,4,6,7,8}));
    });

    runner.add("AVL remains balanced", [] {
        AVLTree<int> tree;
        for (int value = 0; value < 100; ++value) tree.add(value);
        REQUIRE_EQ(tree.size(), std::size_t{100});
        REQUIRE(tree.height() < 10);
        REQUIRE(tree.find(50) != nullptr);
        REQUIRE(tree.remove(50));
    });

    runner.add("RedBlackTree ordered search", [] {
        RedBlackTree<int> tree;
        for (int value = 20; value >= 0; --value) tree.add(value);
        REQUIRE_EQ(tree.size(), std::size_t{21});
        REQUIRE(tree.find(10) != nullptr);
        REQUIRE(tree.remove(10));
        REQUIRE(tree.find(10) == nullptr);
        REQUIRE_EQ(tree.size(), std::size_t{20});
        auto values = tree.inOrder();
        REQUIRE(values.front() == 0 && values.back() == 20);
    });

    runner.add("HashTable linear probing", [] {
        MyHashTable<int, int> table(8, CollisionStrategy::LinearProbing);
        table.put(0, 10); table.put(8, 20); table.put(16, 30);
        REQUIRE_EQ(table.get(8).value(), 20);
        REQUIRE(table.remove(8));
        REQUIRE(!table.get(8).has_value());
        REQUIRE_EQ(table.get(16).value(), 30);
    });

    runner.add("HashTable chaining", [] {
        MyHashTable<int, std::string> table(2, CollisionStrategy::Chaining);
        table.put(0, "a"); table.put(2, "b"); table.put(4, "c");
        REQUIRE_EQ(table.size(), std::size_t{3});
        REQUIRE_EQ(table.get(4).value(), std::string("c"));
    });

    runner.add("Trie insert/find/remove", [] {
        MyTrie trie;
        trie.insert("cat"); trie.insert("car"); trie.insert("dog");
        REQUIRE(trie.find("cat")); REQUIRE(trie.find("car")); REQUIRE(!trie.find("ca"));
        REQUIRE(trie.remove("cat")); REQUIRE(!trie.find("cat")); REQUIRE(trie.find("car"));
    });

    runner.add("RadixSort signed integers", [] {
        std::vector<int> values{5, -2, 0, 100, -10, 5};
        RadixSort::sort(values);
        REQUIRE(values == std::vector<int>({-10, -2, 0, 5, 5, 100}));
    });

    return runner.run();
}
