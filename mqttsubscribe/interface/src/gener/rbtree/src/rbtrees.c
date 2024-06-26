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

  linux/lib/rbtree.c
*/

#include <rbtrees.h>
#include <rbtree.h>
#include <rbtree_augmented.h>


/*
 * red-black trees properties:  http://en.wikipedia.org/wiki/Rbtree
 *
 *  1) A node is either red or black
 *  2) The root is black
 *  3) All leaves (NULL) are black
 *  4) Both children of every red node are black
 *  5) Every simple path from root to leaves contains the same number
 *     of black nodes.
 *
 *  4 and 5 give the O(log n) guarantee, since 4 implies you cannot have two
 *  consecutive red nodes in a path and every red node is therefore followed by
 *  a black. So if B is the number of black nodes on every simple path (as per
 *  5), then the longest possible path due to 4 is 2B.
 *
 *  We shall indicate color with case, where black nodes are uppercase and red
 *  nodes will be lowercase. Unknown color nodes shall be drawn as red within
 *  parentheses and have some accompanying text comment.
 */

/*
 * Notes on lockless lookups:
 *
 * All stores to the tree structure (rb_left and rb_right) must be done using
 * WRITE_ONCE(). And we must not inadvertently cause (temporary) loops in the
 * tree structure as seen in program order.
 *
 * These two requirements will allow lockless iteration of the tree -- not
 * correct iteration mind you, tree rotations are not atomic so a lookup might
 * miss entire subtrees.
 *
 * But they do guarantee that any such traversal will only see valid elements
 * and that it will indeed complete -- does not get stuck in a loop.
 *
 * It also guarantees that if the lookup returns an element it is the 'correct'
 * one. But not returning an element does _NOT_ mean it's not present.
 *
 * NOTE:
 *
 * Stores to __rb_parent_color are not important for simple lookups so those
 * are left undone as of now. Nor did I check for loops involving parent
 * pointers.
 */

static inline void rb_set_blacks(struct rb_nodes *rb)
{
	rb->__rb_parent_color |= RB_BLACKS;
}

static inline struct rb_nodes *rb_red_parents(struct rb_nodes *red)
{
	//这里处理和结构对齐应该有关系
	return (struct rb_nodes *)red->__rb_parent_color;
}

/*
 * Helper function for rotations:
 * - old's parent and color get assigned to new
 * - old gets assigned new as a parent and 'color' as a color.
 */
static inline void
__rb_rotate_set_parents(struct rb_nodes *old, struct rb_nodes *new,
			struct rb_roots *root, int color)
{
	struct rb_nodes *parent = rb_parents(old);
	new->__rb_parent_color = old->__rb_parent_color;
	rb_set_parent_colors(old, new, color);
	__rb_change_childs(old, new, parent, root);
}

