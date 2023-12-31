#include "tree.h"

static void kf_rbtree_lrot(kf_rbtree_t *tree, kf_rbtree_node_t *x);
static void kf_rbtree_rrot(kf_rbtree_t *tree, kf_rbtree_node_t *x);
static void kf_rbtree_insert_fixup(kf_rbtree_t *tree, kf_rbtree_node_t *node);
static kf_rbtree_node_t *kf_rbtree_remove_fixup(kf_rbtree_t *tree, kf_rbtree_node_t *node);
static void kf_rbtree_walknodes4free(kf_rbtree_t *tree, kf_rbtree_node_t *node);

void kf_rbtree_init(kf_rbtree_t *dest,
	kf_rbtree_nodecmp_t node_cmp,
	kf_rbtree_keycmp_t key_cmp,
	kf_rbtree_nodecopy_t node_copy,
	kf_rbtree_nodefree_t node_free) {
	dest->node_cmp = node_cmp;
	dest->key_cmp = key_cmp;
	dest->node_copy = node_copy;
	dest->node_free = node_free;
	dest->root = NULL;
}

void kf_rbtree_insert(kf_rbtree_t *tree, kf_rbtree_node_t *node) {
	assert(!node->l);
	assert(!node->r);

	if (!tree->root) {
		tree->root = node;
		kf_rbtree_setcolor(node, KF_RBTREE_BLACK);
		return;
	}

	kf_rbtree_node_t *x = tree->root, *y = NULL;
	while (x) {
		y = x;
		int result = tree->node_cmp(x, node);
		if (result > 0)
			x = x->l;
		else if (result < 0)
			x = x->r;
		else
			assert(false);
	}

	if (tree->node_cmp(node, y) < 0)
		y->l = node;
	else
		y->r = node;
	kf_rbtree_setparent(node, y);
	kf_rbtree_setcolor(node, KF_RBTREE_RED);

	kf_rbtree_insert_fixup(tree, node);
}

void kf_rbtree_remove(kf_rbtree_t *tree, kf_rbtree_node_t *node) {
	kf_rbtree_node_t *y = kf_rbtree_remove_fixup(tree, node);
	y->r = NULL;
	y->l = NULL;

	tree->node_free(y);
}

kf_rbtree_node_t *kf_rbtree_find(kf_rbtree_t *tree, const void *key) {
	kf_rbtree_node_t *i = tree->root;
	while (i) {
		int result = tree->key_cmp(i, key);
		if (result < 0)
			i = i->r;
		else if (result > 0)
			i = i->l;
		else
			return i;
	}
	return NULL;
}

void kf_rbtree_free(kf_rbtree_t *tree) {
	if (tree->root)
		kf_rbtree_walknodes4free(tree, tree->root);
}

kf_rbtree_node_t *kf_rbtree_getminleaf(kf_rbtree_node_t *node) {
	while (node->l)
		node = node->l;
	return node;
}

kf_rbtree_node_t *kf_rbtree_getmaxleaf(kf_rbtree_node_t *node) {
	while (node->r)
		node = node->r;
	return node;
}

static void kf_rbtree_lrot(kf_rbtree_t *tree, kf_rbtree_node_t *x) {
	kf_rbtree_node_t *y = x->r;
	assert(y);

	x->r = y->l;
	if (y->l)
		kf_rbtree_setparent(y->l, x);

	kf_rbtree_setparent(y, kf_rbtree_parent(x));

	if (!kf_rbtree_parent(x))
		tree->root = y;
	else if (kf_rbtree_parent(x)->l == x)
		kf_rbtree_parent(x)->l = y;
	else
		kf_rbtree_parent(x)->r = y;

	y->l = x;
	kf_rbtree_setparent(x, y);
}

static void kf_rbtree_rrot(kf_rbtree_t *tree, kf_rbtree_node_t *x) {
	kf_rbtree_node_t *y = x->l;
	assert(y);

	x->l = y->r;
	if (y->r)
		kf_rbtree_setparent(y->r, x);

	kf_rbtree_setparent(y, kf_rbtree_parent(x));
	if (!kf_rbtree_parent(x))
		tree->root = y;
	else if (kf_rbtree_parent(x)->l == x)
		kf_rbtree_parent(x)->l = y;
	else
		kf_rbtree_parent(x)->r = y;

	y->r = x;
	kf_rbtree_setparent(x, y);
}

