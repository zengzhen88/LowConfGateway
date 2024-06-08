#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree_augmented.h"
#include "rbtrees.h"
#include <rbtree.h>


typedef struct _tag_rbtree_manages {
	void *key;
	struct rb_nodes rb;
} rbtree_manages;

void erases(rbtree_manages *node, struct rb_roots *root);
void inserts(rbtree_manages *node, struct rb_roots *root, computes callback_compute);
rbtree_manages *searchs(void *key, struct rb_roots *root, computes callback_compute);
void traces(struct rb_roots *root, prints print_callback);

rbtree_manages *searchs(void *key, struct rb_roots *root, computes callback_compute)
{
	int32_t status = -1;
	struct rb_nodes **new = &root->rb_node, *parent = NULL;

	//new指代当前的根节点

	while (*new) {
		/*指向当前的节点*/
		parent = *new;
		/*查找合适的位置插入节点*/
		status = callback_compute(key, (void *)((rb_entrys(parent, rbtree_manages, rb)->key)));
		if (0 > status)
			//左节点有效
			new = &parent->rb_left;
		else if (0 < status)
			//可节点有效
			new = &parent->rb_right;
		else {
			return rb_entrys(parent, rbtree_manages, rb);
		}
	}

	return NULL;
}

void *rbtreeCreate(void) {
    struct rb_roots *root = (struct rb_roots *)(malloc (sizeof(*root)));
    if (root) {
        root->rb_node = NULL;
        return root;
    }

    return NULL;
}

void *rbtreeSearchs(void *key, void *sroot, computes callback_compute)
{
    struct rb_roots *root = (struct rb_roots *)sroot;
    rbtree_manages *node =  (void *)((searchs(key, root, callback_compute)));
    if (node) {
        return node->key;
    }

    return NULL;
}

void inserts(rbtree_manages *node, struct rb_roots *root, computes callback_compute)
{
	struct rb_nodes **new = &root->rb_node, *parent = NULL;
	void *key = node->key;

	//new指代当前的根节点

	while (*new) {
		/*指向当前的节点*/
		parent = *new;
		/*查找合适的位置插入节点*/
		if (0 > callback_compute(key, (void *)rb_entrys(parent, rbtree_manages, rb)->key))
			//左节点有效
			new = &parent->rb_left;
		else
			//可节点有效
			new = &parent->rb_right;
	}

	rb_link_nodes(&node->rb, parent, new);
	rb_insert_colors(&node->rb, root);
}

int32_t rbtreeInserts(void *key, void *sroot, computes callback_compute)
{
    struct rb_roots *root = (struct rb_roots *)sroot;
    rbtree_manages *node = (rbtree_manages *) malloc (sizeof(*node));
    if (node) {
        memset(node, 0x0, sizeof(*node));

        node->key = key;
        inserts(node, root, callback_compute);

        return 0;
    }

    return -1;
}

void erases(rbtree_manages *node, struct rb_roots *root)
{
	rb_erases(&node->rb, root);
}

int32_t rbtreeErases(void *key, void *sroot, computes callback_compute) {
    struct rb_roots *root = (struct rb_roots *)sroot;
    rbtree_manages *node = searchs(key, root, callback_compute);
    if (node) {
        erases(node, root);
        free(node);
        return 0;
    }

    return -1;
}