static  void
__rb_inserts(struct rb_nodes *node, struct rb_roots *root,
	    void (*augment_rotate)(struct rb_nodes *old, struct rb_nodes *new))
{
	//读取需要插入节点的上一级节点
	struct rb_nodes *parent = rb_red_parents(node), *gparent, *tmp;

	while (1) {
		/*
		 * Loop invariant: node is red
		 *
		 * If there is a black parent, we are done.
		 * Otherwise, take some corrective action as we don't
		 * want a red root or two consecutive red nodes.
		 */
		 /* 节点是红色(新插入的节点都是红色)
		  * 如果当前节点的父节点是黑色，处理。
		  * 否则，需要做一些相应的纠结行为，目的是
		  * 我们需要一个红根节点或者连续红节点。
		  */
		//printf ("[%s %d] parent addr:0x%x\n", __func__, __LINE__, parent);
		if (!parent) {
			//如果根节点不存在
			rb_set_parent_colors(node, NULL, RB_BLACKS);//根节点是黑色
			//printf ("[%s %d] insert root node\n", __func__, __LINE__);
			//初次设置完成，结束处理
			break;
		} else if (rb_is_blacks(parent)) {
			//printf ("[%s %d] parent is black\n", __func__, __LINE__);
			//如果父节点是黑色，达到条件，直接结束(新插入的节点是红色)
			break;
		}

		//获取祖父节点实例
		gparent = rb_red_parents(parent);
		//printf ("[%s %d] get current node parent\n", __func__, __LINE__);

		//获取现象中的右叔父节点实例
		tmp = gparent->rb_right;
		if (parent != tmp) {	/* parent == gparent->rb_left */
			//printf ("[%s %d] uncle node is left\n", __func__, __LINE__);
			//处理部分在左部节点
			if (tmp && rb_is_reds(tmp)) {
				//printf ("[%s %d] uncle exist and is red\n", __func__, __LINE__);
				//叔父节点存在，且叔父节点为红色
				/*
				 * Case 1 - color flips
				 *
				 *       G            g
				 *      / \          / \
				 *     p   u  -->   P   U
				 *    /            /
				 *   n            n
				 *
				 * However, since g's parent might be red, and
				 * 4) does not allow this, we need to recurse
				 * at g.
				 */
				//设置叔父节点的父节点，且设置叔父节点为黑色
				rb_set_parent_colors(tmp, gparent, RB_BLACKS);
				//设置父节点的父节点，且设置父节点为黑色
				rb_set_parent_colors(parent, gparent, RB_BLACKS);
				node = gparent;
				//获取祖父节点的父节点实例
				parent = rb_parents(node);
				//设置祖父节点的父节点，且设置祖父节点为红色
				rb_set_parent_colors(node, parent, RB_REDS);
				continue;
			}

			//printf ("[%s %d] ----\n", __func__, __LINE__);
			tmp = parent->rb_right;
			if (node == tmp) {
				/*
				 * Case 2 - left rotate at parent
				 *
				 *      G             G
				 *     / \           / \
				 *    p   U  -->    n   U
				 *     \           /
				 *      n         p
				 *
				 * This still leaves us in violation of 4), the
				 * continuation into Case 3 will fix that.
				 */
				tmp = node->rb_left;
				WRITE_ONCES(parent->rb_right, tmp);
				WRITE_ONCES(node->rb_left, parent);
				if (tmp)
					rb_set_parent_colors(tmp, parent,
							    RB_BLACKS);
				rb_set_parent_colors(parent, node, RB_REDS);
				augment_rotate(parent, node);
				parent = node;
				tmp = node->rb_right;
			}

			/*
			 * Case 3 - right rotate at gparent
			 *
			 *        G           P
			 *       / \         / \
			 *      p   U  -->  n   g
			 *     /                 \
			 *    n                   U
			 */
			WRITE_ONCES(gparent->rb_left, tmp); /* == parent->rb_right */
			WRITE_ONCES(parent->rb_right, gparent);
			if (tmp)
				rb_set_parent_colors(tmp, gparent, RB_BLACKS);
			__rb_rotate_set_parents(gparent, parent, root, RB_REDS);
			augment_rotate(gparent, parent);
			break;
		} else {
			/* parent == gparent->rb_right */
			//printf ("[%s %d] parent node is right\n", __func__, __LINE__);

			//获取叔父节点
			tmp = gparent->rb_left;
			if (tmp && rb_is_reds(tmp)) {
				//printf ("[%s %d] uncle node is exsit, and is red\n", __func__, __LINE__);
				//叔父节点存在，且叔父节点是红色
				/* Case 1 - color flips */

				//策略
				//1、设置叔父节点为黑色
				rb_set_parent_colors(tmp, gparent, RB_BLACKS);
				//2、设置父节点为黑色
				rb_set_parent_colors(parent, gparent, RB_BLACKS);
				node = gparent;
				parent = rb_parents(node);
				//3、设置祖父节点为红色
				rb_set_parent_colors(node, parent, RB_REDS);
				continue;
			}

			//获取兄弟节点或者节点本身
			tmp = parent->rb_left;
			if (node == tmp) {
				//printf ("[%s %d] curent node is left node\n", __func__, __LINE__);
				//节点本身是左子节点
				/* Case 2 - right rotate at parent */
				//获取当前节点的右子节点
				tmp = node->rb_right;
				//
				WRITE_ONCES(parent->rb_left, tmp);
				WRITE_ONCES(node->rb_right, parent);
				if (tmp)
					rb_set_parent_colors(tmp, parent,
							    RB_BLACKS);
				rb_set_parent_colors(parent, node, RB_REDS);
				augment_rotate(parent, node);
				parent = node;
				tmp = node->rb_left;
			}

			/* Case 3 - left rotate at gparent */
			WRITE_ONCES(gparent->rb_right, tmp); /* == parent->rb_left */
			WRITE_ONCES(parent->rb_left, gparent);
			if (tmp)
				rb_set_parent_colors(tmp, gparent, RB_BLACKS);
			__rb_rotate_set_parents(gparent, parent, root, RB_REDS);
			augment_rotate(gparent, parent);
			break;
		}
	}
}

