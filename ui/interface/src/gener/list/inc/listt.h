#ifndef __SDK_TAG_LIST_H__
#define __SDK_TAG_LIST_H__
// #include <typeinfo.h>
#ifdef __cplusplus
extern "C"{
#endif /* _cplusplus */

#ifdef __ANDROID__
#define typeof_ __typeof__
#else 
#define typeof_ typeof
#endif

# define __force

#ifndef __always_inline
#define __always_inline inline
#endif

#ifndef offsetof__
# define offsetof__(T,F) ((unsigned int)((char *)&((T *)0)->F))
#endif

typedef struct list_head_ {
    struct list_head_ *next, *prev;
} List_;

/*
 * struct hlist_head_ {
 *     struct hlist_node *first;
 * }; 
 * struct hlist_node {
 *     struct hlist_node *next, **pprev;
 * };
 */

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#define LIST_HEAD_INIT_(name) { &(name), &(name) }

#define LIST_HEAD_(name) \
	struct list_head_ name = LIST_HEAD_INIT_(name)

static inline void INIT_LIST_HEAD_(struct list_head_ *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add_(struct list_head_ *newNode,
			      struct list_head_ *prev,
			      struct list_head_ *next)
{
	next->prev = newNode;
	newNode->next = next;
	newNode->prev = prev;
	prev->next = newNode;
}

static inline void list_add_(struct list_head_ *newNode, struct list_head_ *head)
{
	__list_add_(newNode, head, head->next);
}

static inline void list_add_tail_(struct list_head_ *newNode, struct list_head_ *head)
{
	__list_add_(newNode, head->prev, head);
}

static inline void __list_del_(struct list_head_ * prev, struct list_head_ * next)
{
	next->prev = prev;
	prev->next = next;
}

#ifndef CONFIG_DEBUG_LIST
static inline void __list_del_entry_(struct list_head_ *entry)
{
	__list_del_(entry->prev, entry->next);
}
static inline void list_del_(struct list_head_ *entry)
{
	__list_del_(entry->prev, entry->next);
    entry->next = entry;
    entry->prev = entry;
}
#else
extern void __list_del_entry_(struct list_head_ *entry);
extern void list_del_(struct list_head_ *entry);
#endif

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace_(struct list_head_ *old,
				struct list_head_ *newNode)
{
	newNode->next = old->next;
	newNode->next->prev = newNode;
	newNode->prev = old->prev;
	newNode->prev->next = newNode;
}

static inline void list_replace_init_(struct list_head_ *old,
					struct list_head_ *newNode)
{
	list_replace_(old, newNode);
	INIT_LIST_HEAD_(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init_(struct list_head_ *entry)
{
	__list_del_entry_(entry);
	INIT_LIST_HEAD_(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move_(struct list_head_ *list, struct list_head_ *head)
{
	__list_del_entry_(list);
	list_add_(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail_(struct list_head_ *list,
				  struct list_head_ *head)
{
	__list_del_entry_(list);
	list_add_tail_(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_last_(const struct list_head_ *list,
				const struct list_head_ *head)
{
	return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty_(const struct list_head_ *head)
{
	return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static inline int list_empty_careful_(const struct list_head_ *head)
{
	struct list_head_ *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void list_rotate_left_(struct list_head_ *head)
{
	struct list_head_ *first;

	if (!list_empty_(head)) {
		first = head->next;
		list_move_tail_(first, head);
	}
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int list_is_singular_(const struct list_head_ *head)
{
	return !list_empty_(head) && (head->next == head->prev);
}

static inline void __list_cut_position_(struct list_head_ *list,
		struct list_head_ *head, struct list_head_ *entry)
{
	struct list_head_ *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void list_cut_position_(struct list_head_ *list,
		struct list_head_ *head, struct list_head_ *entry)
{
	if (list_empty_(head))
		return;
	if (list_is_singular_(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		INIT_LIST_HEAD_(list);
	else
		__list_cut_position_(list, head, entry);
}

static inline void __list_splice_(const struct list_head_ *list,
				 struct list_head_ *prev,
				 struct list_head_ *next)
{
	struct list_head_ *first = list->next;
	struct list_head_ *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice_(const struct list_head_ *list,
				struct list_head_ *head)
{
	if (!list_empty_(list))
		__list_splice_(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice_tail_(struct list_head_ *list,
				struct list_head_ *head)
{
	if (!list_empty_(list))
		__list_splice_(list, head->prev, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init_(struct list_head_ *list,
				    struct list_head_ *head)
{
	if (!list_empty_(list)) {
		__list_splice_(list, head, head->next);
		INIT_LIST_HEAD_(list);
	}
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void list_splice_tail_init_(struct list_head_ *list,
					 struct list_head_ *head)
{
	if (!list_empty_(list)) {
		__list_splice_(list, head->prev, head);
		INIT_LIST_HEAD_(list);
	}
}

#define list_entry_(ptr, type, member) \
	((type *)((unsigned long)(ptr)-((unsigned long)(&((type *)1)->member) - 1)))

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head_ within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry_(ptr, type, member) \
	list_entry_((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head_ within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry_(ptr, type, member) \
	list_entry_((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head_ within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null_(ptr, type, member) \
	(!list_empty_(ptr) ? list_first_entry_(ptr, type, member) : NULL)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head_ within the struct.
 */
#define list_next_entry_(pos, member) \
	list_entry_((pos)->member.next, typeof_(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head_ within the struct.
 */
#define list_prev_entry_(pos, member) \
	list_entry_((pos)->member.prev, typeof_(*(pos)), member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head_ to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head_ to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev_(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head_ to use as a loop cursor.
 * @n:		another &struct list_head_ to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe_(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head_ to use as a loop cursor.
 * @n:		another &struct list_head_ to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe_(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 */
#define list_for_each_entry_(pos, head, member)				\
	for (pos = list_first_entry_(head, typeof_(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry_(pos, member))

#define list_for_each_entry_prev_(pos, head, member)				\
	for (pos = list_entry_((head)->prev, typeof_(*pos), member);	\
		 /* prefetch(pos->member.next),  */(unsigned long)(&pos->member) != (unsigned long)(head); 	\
	     pos = list_entry_(pos->member.prev, typeof_(*pos), member))


/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 */
#define list_for_each_entry_reverse_(pos, head, member)			\
	for (pos = list_last_entry_(head, typeof_(*pos), member);		\
	     &pos->member != (head); 					\
	     pos = list_prev_entry_(pos, member))

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_head_ within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define list_prepare_entry_(pos, head, member) \
	((pos) ? : list_entry_(head, typeof_(*pos), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_(pos, head, member) 		\
	for (pos = list_next_entry_(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_next_entry_(pos, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse_(pos, head, member)		\
	for (pos = list_prev_entry_(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_prev_entry_(pos, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from_(pos, head, member) 			\
	for (; &pos->member != (head);					\
	     pos = list_next_entry_(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 */
#define list_for_each_entry_safe_(pos, n, head, member)			\
	for (pos = list_first_entry_(head, typeof_(*pos), member),	\
		n = list_next_entry_(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_next_entry_(n, member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue_(pos, n, head, member) 		\
	for (pos = list_next_entry_(pos, member), 				\
		n = list_next_entry_(pos, member);				\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry_(n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from_(pos, n, head, member) 			\
	for (n = list_next_entry_(pos, member);					\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry_(n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head_ within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse_(pos, n, head, member)		\
	for (pos = list_last_entry_(head, typeof_(*pos), member),		\
		n = list_prev_entry_(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_prev_entry_(n, member))

/**
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @pos:	the loop cursor used in the list_for_each_entry_safe loop
 * @n:		temporary storage used in list_for_each_entry_safe
 * @member:	the name of the list_head_ within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next_(pos, n, member)				\
	n = list_next_entry_(pos, member)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

#ifdef __cplusplus
}
#endif /* _cplusplus */

#endif
