/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  (C) 2002  David Woodhouse <dwmw2@infradead.org>
  (C) 2012  Michel Lespinasse <walken@google.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  linux/include/linux/rbtree_augmented.h
*/

#ifndef _LINUX_RBTREE_AUGMENTED_H
#define _LINUX_RBTREE_AUGMENTED_H

//#include <linux/compiler.h>
#include <rbtree.h>
#include <rbtrees.h>

/*
 * Please note - only struct rb_augment_callbackss and the prototypes for
 * rb_insert_augmented() and rb_erase_augmented() are intended to be public.
 * The rest are implementation details you are not expected to depend on.
 *
 * See Documentation/rbtree.txt for documentation and samples.
 */

struct rb_augment_callbackss {
	void (*propagate)(struct rb_nodes *node, struct rb_nodes *stop);
	void (*copy)(struct rb_nodes *old, struct rb_nodes *new);
	void (*rotate)(struct rb_nodes *old, struct rb_nodes *new);
};

extern void __rb_insert_augmenteds(struct rb_nodes *node, struct rb_roots *root,
	void (*augment_rotate)(struct rb_nodes *old, struct rb_nodes *new));
/*
 * Fixup the rbtree and update the augmented information when rebalancing.
 *
 * On insertion, the user must update the augmented information on the path
 * leading to the inserted node, then call rb_link_node() as usual and
 * rb_augment_inserted() instead of the usual rb_insert_color() call.
 * If rb_augment_inserted() rebalances the rbtree, it will callback into
 * a user provided function to update the augmented information on the
 * affected subtrees.
 */
static inline void
rb_insert_augmenteds(struct rb_nodes *node, struct rb_roots *root,
		    const struct rb_augment_callbackss *augment)
{
	__rb_insert_augmenteds(node, root, augment->rotate);
}

#define RB_DECLARE_CALLBACKS(rbstatic, rbname, rbstruct, rbfield,	\
			     rbtype, rbaugmented, rbcomputes)		\
static inline void							\
rbname ## _propagates(struct rb_nodes *rb, struct rb_nodes *stop)		\
{									\
	while (rb != stop) {						\
		rbstruct *node = rb_entrys(rb, rbstruct, rbfield);	\
		rbtype augmented = rbcomputes(node);			\
		if (node->rbaugmented == augmented)			\
			break;						\
		node->rbaugmented = augmented;				\
		rb = rb_parents(&node->rbfield);				\
	}								\
}									\
static inline void							\
rbname ## _copys(struct rb_nodes *rb_old, struct rb_nodes *rb_new)		\
{									\
	rbstruct *old = rb_entrys(rb_old, rbstruct, rbfield);		\
	rbstruct *new = rb_entrys(rb_new, rbstruct, rbfield);		\
	new->rbaugmented = old->rbaugmented;				\
}									\
static void								\
rbname ## _rotates(struct rb_nodes *rb_old, struct rb_nodes *rb_new)	\
{									\
	rbstruct *old = rb_entrys(rb_old, rbstruct, rbfield);		\
	rbstruct *new = rb_entrys(rb_new, rbstruct, rbfield);		\
	new->rbaugmented = old->rbaugmented;				\
	old->rbaugmented = rbcomputes(old);				\
}									\
rbstatic const struct rb_augment_callbackss rbname = {			\
	rbname ## _propagates, rbname ## _copys, rbname ## _rotates	\
};


#define	RB_REDS		0
#define	RB_BLACKS	1

#define __rb_parents(pc)    ((struct rb_nodes *)(pc & ~3))

#define __rb_colors(pc)     ((pc) & 1)
#define __rb_is_blacks(pc)  __rb_colors(pc)
#define __rb_is_reds(pc)    (!__rb_colors(pc))
#define rb_colors(rb)       __rb_colors((rb)->__rb_parent_color)
#define rb_is_reds(rb)      __rb_is_reds((rb)->__rb_parent_color)
#define rb_is_blacks(rb)    __rb_is_blacks((rb)->__rb_parent_color)

static inline void rb_set_parents(struct rb_nodes *rb, struct rb_nodes *p)
{
	rb->__rb_parent_color = rb_colors(rb) | (unsigned long)p;
}

static inline void rb_set_parent_colors(struct rb_nodes *rb,
				       struct rb_nodes *p, int color)
{
	//为当前节点设置父节点及节点颜色
	rb->__rb_parent_color = (unsigned long)p | color;
}

#define WRITE_ONCES(x, value) x=value