/*
 * Inline version for rb_erase() use - we want to be able to inline
 * and eliminate the dummy_rotate callback there
 */
static  void
____rb_erase_colors(struct rb_nodes *parent, struct rb_roots *root,
	void (*augment_rotate)(struct rb_nodes *old, struct rb_nodes *new))
{
	struct rb_nodes *node = NULL, *sibling, *tmp1, *tmp2;

	while (1) {
		/*
		 * Loop invariants:
		 * - node is black (or NULL on first iteration)
		 * - node is not the root (parent is not NULL)
		 * - All leaf paths going through parent and node have a
		 *   black node count that is 1 lower than other leaf paths.
		 */
		sibling = parent->rb_right;
		if (node != sibling) {	/* node == parent->rb_left */
			if (rb_is_reds(sibling)) {
				/*
				 * Case 1 - left rotate at parent
				 *
				 *     P               S
				 *    / \             / \
				 *   N   s    -->    p   Sr
				 *      / \         / \
				 *     Sl  Sr      N   Sl
				 */
				tmp1 = sibling->rb_left;
				WRITE_ONCES(parent->rb_right, tmp1);
				WRITE_ONCES(sibling->rb_left, parent);
				rb_set_parent_colors(tmp1, parent, RB_BLACKS);
				__rb_rotate_set_parents(parent, sibling, root,
							RB_REDS);
				augment_rotate(parent, sibling);
				sibling = tmp1;
			}
			tmp1 = sibling->rb_right;
			if (!tmp1 || rb_is_blacks(tmp1)) {
				tmp2 = sibling->rb_left;
				if (!tmp2 || rb_is_blacks(tmp2)) {
					/*
					 * Case 2 - sibling color flip
					 * (p could be either color here)
					 *
					 *    (p)           (p)
					 *    / \           / \
					 *   N   S    -->  N   s
					 *      / \           / \
					 *     Sl  Sr        Sl  Sr
					 *
					 * This leaves us violating 5) which
					 * can be fixed by flipping p to black
					 * if it was red, or by recursing at p.
					 * p is red when coming from Case 1.
					 */
					rb_set_parent_colors(sibling, parent,
							    RB_REDS);
					if (rb_is_reds(parent))
						rb_set_blacks(parent);
					else {
						node = parent;
						parent = rb_parents(node);
						if (parent)
							continue;
					}
					break;
				}
				/*
				 * Case 3 - right rotate at sibling
				 * (p could be either color here)
				 *
				 *   (p)           (p)
				 *   / \           / \
				 *  N   S    -->  N   Sl
				 *     / \             \
				 *    sl  Sr            s
				 *                       \
				 *                        Sr
				 */
				tmp1 = tmp2->rb_right;
				WRITE_ONCES(sibling->rb_left, tmp1);
				WRITE_ONCES(tmp2->rb_right, sibling);
				WRITE_ONCES(parent->rb_right, tmp2);
				if (tmp1)
					rb_set_parent_colors(tmp1, sibling,
							    RB_BLACKS);
				augment_rotate(sibling, tmp2);
				tmp1 = sibling;
				sibling = tmp2;
			}
			/*
			 * Case 4 - left rotate at parent + color flips
			 * (p and sl could be either color here.
			 *  After rotation, p becomes black, s acquires
			 *  p's color, and sl keeps its color)
			 *
			 *      (p)             (s)
			 *      / \             / \
			 *     N   S     -->   P   Sr
			 *        / \         / \
			 *      (sl) sr      N  (sl)
			 */
			tmp2 = sibling->rb_left;
			WRITE_ONCES(parent->rb_right, tmp2);
			WRITE_ONCES(sibling->rb_left, parent);
			rb_set_parent_colors(tmp1, sibling, RB_BLACKS);
			if (tmp2)
				rb_set_parents(tmp2, parent);
			__rb_rotate_set_parents(parent, sibling, root,
						RB_BLACKS);
			augment_rotate(parent, sibling);
			break;
		} else {
			sibling = parent->rb_left;
			if (rb_is_reds(sibling)) {
				/* Case 1 - right rotate at parent */
				tmp1 = sibling->rb_right;
				WRITE_ONCES(parent->rb_left, tmp1);
				WRITE_ONCES(sibling->rb_right, parent);
				rb_set_parent_colors(tmp1, parent, RB_BLACKS);
				__rb_rotate_set_parents(parent, sibling, root,
							RB_REDS);
				augment_rotate(parent, sibling);
				sibling = tmp1;
			}
			tmp1 = sibling->rb_left;
			if (!tmp1 || rb_is_blacks(tmp1)) {
				tmp2 = sibling->rb_right;
				if (!tmp2 || rb_is_blacks(tmp2)) {
					/* Case 2 - sibling color flip */
					rb_set_parent_colors(sibling, parent,
							    RB_REDS);
					if (rb_is_reds(parent))
						rb_set_blacks(parent);
					else {
						node = parent;
						parent = rb_parents(node);
						if (parent)
							continue;
					}
					break;
				}
				/* Case 3 - right rotate at sibling */
				tmp1 = tmp2->rb_left;
				WRITE_ONCES(sibling->rb_right, tmp1);
				WRITE_ONCES(tmp2->rb_left, sibling);
				WRITE_ONCES(parent->rb_left, tmp2);
				if (tmp1)
					rb_set_parent_colors(tmp1, sibling,
							    RB_BLACKS);
				augment_rotate(sibling, tmp2);
				tmp1 = sibling;
				sibling = tmp2;
			}
			/* Case 4 - left rotate at parent + color flips */
			tmp2 = sibling->rb_right;
			WRITE_ONCES(parent->rb_left, tmp2);
			WRITE_ONCES(sibling->rb_right, parent);
			rb_set_parent_colors(tmp1, sibling, RB_BLACKS);
			if (tmp2)
				rb_set_parents(tmp2, parent);
			__rb_rotate_set_parents(parent, sibling, root,
						RB_BLACKS);
			augment_rotate(parent, sibling);
			break;
		}
	}
}

