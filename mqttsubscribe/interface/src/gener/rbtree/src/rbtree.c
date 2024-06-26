#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree_augmented.h"
#include "rbtrees.h"
#include <rbtree.h>

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


void *rbtreeSearchs(void *key, struct rb_roots *root, computes callback_compute)
{
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

int32_t rbtreeInserts(void *key, struct rb_roots *root, computes callback_compute)
{
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

int32_t rbtreeErases(void *key, struct rb_roots *root, computes callback_compute) {
    rbtree_manages *node = searchs(key, root, callback_compute);
    if (node) {
        erases(node, root);
        free(node);
        return 0;
    }

    return -1;
}

static int32_t dep_levels(struct rb_nodes *node) {
	int32_t n_left		= 0;
	int32_t n_right		= 0;

	if (NULL == node) {
		return 0;
	}

	n_left		= dep_levels(node->rb_left);
	n_right		= dep_levels(node->rb_right);

	return n_left >n_right ? n_left + 1 : n_right + 1;

}

int32_t levels(struct rb_roots *root) {
	return dep_levels(root->rb_node);
}

int32_t _pows(int32_t a, int32_t n) {
	int32_t index = 0; 
	int32_t value = a;

	switch (n) {
		case 0:
			return 1;
		case 1:
			return a;
		default:
			{
				for (index = 0; index < n - 1; index++) {
					value *= a;
				}
				break;
			}
	}

	return value;
}
#ifdef OSA
#include <osa_que.h>
#endif

void traces(struct rb_roots *root, prints print_callback) {
	int32_t top_level	= 0;
	int32_t v_level		= 0;
#ifdef OSA
	int32_t status		= -1;
	void *pointer	= NULL;
	int32_t w_pointer	= 0;
	int32_t r_pointer	= 0;
	struct rb_nodes *node = NULL;
	rbtree_manages *manage = NULL;
#endif

	if (NULL == root) {
		return;
	}

	top_level = levels(root);
	if (!top_level) {
		printf ("tree is empty!\n");
		return;
	}

	printf ("top_level : %d\n", top_level);

	v_level = _pows(2, (top_level - 1));
	printf ("v level value :%d\n", v_level);

	if (!v_level) {
		return;
	}

#ifdef OSA
	OSA_QueHndl hndl;
	if (OSA_queCreate(&hndl, v_level)) return;

	status = OSA_quePut(&hndl, (int32_t)root->rb_nodes, OSA_TIMEOUT_NONE);
	if (OSA_SOK == status) {
		while (1) {
			if (OSA_queIsEmpty(&hndl)) {
				break;
			}
			status = OSA_queGet(&hndl, (int32_t *)&node, OSA_TIMEOUT_NONE);
			if (OSA_SOK == status) {
				manage = rb_entrys(node, rbtree_manages, rb);
				print32_t_callback(manage->key);

				if (node->rb_left) {
					OSA_quePut(&hndl, (int32_t)node->rb_left, OSA_TIMEOUT_NONE);
				}

				if (node->rb_right) {
					OSA_quePut(&hndl, (int32_t)node->rb_right, OSA_TIMEOUT_NONE);
				}
			}
		}
	}
#endif
}

