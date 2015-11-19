#include <time.h>
#include "my_list.h"
#include "malloc.h"

Point make_point(int x, int y)
{
	Point p;
	p.x = x;
	p.y = y;
	return p;
}



//void print_point(Point p)
//{
//	printf("(%d, %d), ", p.x, p.y);
//}

void print_list(List *list)
{
	Node *curr = list->head->next;
	while(curr != list->head){
		//print_point(curr->p);
		curr = curr->next;
	}
	//printf("\n");
}

List *make_list(void)
{
	List *list = (List*)mymalloc(MY_LIST_MEM, sizeof(List));
	Node *dummy_head = (Node*)mymalloc(MY_LIST_MEM, sizeof(Node));
	dummy_head->p = make_point(0,0);
	dummy_head->prev = dummy_head->next = dummy_head;
	list->head = dummy_head;
	list->length = 0;
	return list;
}

void push_front(List *list, Point p)
{
	Node *new_node = (Node*)mymalloc(MY_LIST_MEM, sizeof(Node));
	new_node->p = p;
	new_node->prev = list->head;
	new_node->next = list->head->next;
	list->head->next = new_node;
	new_node->next->prev = new_node;
	++(list->length);
}

void push_back(List *list, Point p)
{
	Node *new_node = (Node*)mymalloc(MY_LIST_MEM, sizeof(Node));
	new_node->p = p;
	new_node->prev = list->head->prev;
	new_node->next = list->head;
	list->head->prev = new_node;
	new_node->prev->next = new_node;
	++(list->length);
}

// get the pointer points to the first node
Node *get_head(List *list)
{
	return list->head->next;
}

// get the pointer points to the last node
Node *get_tail(List *list)
{
	return list->head->prev;
}

void pop_front(List *list)
{
	Node *temp = list->head->next;
	if(list->head == list->head->next)
		return;
	list->head->next = temp->next;
	temp->next->prev = temp->prev;
	--(list->length);
	myfree(MY_LIST_MEM, temp);
}

void pop_back(List *list)
{
	Node *temp = list->head->prev;
	if(list->head == list->head->next)
		return;
	list->head->prev = temp->prev;
	temp->prev->next = temp->next;
	--(list->length);
	myfree(MY_LIST_MEM, temp);
}

Point list_front(List *list)
{
	return list->head->next->p;
}

Point list_back(List *list)
{
	return list->head->prev->p;
}

void clear_list(List *list)
{
	Node *tail = list->head->prev;
	Node *curr = list->head->next;
	while(curr != list->head){
		myfree(MY_LIST_MEM, curr->prev);
		curr = curr->next;
	}
	myfree(MY_LIST_MEM, tail);
	myfree(MY_LIST_MEM, list);
}

List *copy_list(List *list)
{
	List *new_list = make_list();
	Node *curr = list->head->next;
	while(curr != list->head){
		push_back(new_list, curr->p);
		curr = curr->next;
	}
	return new_list;
}
