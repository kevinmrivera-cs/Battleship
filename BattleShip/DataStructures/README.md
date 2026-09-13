# Data Structures — C++

**Author:** Kevin Munoz-Rivera  
**Origin:** Reworked from Java data-structure coursework into idiomatic C++.

## Included structures

| Structure | C++ implementation notes |
|---|---|
| `MyArrayList<T>` | Manual dynamic storage with RAII, checked access, rule-of-five support |
| `MyLinkedList<T>` | Singly linked list using `std::unique_ptr`; supports a course-style cursor |
| `MyOrderedList<T>` | Sorted array-backed list with binary search |
| `MyPriorityQueue<T>` | Minimum binary heap backed by `MyArrayList<T>` |
| `MyTrie` | Prefix tree for strings |
| `LinkedBinaryTree<T>` | Linked general binary tree with traversals |
| `ArrayStack<T>` | LIFO stack backed by `MyArrayList<T>` |
| `CircularArrayQueue<T>` | Dynamically growing circular queue |
| `LinkedQueue<T>` | Linked FIFO queue |
| `MyBinarySearchTree<T>` | BST insert/find/remove; removal uses the predecessor |
| `AVLTree<T>` | Self-balancing BST with rotations |
| `RedBlackTree<T>` | Left-leaning red-black insertion/search implementation |
| `MyHashTable<K,V>` | Linear-probing or chaining collision strategy |
| `RadixSort` | LSD radix sort for signed integers |

## Safety choices

The implementations validate indices and capacities, avoid owning raw pointers, prevent accidental shallow-copy ownership bugs, bound probing loops in the hash table, and use exceptions for invalid operations that Java code previously represented with `null` or silent failure.
