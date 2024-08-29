#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef __RBTREE_H__
#define __RBTREE_H__

#if defined (__cplusplus)
extern "C" {
#endif

struct rb_nodes {
	//节点的颜色
	unsigned long  __rb_parent_color;
	//右子节点
	struct rb_nodes *rb_right;
	//左子节点
	struct rb_nodes *rb_left;
} __attribute__((aligned(sizeof(long))));
    /* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_roots {
	struct rb_nodes *rb_node;
};

#define RB_ROOTS	(struct rb_roots) { NULL, }

#define rb_parents(r)   ((struct rb_nodes *)((r)->__rb_parent_color & ~3))

#define ___offsetofs(TYPE, MEMBER)	((size_t)&((TYPE *)0)->MEMBER)
#define ___container_ofs(ptr, type, member) ({			\
		const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
		(type *)( (char *)__mptr - ___offsetofs(type,member) );})
#define	rb_entrys(ptr, type, member) ___container_ofs(ptr, type, member)

#define RB_EMPTY_ROOTS(root)  (READ_ONCE((root)->rb_node) == NULL)

/* 'empty' nodes are nodes that are known not to be inserted in an rbtree */
#define RB_EMPTY_NODES(node)  \
	((node)->__rb_parent_color == (unsigned long)(node))
#define RB_CLEAR_NODES(node)  \
	((node)->__rb_parent_color = (unsigned long)(node))


extern void rb_insert_colors(struct rb_nodes *, struct rb_roots *);
extern void rb_erases(struct rb_nodes *, struct rb_roots *);


/* Find logical next and previous nodes in a tree */
extern struct rb_nodes *rb_nexts(const struct rb_nodes *);
extern struct rb_nodes *rb_prevs(const struct rb_nodes *);
extern struct rb_nodes *rb_firsts(const struct rb_roots *);
extern struct rb_nodes *rb_lasts(const struct rb_roots *);

/* Postorder iteration - always visit the parent after its children */
extern struct rb_nodes *rb_first_postorders(const struct rb_roots *);
extern struct rb_nodes *rb_next_postorders(const struct rb_nodes *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_nodes(struct rb_nodes *victim, struct rb_nodes *news,
			    struct rb_roots *root);
extern void rb_replace_node_rcus(struct rb_nodes *victim, struct rb_nodes *news,
				struct rb_roots *root);

static inline void rb_link_nodes(struct rb_nodes *node, struct rb_nodes *parent,
				struct rb_nodes **rb_link)
{
	node->__rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

static inline void rb_link_node_rcus(struct rb_nodes *node, struct rb_nodes *parent,
				    struct rb_nodes **rb_link)
{
	node->__rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	//rcu_assign_pointer(*rb_link, node);
}

#define rb_entry_safes(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? rb_entrys(____ptr, type, member) : NULL; \
	})

/**
 * rbtree_postorder_for_each_entry_safe - iterate in post-order over rb_roots of
 * given type allowing the backing memory of @pos to be invalidated
 *
 * @pos:	the 'type *' to use as a loop cursor.
 * @n:		another 'type *' to use as temporary storage
 * @root:	'rb_roots *' of the rbtree.
 * @field:	the name of the rb_node field within 'type'.
 *
 * rbtree_postorder_for_each_entry_safe() provides a similar guarantee as
 * list_for_each_entry_safe() and allows the iteration to continue independent
 * of changes to @pos by the body of the loop.
 *
 * Note, however, that it cannot handle other modifications that re-order the
 * rbtree it is iterating over. This includes calling rb_erase() on @pos, as
 * rb_erase() may rebalance the tree, causing us to miss some nodes.
 */
#define rbtree_postorder_for_each_entry_safes(pos, n, root, field) \
	for (pos = rb_entry_safes(rb_first_postorders(root), typeof(*pos), field); \
	     pos && ({ n = rb_entry_safes(rb_next_postorders(&pos->field), \
			typeof(*pos), field); 1; }); \
	     pos = n)


/******************interface********************/

typedef int (*computes)(void *key, void *_key);
typedef void (*prints)(int64_t key);

typedef struct _tag_rbtree_manages {
	void *key;
	struct rb_nodes rb;
} rbtree_manages;

void erases(rbtree_manages *node, struct rb_roots *root);
void inserts(rbtree_manages *node, struct rb_roots *root, computes callback_compute);
rbtree_manages *searchs(void *key, struct rb_roots *root, computes callback_compute);
void traces(struct rb_roots *root, prints print_callback);


/*new interface*/
void *rbtreeSearchs(void *key, struct rb_roots *root, computes callback_compute);
int32_t rbtreeInserts(void *key, struct rb_roots *root, computes callback_compute);
int32_t rbtreeErases(void *key, struct rb_roots *root, computes callback_compute);

#if defined (__cplusplus)
}
#endif

#endif /* __RBTREE_H__*/
