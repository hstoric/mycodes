#ifndef __SET_H__
#define __SET_H__

#include <cassert>
#include <stdexcept>
#include <iostream>

template <typename K, typename V>
class Set {
protected:
	enum class Color : bool {
		BLACK = false,
		RED = true
	};

	struct Node {
		Node *p = nullptr, *l = nullptr, *r = nullptr;
		K key;
		V value;
		Color color;

		inline Node() = default;
		inline Node(const Node &node) = default;
		inline Node(K key) : key(key) {}
		inline Node(K key, V value) : key(key), value(value) {}

		inline ~Node() {
			if (l)
				delete l;
			if (r)
				delete r;
		}

		inline Node *getMinNode() {
			Node *i = this;
			if (i->l)
				return i->l->getMinNode();
			return i;
		}

		inline Node *getMaxNode() {
			Node *i = this;
			if (i->r)
				return i->r->getMaxNode();
			return i;
		}
	};

	Node *_root = nullptr;

	inline bool _isRed(Node *node) { return node && node->color == Color::RED; }
	inline bool _isBlack(Node *node) { return (!node) || node->color == Color::BLACK; }

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

	inline void _insertFixUp(Node *node) {
		Node *p, *gp = node, *u;  // Parent, grandparent and uncle

		while ((p = gp->p) && _isRed(p)) {
			gp = p->p;

			if (p == gp->l) {
				u = gp->r;

				if (_isRed(u)) {
					p->color = Color::BLACK;
					u->color = Color::BLACK;
					gp->color = Color::RED;
					node = gp;
					continue;
				} else {
					if (node == p->r) {
						_lRot(p);
						std::swap(node, p);
					}
					_rRot(gp);
					p->color = Color::BLACK;
					gp->color = Color::RED;
				}
			} else {
				u = gp->l;

				if (_isRed(u)) {
					p->color = Color::BLACK;
					u->color = Color::BLACK;
					gp->color = Color::RED;
					node = gp;
					continue;
				} else {
					if (node == p->l) {
						_rRot(p);
						std::swap(node, p);
					}
					_lRot(gp);
					p->color = Color::BLACK;
					gp->color = Color::RED;
				}
			}
		}

		_root->color = Color::BLACK;
	}

	inline void _insert(Node *node) {
		assert(!node->l);
		assert(!node->r);

		if (!_root) {
			_root = node;
			node->color = Color::BLACK;
			return;
		}

		Node *x = _root, *y = nullptr;
		while (x) {
			y = x;

			if (x->key > node->key)
				x = x->l;
			else if (x->key < node->key)
				x = x->r;
			else
				throw std::runtime_error("Entry exists");
		}

		if (node->key < y->key)
			y->l = node;
		else
			y->r = node;
		node->p = y;
		node->color = Color::RED;

		_insertFixUp(node);
	}

	inline Node *_get(K key) {
		Node *i = _root;
		while (i) {
			if (i->key < key)
				i = i->r;
			else if (i->key > key)
				i = i->l;
			else
				return i;
		}
		return nullptr;
	}

