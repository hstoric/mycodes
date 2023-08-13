#ifndef __LIST_HH__
#define __LIST_HH__

#include <cstdint>
#include <algorithm>
#include <cassert>
#include <stdexcept>

template <typename T>
class List {
public:
	struct Node {
		Node *pre = nullptr, *next = nullptr;
		T v;

		inline Node() {}
		inline Node(Node *pre, Node *next) : pre(pre), next(next) {}
		inline Node(T v, Node *pre, Node *next) : v(v), pre(pre), next(next) {}
	};

	struct Iterator {
		Node *node;

		inline Iterator(const Iterator &it) : node(it.node) {}
		inline Iterator(const Iterator &&it) : node(it.node) {}
		inline Iterator(Node *node) : node(node) {}

		inline Iterator &operator=(const Iterator &rhs) noexcept {
			node = rhs.node;
			return *this;
		}

		inline Iterator &operator=(const Iterator &&rhs) noexcept {
			node = rhs.node;
			return *this;
		}

		inline Iterator &operator++() {
			if (!node)
				throw std::logic_error("Increasing the end iterator");

			node = node->next;

			return *this;
		}

		inline Iterator operator++(int) {
			Iterator it = *this;
			++(*this);
			return it;
		}

		inline Iterator &operator--() {
			if (!node->pre)
				throw std::logic_error("Dereasing the begin iterator");

			node = node->pre;

			return *this;
		}

		inline Iterator operator--(int) {
			Iterator it = *this;
			--(*this);
			return it;
		}

		inline bool operator==(const Node *node) const noexcept {
			return node == node;
		}

		inline bool operator==(const Iterator &it) const {
			return node == it.node;
		}

		inline bool operator==(const Iterator &&rhs) const {
			const Iterator it = rhs;
			return *this == it;
		}

		inline bool operator!=(const Node *node) const noexcept {
			return node != node;
		}

		inline bool operator!=(const Iterator &it) const {
			return node != it.node;
		}

		inline bool operator!=(Iterator &&rhs) const {
			Iterator it = rhs;
			return *this != it;
		}

		inline T &operator*() {
			return node->v;
		}

		inline const T &operator*() const {
			return node->v;
		}

		inline T *operator->() {
			return &node->v;
		}

		inline const T *operator->() const {
			return &node->v;
		}
	};

	inline Node *_prepend(Node *where) {
		auto node = new Node(where->pre, where);
		if (where->pre)
			where->pre->next = node;
		where->pre = node;

		if (where == _head)
			_head = where;

		++_curSize;

		return node;
	}
	inline Node *_append(Node *where) {
		assert(where != _end);

		auto node = new Node(where, where->next);
		if (where->next)
			where->next->pre = node;
		where->next = node;

		++_curSize;

		return node;
	}

	inline void _remove(Node *where) {
		if (where->next)
			where->next->pre = where->pre;
		if (where->pre)
			where->pre->next = where->next;
		delete where;
	}

protected:
	Node *_head, *_end;
	size_t _curSize;

public:
	inline List(size_t size = 0) : _curSize(size) {
		_head = new Node(), _end = new Node();
		_head->next = _end, _end->pre = _head;

		while (size--) {
			_append(_head);
		}
	}

	inline ~List() {
		for (Node *i = _head; i != _end;) {
			auto next = i->next;
			remove(i);
			i = next;
		}
		delete _end;
	}

	inline Iterator begin() {
		return Iterator(_head);
	}
	inline Iterator end() {
		return Iterator(_end);
	}

	inline Iterator prepend(Iterator where, T data) {
		auto node = _prepend(where.node);
		node->v = data;

		return Iterator(node);
	}
	inline Iterator append(Iterator where, T data) {
		auto node = _append(where.node);
		node->v = data;

		return Iterator(node);
	}

	inline void remove(Iterator where) {
		_remove(where.node);
	}

	inline T &at(size_t i) {
		Node *node = nullptr;
		if (i > (_curSize >> 1)) {
			node = _end->pre;
			while (++i < _curSize) {
				assert(node);
				node = node->pre;
			}
		} else {
			node = _head->next;
			while (i--) {
				assert(node);
				node = node->next;
			};
		}
		assert(node != _head);
		assert(node != _end);
		assert(node);
		return node->v;
	}

	inline const T &at(size_t i) const {
		return ((List<T> *)this)->at(i);
	}

	inline T get(size_t i) {
		return at(i);
	}

	inline const T get(size_t i) const {
		return at(i);
	}

	inline void clear() {
		for (Node *i = _head->next; i != _end;) {
			auto next = i->next;
			remove(i);
			i = next;
		}
	}

	inline size_t size() const {
		return _curSize;
	}
};

#endif