static inline void
__rb_change_childs(struct rb_nodes *old, struct rb_nodes *new,
		  struct rb_nodes *parent, struct rb_roots *root)
{
	if (parent) {
		if (parent->rb_left == old)
			WRITE_ONCES(parent->rb_left, new);
		else
			WRITE_ONCES(parent->rb_right, new);
	} else
		WRITE_ONCES(root->rb_node, new);
}

#if 0
static inline void
__rb_change_child_rcu(struct rb_nodes *old, struct rb_nodes *new,
		      struct rb_nodes *parent, struct rb_roots *root)
{
	if (parent) {
		if (parent->rb_left == old)
			rcu_assign_pointer(parent->rb_left, new);
		else
			rcu_assign_pointer(parent->rb_right, new);
	} else
		rcu_assign_pointer(root->rb_node, new);
}
#endif

extern void __rb_erase_colors(struct rb_nodes *parent, struct rb_roots *root,
	void (*augment_rotate)(struct rb_nodes *old, struct rb_nodes *new));

static inline struct rb_nodes *
__rb_erase_augmenteds(struct rb_nodes *node, struct rb_roots *root,
		     const struct rb_augment_callbackss*augment)
{
	struct rb_nodes *child = node->rb_right;
	struct rb_nodes *tmp = node->rb_left;
	struct rb_nodes *parent, *rebalance;
	unsigned long pc;

	if (!tmp) {
		/*
		 * Case 1: node to erase has no more than 1 child (easy!)
		 *
		 * Note that if there is one child it must be red due to 5)
		 * and node must be black due to 4). We adjust colors locally
		 * so as to bypass __rb_erase_colors() later on.
		 */
		pc = node->__rb_parent_color;
		parent = __rb_parents(pc);
		__rb_change_childs(node, child, parent, root);
		if (child) {
			child->__rb_parent_color = pc;
			rebalance = NULL;
		} else
			rebalance = __rb_is_blacks(pc) ? parent : NULL;
		tmp = parent;
	} else if (!child) {
		/* Still case 1, but this time the child is node->rb_left */
		tmp->__rb_parent_color = pc = node->__rb_parent_color;
		parent = __rb_parents(pc);
		__rb_change_childs(node, tmp, parent, root);
		rebalance = NULL;
		tmp = parent;
	} else {
		struct rb_nodes *successor = child, *child2;

		tmp = child->rb_left;
		if (!tmp) {
			/*
			 * Case 2: node's successor is its right child
			 *
			 *    (n)          (s)
			 *    / \          / \
			 *  (x) (s)  ->  (x) (c)
			 *        \
			 *        (c)
			 */
			parent = successor;
			child2 = successor->rb_right;

			augment->copy(node, successor);
		} else {
			/*
			 * Case 3: node's successor is leftmost under
			 * node's right child subtree
			 *
			 *    (n)          (s)
			 *    / \          / \
			 *  (x) (y)  ->  (x) (y)
			 *      /            /
			 *    (p)          (p)
			 *    /            /
			 *  (s)          (c)
			 *    \
			 *    (c)
			 */
			do {
				parent = successor;
				successor = tmp;
				tmp = tmp->rb_left;
			} while (tmp);
			child2 = successor->rb_right;
			WRITE_ONCES(parent->rb_left, child2);
			WRITE_ONCES(successor->rb_right, child);
			rb_set_parents(child, successor);

			augment->copy(node, successor);
			augment->propagate(parent, successor);
		}

		tmp = node->rb_left;
		WRITE_ONCES(successor->rb_left, tmp);
		rb_set_parents(tmp, successor);

		pc = node->__rb_parent_color;
		tmp = __rb_parents(pc);
		__rb_change_childs(node, successor, tmp, root);

		if (child2) {
			successor->__rb_parent_color = pc;
			rb_set_parent_colors(child2, parent, RB_BLACKS);
			rebalance = NULL;
		} else {
			unsigned long pc2 = successor->__rb_parent_color;
			successor->__rb_parent_color = pc;
			rebalance = __rb_is_blacks(pc2) ? parent : NULL;
		}
		tmp = successor;
	}

	augment->propagate(tmp, NULL);
	return rebalance;
}

static inline void
rb_erase_augmenteds(struct rb_nodes *node, struct rb_roots *root,
		   const struct rb_augment_callbackss *augment)
{
	struct rb_nodes *rebalance = __rb_erase_augmenteds(node, root, augment);
	if (rebalance)
		__rb_erase_colors(rebalance, root, augment->rotate);
}

#endif	/* _LINUX_RBTREE_AUGMENTED_H */