static void kf_rbtree_insert_fixup(kf_rbtree_t *tree, kf_rbtree_node_t *node) {
	kf_rbtree_node_t *p, *gp = node, *u;  // Parent, grandparent and uncle

	while ((p = kf_rbtree_parent(gp)) && kf_rbtree_isred(p)) {
		gp = kf_rbtree_parent(p);

		if (p == gp->l) {
			u = gp->r;

			if (kf_rbtree_isred(u)) {
				kf_rbtree_setcolor(p, KF_RBTREE_BLACK);
				kf_rbtree_setcolor(u, KF_RBTREE_BLACK);
				kf_rbtree_setcolor(gp, KF_RBTREE_RED);
				node = gp;
				continue;
			} else {
				if (node == p->r) {
					kf_rbtree_lrot(tree, p);

					{
						kf_rbtree_node_t *tmp;
						tmp = node;
						node = p;
						p = tmp;
					}
				}
				kf_rbtree_rrot(tree, gp);
				kf_rbtree_setcolor(p, KF_RBTREE_BLACK);
				kf_rbtree_setcolor(gp, KF_RBTREE_RED);
			}
		} else {
			u = gp->l;

			if (kf_rbtree_isred(u)) {
				kf_rbtree_setcolor(p, KF_RBTREE_BLACK);
				kf_rbtree_setcolor(u, KF_RBTREE_BLACK);
				kf_rbtree_setcolor(gp, KF_RBTREE_RED);
				node = gp;
				continue;
			} else {
				if (node == p->l) {
					kf_rbtree_rrot(tree, p);

					{
						kf_rbtree_node_t *tmp;
						tmp = node;
						node = p;
						p = tmp;
					}
				}
				kf_rbtree_lrot(tree, gp);
				kf_rbtree_setcolor(p, KF_RBTREE_BLACK);
				kf_rbtree_setcolor(gp, KF_RBTREE_RED);
			}
		}
	}

	kf_rbtree_setcolor(tree->root, KF_RBTREE_BLACK);
}

