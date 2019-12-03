
#ifndef LISTMAP_H
#define LISTMAP_H

struct List;
typedef struct List* List;
struct List {
	void *obj;
	List next;
};

void list_push(List* list, void *obj);
int list_len(List* list);
void* list_get(List* list, int i);

struct Map;
typedef struct Map* Map;
struct Map {
	void* obj;
	char* str;
	Map high;
	Map low;
};

void map_set(Map* map, char* str, void* obj);
void* map_get(Map* map, char* str);

#endif
