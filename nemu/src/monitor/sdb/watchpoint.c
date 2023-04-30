/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

//addheadfile
#include <memory/paddr.h>
#include <string.h>

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
	int value;
	char p[50];
	//Said by dtyy : so , why not use char *p ? You must know this : 
	//If you choose a pointer to receive a string in array or which
	//pointed by pointer, in some case , the disater must befalls you. 
	//Because the pointer is only a address value , it can't contain any
	//another useful infomation in it .so whatever you copied, you must
	//know the origin string type clearly, if the origin string is const,
	//well , everything is ok, but if not , believe me choose array which
	//can save you !
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
void new_wp(char *v)
{
	/*
	WP* temp_free = free_;
	//Said by dtyy : so what can pointer do ? 
	//only torment you! try to draw "state machine" to avoid it!
	WP* temp_head = head;
	strcpy(free_->p , v);
	free_->value = expr(v, NULL);
	free_ = free_->next;
	head = temp_free;
	head->next = temp_head;
	*/
	WP* temp = free_;
	strcpy(free_->p , v);
	free_->value = expr(v, NULL);
	free_ = free_->next;
	temp->next = head;
	head = temp;
}

void free_wp(int n)
{
	WP *i, *temp;
	for(i = head; i != NULL; i = i->next)
	{
		if(i->next != NULL && i->next->NO == n)
		{
			temp = i->next;
			i->next = i->next->next;
			temp->next = free_;
			free_ = temp;
			break;
		}
		else
		{
			if(i->NO == n)
			{
				head = i->next;
				i->next = free_;
				free_ = i;
			}
		}
	}
}

void display_wp()
{
	WP* i;
	for(i = head; i != NULL; i = i->next)
		printf("%d: %s %d %x\n", i->NO, i->p, i->value, i->value);
}

void check()
{
	WP* i;
    for(i = head; i != NULL; i = i->next)
		if(i->value != expr(i->p, NULL))
			nemu_state.state = NEMU_STOP;
	printf("hello");
	printf("hello");
	printf("hello");
	printf("hello");
	printf("hello");
	printf("hello");
	printf("hello");
	printf("hello");
	printf("hello");
}
