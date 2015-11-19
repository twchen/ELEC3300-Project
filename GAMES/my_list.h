#ifndef MY_POINT_H
#define MY_POINT_H

typedef struct Point
{
	int x;
	int y;
} Point;

#endif

#ifndef MY_NODE_H
#define MY_NODE_H

typedef struct Node
{
	Point p;
	struct Node *prev;
	struct Node *next;
} Node;

#endif

#ifndef MY_LIST_H
#define MY_LIST_H

typedef struct List
{
	Node *head;
	int length;
} List;

Point make_point(int x, int y);

//void print_point(Point p);

void print_list(List *list);

List *make_list(void);

void push_front(List *list, Point p);

void push_back(List *list, Point p);

// get the pointer points to the first node
Node *get_head(List *list);
// get the pointer points to the last node
Node *get_tail(List *list);
void pop_front(List *list);

void pop_back(List *list);

Point list_front(List *list);
Point list_back(List *list);
void clear_list(List *list);

List *copy_list(List *list);

#endif

#ifndef MY_LIST_MEM
#define MY_LIST_MEM SRAMEX
#endif