/* Non-inline version for rb_erase_augmented() use */
void __rb_erase_colors(struct rb_nodes *parent, struct rb_roots *root,
	void (*augment_rotate)(struct rb_nodes *old, struct rb_nodes *new))
{
	____rb_erase_colors(parent, root, augment_rotate);
}
//EXPORT_SYMBOL(__rb_erase_colors);

/*
 * Non-augmented rbtree manipulation functions.
 *
 * We use dummy augmented callbacks here, and have the compiler optimize them
 * out of the rb_insert_color() and rb_erase() function definitions.
 */

static inline void dummy_propagate(struct rb_nodes *node, struct rb_nodes *stop) {}
static inline void dummy_copy(struct rb_nodes *old, struct rb_nodes *new) {}
static inline void dummy_rotate(struct rb_nodes *old, struct rb_nodes *new) {}

static const struct rb_augment_callbackss dummy_callbacks = {
	dummy_propagate, dummy_copy, dummy_rotate
};

void rb_insert_colors(struct rb_nodes *node, struct rb_roots *root)
{
	__rb_inserts(node, root, dummy_rotate);
}
//EXPORT_SYMBOL(rb_insert_color);

void rb_erases(struct rb_nodes *node, struct rb_roots *root)
{
	struct rb_nodes *rebalance;
	rebalance = __rb_erase_augmenteds(node, root, &dummy_callbacks);
	if (rebalance)
		____rb_erase_colors(rebalance, root, dummy_rotate);
}
//EXPORT_SYMBOL(rb_erase);

/*
 * Augmented rbtree manipulation functions.
 *
 * This instantiates the same  functions as in the non-augmented
 * case, but this time with user-defined callbacks.
 */

void __rb_insert_augmenteds(struct rb_nodes *node, struct rb_roots *root,
	void (*augment_rotate)(struct rb_nodes *old, struct rb_nodes *new))
{
	__rb_inserts(node, root, augment_rotate);
}
//EXPORT_SYMBOL(__rb_insert_augmented);

/*
 * This function returns the first node (in sort order) of the tree.
 */
struct rb_nodes *rb_firsts(const struct rb_roots *root)
{
	struct rb_nodes	*n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_left)
		n = n->rb_left;
	return n;
}
//EXPORT_SYMBOL(rb_first);

