#ifndef __TREE_H__
#define __TREE_H__

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "mutex.h"
#include "lockguard.h"

template <typename T>
class RBTree {
public:
	constexpr static bool BLACK = false, RED = true;

	struct Node {
		Node *p = nullptr, *l = nullptr, *r = nullptr;
		T value;
		bool color;

		inline Node() = default;
		inline Node(const Node &node) = default;
		inline Node(T value) : value(value) {}

		inline ~Node() {
			if (l)
				delete l;
			if (r)
				delete r;
		}
	};

private:
	Mutex _mutex;
	Node *_root = nullptr;
	Node *_cachedMinNode = nullptr, *_cachedMaxNode = nullptr;
	size_t _nNodes = 0;

	static inline Node *_getMinNode(Node *node) {
		if (!node)
			return node;

		while (node->l)
			node = node->l;
		return node;
	}

	static inline Node *_getMaxNode(Node *node) {
		if (!node)
			return node;

		while (node->r)
			node = node->r;
		return node;
	}

	static inline bool _isRed(Node *node) { return node && node->color == RED; }
	static inline bool _isBlack(Node *node) { return (!node) || node->color == BLACK; }

	inline void _lRot(Node *x) {
		Node *y = x->r;
		assert(y);

		x->r = y->l;
		if (y->l)
			y->l->p = x;

		y->p = x->p;

		if (!x->p)
			_root = y;
		else if (x->p->l == x)
			x->p->l = y;
		else
			x->p->r = y;

		y->l = x;
		x->p = y;
	}

	inline void _rRot(Node *x) {
		Node *y = x->l;
		assert(y);

		x->l = y->r;
		if (y->r)
			y->r->p = x;

		y->p = x->p;
		if (!x->p)
			_root = y;
		else if (x->p->l == x)
			x->p->l = y;
		else
			x->p->r = y;

		y->r = x;
		x->p = y;
	}

	inline Node *_get(T value) {
		Node *i = _root;
		while (i) {
			if (i->value < value)
				i = i->r;
			else if (i->value > value)
				i = i->l;
			else
				return i;
		}
		return nullptr;
	}

	inline void _insertFixUp(Node *node) {
		Node *p, *gp = node, *u;  // Parent, grandparent and uncle

		while ((p = gp->p) && _isRed(p)) {
			gp = p->p;

			if (p == gp->l) {
				u = gp->r;

				if (_isRed(u)) {
					p->color = BLACK;
					u->color = BLACK;
					gp->color = RED;
					node = gp;
					continue;
				} else {
					if (node == p->r) {
						_lRot(p);
						std::swap(node, p);
					}
					_rRot(gp);
					p->color = BLACK;
					gp->color = RED;
				}
			} else {
				u = gp->l;

				if (_isRed(u)) {
					p->color = BLACK;
					u->color = BLACK;
					gp->color = RED;
					node = gp;
					continue;
				} else {
					if (node == p->l) {
						_rRot(p);
						std::swap(node, p);
					}
					_lRot(gp);
					p->color = BLACK;
					gp->color = RED;
				}
			}
		}

		_root->color = BLACK;
	}

	inline void _insert(Node *node) {
		assert(!node->l);
		assert(!node->r);

		if (!_root) {
			_root = node;
			node->color = BLACK;
			goto updateNodeCaches;
		}

		{
			Node *x = _root, *y = nullptr;
			while (x) {
				y = x;

				if (x->value > node->value)
					x = x->l;
				else if (x->value < node->value)
					x = x->r;
				else
					throw std::runtime_error("Entry exists");
			}

			if (node->value < y->value)
				y->l = node;
			else
				y->r = node;
			node->p = y;
			node->color = RED;

			_insertFixUp(node);
		}

	updateNodeCaches:
		_cachedMinNode = _getMinNode(_root);
		_cachedMaxNode = _getMaxNode(_root);

		++_nNodes;
	}

	inline Node *_removeFixUp(Node *node) {
		// Originally adopted from SGI STL's stl_tree, with minor improvements.
		Node *y = node, *x, *p;

		if (!y->l)
			// The node has right child only.
			x = y->r;
		else if (!y->r) {
			// The node has left child only.
			x = y->l;
		} else {
			// The node has two children.
			y = _getMinNode(y->r);
			x = y->r;
		}

		if (y != node) {
			node->l->p = y;
			y->l = node->l;

			if (y != node->r) {
				p = y->p;
				if (x)
					x->p = y->p;
				y->p->l = x;
				y->r = node->r;
				node->r->p = y;
			} else
				p = y;

			if (_root == node)
				_root = y;
			else if (node->p->l == node)
				node->p->l = y;
			else
				node->p->r = y;

			y->p = node->p;
			std::swap(y->color, node->color);
			y = node;
		} else {
			p = y->p;
			if (x)
				x->p = y->p;

			if (_root == node)
				_root = x;
			else if (node->p->l == node)
				node->p->l = x;
			else
				node->p->r = x;
		}

		if (_isBlack(y)) {
			while (x != _root && _isBlack(x)) {
				if (x == p->l) {
					auto w = p->r;

					if (_isRed(w)) {
						w->color = BLACK;
						p->color = RED;
						_lRot(p);
						w = p->r;
					}

					if (_isBlack(w->l) && _isBlack(w->r)) {
						w->color = RED;
						x = p;
						p = p->p;
					} else {
						if (_isBlack(w->r)) {
							if (w->l)
								w->l->color = BLACK;
							w->color = RED;
							_rRot(w);
							w = p->r;
						}
						w->color = p->color;
						p->color = BLACK;
						if (w->r)
							w->r->color = BLACK;
						_lRot(p);
						break;
					}
				} else {
					auto w = p->l;

					if (_isRed(w)) {
						w->color = BLACK;
						p->color = RED;
						_rRot(p);
						w = p->l;
					}

					if (_isBlack(w->r) && _isBlack(w->l)) {
						w->color = RED;
						x = p;
						p = p->p;
					} else {
						if (_isBlack(w->l)) {
							if (w->r)
								w->r->color = BLACK;
							w->color = RED;
							_lRot(w);
							w = p->l;
						}
						w->color = p->color;
						p->color = BLACK;
						if (w->l)
							w->l->color = BLACK;
						_rRot(p);
						break;
					}
				}
			}
			if (x)
				x->color = BLACK;
		}

		return y;
	}

