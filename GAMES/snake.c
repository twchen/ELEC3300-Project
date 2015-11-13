#include <stdlib.h>
#include <time.h>
#include "stdbool.h"
#include "lcd.h"
#include "key.h"
#include "delay.h"
#include "touch.h"

#define HEIGHT 16
#define WIDTH 32
#define NUM_OF_PIXELS (HEIGHT * WIDTH)

void _ttywrch(int ch)
{
ch = ch;
}

typedef struct Point
{
	int x;
	int y;
} Point;

typedef struct Node
{
	Point p;
	struct Node *prev;
	struct Node *next;
} Node;

typedef struct List
{
	Node *head;
	int length;
} List;

Point make_point(int x, int y)
{
	Point p;
	p.x = x;
	p.y = y;
	return p;
}


void erasePixel(int x, int y)
{
	LCD_Fill(x * 20 + 20, y * 20 + 20, x*20 + 40, y*20 + 40, WHITE);
}

void fillPixel(int x, int y)
{
	LCD_Fill(x * 20+20, y * 20+20, x*20 + 40, y*20+40, BLACK);
}

void drawFood(int x, int y)
{
	LCD_Fill(x * 20+20, y * 20+20, x*20 + 40, y*20+40, RED);
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

List *make_list()
{
	List *list = (List*)malloc(sizeof(List));
	Node *dummy_head = (Node*)malloc(sizeof(Node));
	dummy_head->p = make_point(0,0);
	dummy_head->prev = dummy_head->next = dummy_head;
	list->head = dummy_head;
	list->length = 0;
	return list;
}

void push_front(List *list, Point p)
{
	Node *new_node = (Node*)malloc(sizeof(Node));
	new_node->p = p;
	new_node->prev = list->head;
	new_node->next = list->head->next;
	list->head->next = new_node;
	new_node->next->prev = new_node;
	++(list->length);
}

void push_back(List *list, Point p)
{
	Node *new_node = (Node*)malloc(sizeof(Node));
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
	free(temp);
}

void pop_back(List *list)
{
	Node *temp = list->head->prev;
	if(list->head == list->head->next)
		return;
	list->head->prev = temp->prev;
	temp->prev->next = temp->next;
	--(list->length);
	free(temp);
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
		free(curr->prev);
		curr = curr->next;
	}
	free(tail);
	free(list);
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




typedef enum Direction
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
} Direction;

bool space[NUM_OF_PIXELS]; // 16 * 32

List *snake;
Direction dir;

Point food;
bool game_over;

List *make_snake()
{
	List *list = make_list();
	int i=0;
	for(; i<5; ++i){
		push_front(list, make_point(10, 5 + i));
	}
	return list;
}

void make_food()
{
	while(true){
		int x = rand() % HEIGHT;
		int y = rand() % WIDTH;
		if(space[x * WIDTH + y] == false){
			food = make_point(x, y);
			space[x * WIDTH + y] = true;
			break;
		}
	}
	drawFood(food.x, food.y);
}

/* move one step in the current direction */
void move_forward()
{
	Point p = list_front(snake);
	int x = p.x, y = p.y;
	switch(dir){
		case UP:
			--x;
			break;
		case DOWN:
			++x;
			break;
		case RIGHT:
			++y;
			break;
		case LEFT:
			--y;
			break;
	}

	// out of border?
	if(x >= HEIGHT || y >= WIDTH || x < 0 || y < 0){
		game_over = true;
		return;
	}

	if(space[x * WIDTH + y] == false){ // nothing there
		// simply move forward by one step
		Point tail = list_back(snake);
		space[tail.x * WIDTH + tail.y] = false;
		erasePixel(tail.x, tail.y);
		pop_back(snake);
		push_front(snake, make_point(x, y));
		fillPixel(x, y);
		space[x * WIDTH + y] = true;
	}
	else if(food.x == x && food.y == y){ // food there
		// eat the food and generate new food
		push_front(snake, food);
		fillPixel(x, y);
		space[x * WIDTH + y] = true;
		make_food();
	}
	else{ // collide with itself
		Point tail = list_back(snake);
		if(tail.x == x && tail.y == y){ // catching its tail is not considered as a collision
			pop_back(snake);
			push_front(snake, tail);
		}
		else
			game_over = true;
	}
}

/* initialize space */
void init_space()
{
	int i=0;
	LCD_Clear(WHITE);
	LCD_Fill(0, 0, 20, 680, GREEN);
	LCD_Fill(340, 0, 360, 680, GREEN);
	LCD_Fill(20, 0, 340, 20, GREEN);
	LCD_Fill(20, 660, 340, 680, GREEN);
	for(; i<NUM_OF_PIXELS; ++i)
		space[i] = false;
	/* init snake position */
	i=0;
	for(; i<5; ++i)
		space[10*WIDTH + 5 + i] = true;
}

void print_game_over()
{
	LCD_Clear(WHITE);
	LCD_ShowString(30, 200, 200, 24, 24, "GAME OVER!");
}

#define KEY_UP 3
#define KEY_DOWN 1
#define KEY_RIGHT 2
#define KEY_LEFT 4

void snake_game()
{
	//srand ( time(NULL) );
	srand(0);
	init_space();

	snake = make_snake();
	dir = RIGHT;
	make_food();
	game_over = false;

	while(true){
		u8 ch = 0;
		int counter = 500000;
		while(ch == 0 && counter > 0){
			ch = KEY_Scan(0);
			counter--;
		}
		counter = 500000;
		if(ch == KEY_UP && dir != DOWN)
			dir = UP;
		else if(ch == KEY_RIGHT && dir != LEFT)
			dir = RIGHT;
		else if(ch == KEY_DOWN && dir != UP)
			dir = DOWN;
		else if(ch == KEY_LEFT && dir != RIGHT)
			dir = LEFT;
		
		tp_dev.scan(0);
		if((tp_dev.sta) & 0x80 && tp_dev.y[4] > 600)
			break;
		move_forward();
		if(game_over){
			print_game_over();
			delay_ms(1000);
			break;
		}
	}
}