struct rb_nodes *rb_lasts(const struct rb_roots *root)
{
	struct rb_nodes	*n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_right)
		n = n->rb_right;
	return n;
}
//EXPORT_SYMBOL(rb_last);

struct rb_nodes *rb_nexts(const struct rb_nodes *node)
{
	struct rb_nodes *parent;

	if (RB_EMPTY_NODES(node))
		return NULL;

	/*
	 * If we have a right-hand child, go down and then left as far
	 * as we can.
	 */
	if (node->rb_right) {
		node = node->rb_right; 
		while (node->rb_left)
			node=node->rb_left;
		return (struct rb_nodes *)node;
	}

	/*
	 * No right-hand children. Everything down and left is smaller than us,
	 * so any 'next' node must be in the general direction of our parent.
	 * Go up the tree; any time the ancestor is a right-hand child of its
	 * parent, keep going up. First time it's a left-hand child of its
	 * parent, said parent is our 'next' node.
	 */
	while ((parent = rb_parents(node)) && node == parent->rb_right)
		node = parent;

	return parent;
}
//EXPORT_SYMBOL(rb_next);

struct rb_nodes *rb_prevs(const struct rb_nodes *node)
{
	struct rb_nodes *parent;

	if (RB_EMPTY_NODES(node))
		return NULL;

	/*
	 * If we have a left-hand child, go down and then right as far
	 * as we can.
	 */
	if (node->rb_left) {
		node = node->rb_left; 
		while (node->rb_right)
			node=node->rb_right;
		return (struct rb_nodes *)node;
	}

	/*
	 * No left-hand children. Go up till we find an ancestor which
	 * is a right-hand child of its parent.
	 */
	while ((parent = rb_parents(node)) && node == parent->rb_left)
		node = parent;

	return parent;
}
//EXPORT_SYMBOL(rb_prev);

void rb_replace_nodes(struct rb_nodes *victim, struct rb_nodes *new,
		     struct rb_roots *root)
{
	struct rb_nodes *parent = rb_parents(victim);

	/* Copy the pointers/colour from the victim to the replacement */
	*new = *victim;

	/* Set the surrounding nodes to point to the replacement */
	if (victim->rb_left)
		rb_set_parents(victim->rb_left, new);
	if (victim->rb_right)
		rb_set_parents(victim->rb_right, new);
	__rb_change_childs(victim, new, parent, root);
}
//EXPORT_SYMBOL(rb_replace_node);

#if 0
void rb_replace_node_rcu(struct rb_nodes *victim, struct rb_nodes *new,
			 struct rb_roots *root)
{
	struct rb_nodes *parent = rb_parent(victim);

	/* Copy the pointers/colour from the victim to the replacement */
	*new = *victim;

	/* Set the surrounding nodes to point to the replacement */
	if (victim->rb_left)
		rb_set_parent(victim->rb_left, new);
	if (victim->rb_right)
		rb_set_parent(victim->rb_right, new);

	/* Set the parent's pointer to the new node last after an RCU barrier
	 * so that the pointers onwards are seen to be set correctly when doing
	 * an RCU walk over the tree.
	 */
	__rb_change_child_rcu(victim, new, parent, root);
}
//EXPORT_SYMBOL(rb_replace_node_rcu);
#endif

static struct rb_nodes *rb_left_deepest_nodes(const struct rb_nodes *node)
{
	for (;;) {
		if (node->rb_left)
			node = node->rb_left;
		else if (node->rb_right)
			node = node->rb_right;
		else
			return (struct rb_nodes *)node;
	}
}

struct rb_nodes *rb_next_postorders(const struct rb_nodes *node)
{
	const struct rb_nodes *parent;
	if (!node)
		return NULL;
	parent = rb_parents(node);

	/* If we're sitting on node, we've already seen our children */
	if (parent && node == parent->rb_left && parent->rb_right) {
		/* If we are the parent's left node, go to the parent's right
		 * node then all the way down to the left */
		return rb_left_deepest_nodes(parent->rb_right);
	} else
		/* Otherwise we are the parent's right node, and the parent
		 * should be next */
		return (struct rb_nodes *)parent;
}
//EXPORT_SYMBOL(rb_next_postorder);

struct rb_nodes *rb_first_postorders(const struct rb_roots *root)
{
	if (!root->rb_node)
		return NULL;

	return rb_left_deepest_nodes(root->rb_node);
}
//EXPORT_SYMBOL(rb_first_postorder);