static kf_rbtree_node_t *kf_rbtree_remove_fixup(kf_rbtree_t *tree, kf_rbtree_node_t *node) {
	// Originally adopted from SGI STL's stl_tree, with minor improvements.
	kf_rbtree_node_t *y = node, *x, *p;

	if (!y->l)
		// The node has right child only.
		x = y->r;
	else if (!y->r) {
		// The node has left child only.
		x = y->l;
	} else {
		// The node has two children.
		y = kf_rbtree_getminleaf(y->r);
		x = y->r;
	}

	if (y != node) {
		kf_rbtree_setparent(node->l, y);
		y->l = node->l;

		if (y != node->r) {
			p = kf_rbtree_parent(y);
			if (x)
				kf_rbtree_setparent(x, kf_rbtree_parent(y));
			kf_rbtree_parent(y)->l = x;
			y->r = node->r;
			kf_rbtree_setparent(node->r, y);
		} else
			p = y;

		if (tree->root == node)
			tree->root = y;
		else if (kf_rbtree_parent(node)->l == node)
			kf_rbtree_parent(node)->l = y;
		else
			kf_rbtree_parent(node)->r = y;

		kf_rbtree_setparent(y, kf_rbtree_parent(node));

		{
			bool color = kf_rbtree_color(y);
			kf_rbtree_setcolor(y, kf_rbtree_color(node));
			kf_rbtree_setcolor(node, color);
		}
		y = node;
	} else {
		p = kf_rbtree_parent(y);
		if (x)
			kf_rbtree_setparent(x, kf_rbtree_parent(y));

		if (tree->root == node)
			tree->root = x;
		else if (kf_rbtree_parent(node)->l == node)
			kf_rbtree_parent(node)->l = x;
		else
			kf_rbtree_parent(node)->r = x;
	}

	if (kf_rbtree_isblack(y)) {
		while (x != tree->root && kf_rbtree_isblack(x)) {
			if (x == p->l) {
				kf_rbtree_node_t *w = p->r;

				if (kf_rbtree_isred(w)) {
					kf_rbtree_setcolor(w, KF_RBTREE_BLACK);
					kf_rbtree_setcolor(p, KF_RBTREE_RED);
					kf_rbtree_lrot(tree, p);
					w = p->r;
				}

				if (kf_rbtree_isblack(w->l) && kf_rbtree_isblack(w->r)) {
					kf_rbtree_setcolor(w, KF_RBTREE_RED);
					x = p;
					p = kf_rbtree_parent(p);
				} else {
					if (kf_rbtree_isblack(w->r)) {
						if (w->l)
							kf_rbtree_setcolor(w->l, KF_RBTREE_BLACK);
						kf_rbtree_setcolor(w, KF_RBTREE_RED);
						kf_rbtree_rrot(tree, w);
						w = p->r;
					}
					kf_rbtree_setcolor(w, kf_rbtree_color(p));
					kf_rbtree_setcolor(p, KF_RBTREE_BLACK);
					if (w->r)
						kf_rbtree_setcolor(w->r, KF_RBTREE_BLACK);
					kf_rbtree_lrot(tree, p);
					break;
				}
			} else {
				kf_rbtree_node_t *w = p->l;

				if (kf_rbtree_isred(w)) {
					kf_rbtree_setcolor(w, KF_RBTREE_BLACK);
					kf_rbtree_setcolor(p, KF_RBTREE_RED);
					kf_rbtree_rrot(tree, p);
					w = p->l;
				}

				if (kf_rbtree_isblack(w->r) && kf_rbtree_isblack(w->l)) {
					kf_rbtree_setcolor(w, KF_RBTREE_RED);
					x = p;
					p = kf_rbtree_parent(p);
				} else {
					if (kf_rbtree_isblack(w->l)) {
						if (w->r)
							kf_rbtree_setcolor(w->r, KF_RBTREE_BLACK);
						kf_rbtree_setcolor(w, KF_RBTREE_RED);
						kf_rbtree_lrot(tree, w);
						w = p->l;
					}
					kf_rbtree_setcolor(w, kf_rbtree_color(p));
					kf_rbtree_setcolor(p, KF_RBTREE_BLACK);
					if (w->l)
						kf_rbtree_setcolor(w->l, KF_RBTREE_BLACK);
					kf_rbtree_rrot(tree, p);
					break;
				}
			}
		}
		if (x)
			kf_rbtree_setcolor(x, KF_RBTREE_BLACK);
	}

	return y;
}

static void kf_rbtree_walknodes4free(kf_rbtree_t *tree, kf_rbtree_node_t *node) {
	if (node->l)
		kf_rbtree_walknodes4free(tree, node->l);
	if (node->r)
		kf_rbtree_walknodes4free(tree, node->r);
	tree->node_free(node);
}

static inline void _verify(kf_rbtree_node_t *node, const size_t nBlack, size_t cntBlack) {
	if (!node) {
		// We have reached a terminal node.
		if (nBlack != cntBlack)
			assert(false);
		return;
	}

	if (kf_rbtree_isred(node) && kf_rbtree_isred(kf_rbtree_parent(node)))
		assert(false);

	if (kf_rbtree_isblack(node))
		++cntBlack;

	_verify(node->l, nBlack, cntBlack);
	_verify(node->r, nBlack, cntBlack);
}

void kf_rbtree_verify(kf_rbtree_t *tree) {
	if (!tree->root)
		return;

	if (kf_rbtree_isred(tree->root))
		assert(false);

	size_t nBlack = 0;
	for (kf_rbtree_node_t *i = tree->root; i; i = i->l) {
		if (kf_rbtree_isblack(i))
			++nBlack;
	}

	_verify(tree->root, nBlack, 0);
}

kf_rbtree_node_t *kf_rbtree_next(kf_rbtree_node_t *node) {
	if (!node)
		return NULL;

	if (node->r) {
		node = kf_rbtree_getminleaf(node->r);
	} else {
		while (kf_rbtree_parent(node) && (node == kf_rbtree_parent(node)->r))
			node = kf_rbtree_parent(node);
		node = kf_rbtree_parent(node);
	}

	return node;
}
