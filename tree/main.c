#include "tree.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct _mynode_t {
	kf_rbtree_node_t node;
	int key;
} mynode_t;

static int mynode_nodecmp(const kf_rbtree_node_t *x, const kf_rbtree_node_t *y) {
	const mynode_t *_x = (const mynode_t *)x, *_y = (const mynode_t *)y;

	if (_x->key > _y->key)
		return 1;
	else if (_x->key < _y->key)
		return -1;

	return 0;
}

static int mynode_keycmp(const kf_rbtree_node_t *x, const void *key) {
	const mynode_t *_x = (const mynode_t *)x;
	int _key = (int)key;

	if (_x->key > _key)
		return 1;
	else if (_x->key < _key)
		return -1;

	return 0;
}

static void mynode_nodecopy(kf_rbtree_node_t *dest, const kf_rbtree_node_t *src) {
	mynode_t *_dest = (mynode_t *)dest, *_src = (mynode_t *)src;

	_dest->key = _src->key;
}

static void mynode_nodefree(kf_rbtree_node_t *p) {
	free(p);
}

int main() {
	kf_rbtree_t *tree = malloc(sizeof(kf_rbtree_t));

	kf_rbtree_init(
		tree,
		mynode_nodecmp,
		mynode_keycmp,
		mynode_nodecopy,
		mynode_nodefree);

	for (int i = 0; i < 64; i++) {
		int j = i & 1 ? i : 128 - i;

		mynode_t *node = malloc(sizeof(mynode_t));
		memset(node, 0, sizeof(*node));
		node->key = j;

		printf("Inserting: %d\n", j);
		kf_rbtree_insert(tree, node);
		kf_rbtree_verify(tree);
	}

	for (int i = 0; i < 64; i++) {
		int j = i & 1 ? i : 128 - i;
		printf("Removing: %d\n", j);

		kf_rbtree_remove(tree, kf_rbtree_find(tree, j));

		for (kf_rbtree_node_t *i = kf_rbtree_begin(tree); i; i = kf_rbtree_next(i))
			printf("%d\n", ((mynode_t *)i)->key);

		kf_rbtree_verify(tree);
	}

	kf_rbtree_free(tree);
	free(tree);
}