	inline void _remove(Node *node) {
		auto y = _removeFixUp(node);
		y->r = nullptr; 
		y->l = nullptr;
		delete y;

		_cachedMinNode = _getMinNode(_root);
		_cachedMaxNode = _getMaxNode(_root);

		--_nNodes;
	}

	inline void _verify(Node *node, const size_t nBlack, size_t cntBlack) {
		if (!node) {
			// We have reached a terminal node.
			if (nBlack != cntBlack)
				throw std::logic_error("Inequal black node counts detected");
			return;
		}

		if (_isRed(node) && _isRed(node->p))
			throw std::logic_error("Connected red nodes detected");

		if (_isBlack(node))
			++cntBlack;

		_verify(node->l, nBlack, cntBlack);
		_verify(node->r, nBlack, cntBlack);
	}

public:
	struct Iterator {
		Node *node;
		RBTree *tree;

		inline Iterator(const Iterator &it) : node(it.node), tree(it.tree) {}
		inline Iterator(const Iterator &&it) : node(it.node), tree(it.tree) {}
		inline Iterator(Node *node, RBTree *tree) : node(node), tree(tree) {}

		inline Iterator &operator=(const Iterator &rhs) noexcept {
			node = rhs.node;
			tree = rhs.tree;
			return *this;
		}

		inline Iterator &operator=(const Iterator &&rhs) noexcept {
			node = rhs.node;
			tree = rhs.tree;
			return *this;
		}

		inline Iterator &operator++() {
			if (!node)
				throw std::logic_error("Increasing the end iterator");

			if (node != tree->_cachedMaxNode) {
				if (node->r) {
					node = _getMinNode(node->r);
				} else {
					while (node->p && (node == node->p->r))
						node = node->p;
					node = node->p;
				}
			} else {
				node = nullptr;
			}

			return *this;
		}

		inline Iterator operator++(int) {
			Iterator it = *this;
			++(*this);
			return it;
		}

		inline Iterator &operator--() {
			if (node == tree->_cachedMinNode)
				throw std::logic_error("Dereasing the begin iterator");

			if (!node)
				node = tree->_cachedMaxNode;
			else {
				if (node->l) {
					node = _getMaxNode(node->l);
				} else {
					while (node->p && (node == node->p->l))
						node = node->p;
					node = node->p;
				}
			}

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
			if (tree != it.tree)
				throw std::logic_error("Cannot compare iterators from different trees");
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
			if (tree != it.tree)
				throw std::logic_error("Cannot compare iterators from different trees");
			return node != it.node;
		}

		inline bool operator!=(Iterator &&rhs) const {
			Iterator it = rhs;
			return *this != it;
		}

		inline T &operator*() {
			if (!node)
				throw std::logic_error("Deferencing the end iterator");
			return node->value;
		}

		inline const T &operator*() const {
			if (!node)
				throw std::logic_error("Deferencing the end iterator");
			return node->value;
		}

		inline T *operator->() {
			if (!node)
				throw std::logic_error("Deferencing the end iterator");
			return &node->value;
		}

		inline const T *operator->() const {
			if (!node)
				throw std::logic_error("Deferencing the end iterator");
			return &node->value;
		}
	};

	Iterator begin() {
		return Iterator(_cachedMinNode, this);
	}

	Iterator end() {
		return Iterator(nullptr, this);
	}

	virtual inline ~RBTree() {
		delete _root;
	}

	inline Node *get(T value) {
		LockGuard<Mutex> lg(_mutex);

		return _get(value);
	}

	inline void insert(Node *node) {
		LockGuard<Mutex> lg(_mutex);

		_insert(node);
	}

	inline void insert(T value) {
		insert(new Node(value));
	}

	inline void remove(Node *node) {
		LockGuard<Mutex> lg(_mutex);

		_remove(node);
	}

	inline void remove(T value) {
		LockGuard<Mutex> lg(_mutex);

		Node *node = _get(value);
		if (!node)
			throw std::invalid_argument("No such entry");

		_remove(node);
	}

	inline bool has(T value) {
		LockGuard<Mutex> lg(_mutex);
		return _get(value) != nullptr;
	}

	inline void verify() {
		LockGuard<Mutex> lg(_mutex);
		if (!_root)
			return;

		if (_isRed(_root))
			throw std::logic_error("Red root node detected");

		size_t nBlack = 0;
		for (Node *i = _root; i; i = i->l) {
			if (_isBlack(i))
				++nBlack;
		}

		_verify(_root, nBlack, 0);
	}

	inline void clear() {
		LockGuard<Mutex> lg(_mutex);
		delete _root;
		_root = nullptr;
		_cachedMinNode = nullptr, _cachedMaxNode = nullptr;
		_nNodes = 0;
	}

	inline size_t size() const {
		return _nNodes;
	}
};

#endif
