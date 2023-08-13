#ifndef __MAP_H__
#define __MAP_H__

#include "tree.h"

template <typename K, typename V>
class Map final {
public:
	struct Entry {
		K key;
		V value;

		inline Entry(K key) : key(key) {}
		inline Entry(K key, V value) : key(key), value(value) {}

		inline bool operator<(const Entry &rhs) const noexcept {
			return key < rhs.key;
		}

		inline bool operator>(const Entry &rhs) const noexcept {
			return key > rhs.key;
		}
	};

private:
	using Tree = RBTree<Entry>;
	Tree *_tree;

public:
	inline Map() {
		_tree = new Tree();
	}

	inline ~Map() {
		delete _tree;
	}

	typename Tree::Iterator begin() {
		return _tree->begin();
	}

	typename Tree::Iterator end() {
		return _tree->end();
	}

	inline void insert(K key, V value) {
		if (_tree->has(key))
			_tree->remove(key);
		_tree->insert({ key, value });
#ifndef NDEBUG
		_tree->verify();
#endif
	}

	inline void remove(K key) {
		_tree->remove(key);
#ifndef NDEBUG
		_tree->verify();
#endif
	}
};

#endif
