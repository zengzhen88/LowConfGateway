#if 0
#include <stdio.h>
#include <stdlib.h>
#include <rbtree_interface.h>

typedef struct _tagsUserData {
	unsigned data;
} UserData;

static UserData *init_user(unsigned data) {
	UserData *user_data = NULL;

	user_data = (UserData *) malloc (sizeof(*user_data));
	if (NULL == user_data) {
		printf ("alloc UserData instance data:%d\n", data);
		return NULL;
	}

	user_data->data		= data;

	return user_data;
}

static void exit_user(UserData *data) {
	free(data);
}

static int key_compute(void *v_key, void *u_key) {
	unsigned key = ((UserData *)v_key)->data;
	unsigned _key = ((UserData *)u_key)->data;
	if (key > _key) return 1;
	else if (key < _key) return -1;
	else return 0;
}

static void key_print(void *v_key) {
	unsigned key = ((UserData *)v_key)->data;

	printf (" %d ", key);
}

int main(int args, char *argv[]) {
	int index				= 0;
	UserData *user_data		= NULL;
	rbtree_manage *node		= NULL;
	struct rb_root root	= RB_ROOT;

	if (args < 2) {
		printf ("./rbtree refNum\n");
		return -1;
	}

	for (index = 1; index < args; index++) {
		printf ("[%s %d] insert node->data:%d\n", __func__, __LINE__, atoi(argv[index]));
		user_data = init_user((unsigned)atoi(argv[index]));
		if (NULL == user_data) return -1;

		node = init_node();
		if (NULL == node) {
			exit_user(user_data);
			return -1;
		}

		node->key = user_data;

		insert(node, &root, key_compute);
	}

	for (index = 1; index < args; index++) {
		printf ("[%s %d] search node->data:%d\n", __func__, __LINE__, atoi(argv[index]));
		UserData _user_data;
		_user_data.data = (unsigned)atoi(argv[index]);
		node = search(&_user_data, &root, key_compute);
		if (NULL != node) {
			printf ("search data:%d from root, is %s,\n", 
					atoi(argv[index]), 
					node->rb.__rb_parent_color & 0x3 ? "BLACK" : "RED");
		}
		else {
			printf ("search data:%d from root failure\n", atoi(argv[index]));
		}

#if 0
		if (index == 1 || index == 2)
			erase(node, &root);
#endif
	}

#if 0
	for (index = 1; index < args; index++) {
		printf ("[%s %d] search node->data:%d\n", __func__, __LINE__, atoi(argv[index]));
		UserData _user_data;
		_user_data.data = (unsigned)atoi(argv[index]);
		node = search(&_user_data, &root, key_compute);
		if (NULL != node) {
			printf ("search data:%d from root, is %s,\n", 
					atoi(argv[index]), 
					node->rb.__rb_parent_color & 0x3 ? "BLACK" : "RED");
		}
		else {
			printf ("search data:%d from root failure\n", atoi(argv[index]));
		}
	}
#endif

	trace(&root, key_print);

	return 0;
}
#endif
