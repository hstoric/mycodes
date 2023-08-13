#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <xhash>
#include <string>
#include <memory>
#include "../list/list.hh
#include "../map/tree.h"
#include "../list/dynarray.hh"

template <typename K, typename V, typename H = std::hash<K>>
class HashMap {
public:
	struct Node {
		decltype(H::operator()) hash;
		K key;
		V value;

		inline Node(decltype(hash) hash, K key, V value) : hash(hash), key(key), value(value) {}
	};

private:
	DynArray<RBTree<Node>> _buckets;

private:
	inline void put(K key, V value) {
		auto hash = H(key);
		size_t index = hash % _buckets.size();

		if (_buckets.at(index).size()) {
			fprintf(stderr, "Warning: hash conflicted at bucket #%zu\n", index);
		}

		_buckets.at(index).insert(Node(hash, std::move(key), std::move(value)));
	}
};

#endif