	inline void _verify(Node *node, const size_t nBlack, size_t cntBlack) {
		if (!node) {
			// We have reached a terminal node.
			if (nBlack != cntBlack)
				throw std::logic_error("Inequal black node count");
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
	virtual inline ~Map() {
		delete _root;
	}

	inline void verify() {
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

	inline V &get(K key) {
		Node *node = _get(key);
		if (!node)
			throw std::invalid_argument("No such entry");
		return node->value;
	}

	inline void _removeFixUp(Node *node) {
		while (node != _root && _isBlack(node)) {
			Node *b = NULL;	 // Brother

			if (node == node->p->l) {
				// Node is the left child.
				b = node->p->r;

				// The brother is red.
				if (_isRed(b)) {
					b->color = Color::BLACK;
					b->p->color = Color::RED;
					_lRot(b->p);
				}

				// The brother is black.
				if (_isBlack(b)) {
					// Its children are all black.
					if ((_isBlack(b->l)) && (_isBlack(b->r))) {
						b->color = Color::RED;
						node = node->p;
					} else {
						if ((_isRed(b->l)) && (_isBlack(b->r))) {
							b->l->color = Color::BLACK;
							b->color = Color::RED;
							_rRot(b);
							b = node->p->r;
						}
						b->color = node->p->color;
						node->p->color = Color::BLACK;
						if (_isRed(b->r))
							b->r->color = Color::BLACK;
						_lRot(node->p);
						break;
					}
				}
			} else {
				// Node is the right child.
				b = node->p->l;

				// The brother is red.
				if (_isRed(b)) {
					b->color = Color::BLACK;
					b->p->color = Color::RED;
					_rRot(b->p);
				}

				// The brother is black.
				if (_isBlack(b)) {
					// Its children are all black.
					if ((_isBlack(b->l)) && (_isBlack(b->r))) {
						b->color = Color::RED;
						node = node->p;
					} else {
						if ((_isRed(b->r)) && (_isBlack(b->l))) {
							b->r->color = Color::BLACK;
							b->color = Color::RED;
							_lRot(b);
							b = node->p->l;
						}
						b->color = node->p->color;
						node->p->color = Color::BLACK;
						if (_isRed(b->l))
							b->l->color = Color::BLACK;
						_rRot(node->p);
						break;
					}
				}
			}
		}
		node->color = Color::BLACK;
	}

	inline void remove(K key) {
		Node *node = _get(key);
		if (!node)
			throw std::invalid_argument("No such entry");

		Color color;
		bool rebalance = false;

		if (node->l && node->r) {
			// 3
			// Has both children
			Node *successor = node->r->getMinNode();

			node->key = successor->key;
			node->value = successor->value;
			successor->p->l = nullptr;

			node = successor;

			if (_isBlack(successor))
				_removeFixUp(node);
		} else if (node->l) {
			// 2
			// Only has left child
			if (node->p->l == node)
				node->p->l = node->l;
			else
				node->p->r = node->l;
			node->l->color = Color::BLACK;
		} else if (node->r) {
			// 2
			// Only has right child
			if (node->p->l == node)
				node->p->l = node->r;
			else
				node->p->r = node->r;
			node->r->color = Color::BLACK;
		} else {
			// 1
			// Has no child
			if (node->p->l == node)
				node->p->l = nullptr;
			else
				node->p->r = nullptr;
			if (_isBlack(node)) {
				_removeFixUp(node);
				// Structure of the tree will be correct if you uncomment
				// this statement, but the colors are still incorrect.
				// It seems like the fix up method did not rotate and colorize
				// some nodes correctly.
				// |
				// V
				//_rRot(_root->r);
			}
		}

		// Free the node.
		node->l = nullptr;
		node->r = nullptr;
		delete node;
	}

	inline bool has(K key) {
		return _get(key) != nullptr;
	}

	inline V &operator[](K key) {
		Node *node = _get(key);
		if (node)
			return node->value;

		node = new Node(key);
		_insert(node);
		return node->value;
	}

	void dump(std::ostream &os, Node *node, bool isLeft, std::string indent) {
		if (node->r)
			dump(os, node->r, false, indent + (isLeft ? "|   " : "    "));

		os << indent
		   << "+---"
		   << (node->color == Color::RED ? 'R' : 'B') << ':'
		   << node->key
		   << '\n';

		if (node->l)
			dump(os, node->l, true, indent + (isLeft ? "    " : "|   "));
	}

	void dump(std::ostream &os) {
		if (!_root) {
			os << "(empty)"
			   << "\n";
			return;
		}

		if (_root->r)
			dump(os, _root->r, false, "");
		os << (_root->color == Color::RED ? 'R' : 'B') << ' '
		   << _root->key
		   << '\n';
		if (_root->l)
			dump(os, _root->l, true, "");
	}
};

#endif
