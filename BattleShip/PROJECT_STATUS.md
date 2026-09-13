# Project Status

**Author:** Kevin Munoz-Rivera  
**Current milestone:** Initial C++ port + Battleship MVC console version

## DataStructures

- [x] `MyArrayList<T>`
- [x] `MyLinkedList<T>`
- [x] `MyOrderedList<T>`
- [x] `MyPriorityQueue<T>` / min heap
- [x] `MyTrie`
- [x] `LinkedBinaryTree<T>`
- [x] `ArrayStack<T>`
- [x] `CircularArrayQueue<T>`
- [x] `LinkedQueue<T>`
- [x] `MyBinarySearchTree<T>`
- [x] `AVLTree<T>`
- [x] `RedBlackTree<T>`
- [x] `MyHashTable<K,V>` — linear probing and chaining
- [x] `RadixSort`
- [x] Unit tests

## Battleship

- [x] MVC folder separation
- [x] Console View
- [x] 10x10 game board
- [x] Standard five-ship fleet
- [x] Safe coordinate parser (`A1` through `J10`)
- [x] Horizontal/vertical placement
- [x] Overlap and boundary protection
- [x] Hit / miss / sunk / duplicate-shot handling
- [x] Computer fleet placement
- [x] Computer targeting queue after hits
- [x] Enemy ship hiding in the console View
- [x] Unit tests
- [x] Strict-warning build
- [x] AddressSanitizer + UndefinedBehaviorSanitizer test pass

## Next iteration ideas

- [ ] Add save/load support
- [ ] Separate difficulty levels for computer targeting
- [ ] Add a replay option
- [ ] Add a GUI View later without changing Model rules
- [ ] Migrate the dependency-free test harness to GoogleTest when desired
