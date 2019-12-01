
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "listmap.h"

void list_push(List* list, void *obj){
	
	while (*list != 0) {
		list = &(list[0]->next);
	}

	*list = malloc(sizeof(struct List));
	list[0]->obj = obj;
	list[0]->next = 0;
}

int list_len(List* list) {
	
	int length = 0;

	while (*list != 0) {
		list = &(list[0]->next);
		length++;
	}

	return length;
}

void* list_get(List* list, int i) {

	while (i > 0) {
		if (*list == 0) {
			return 0;
		}

		list = &(list[0]->next);
		i--;
	}

	return list[0]->obj;
}

void map_set(Map* map, char *str, void *obj) {
	
	while (*map != 0) {
		int res = strcmp(str, map[0]->str);

		if (res == 0) {
			break;
		} else if (res > 0) {
			map = &(map[0]->high);
		} else {
			map = &(map[0]->low);
		}
	}

	if (*map != 0) {
		free(*map);
	}

	*map = malloc(sizeof(struct Map));

	map[0]->str = str;
	map[0]->obj = obj;
	map[0]->high = 0;
	map[0]->low = 0;
}

void *map_get(Map* map, char* str) {
	
	while (*map != 0) {
		int res = strcmp(str, map[0]->str);

		if (res == 0) {
			return map[0]->obj;
		} else if(res > 0) {
			map = &(map[0]->high);
		} else {
			map = &(map[0]->low); 
		}
	}

	return 0;
}
