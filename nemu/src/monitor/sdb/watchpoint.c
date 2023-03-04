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
	char *p;
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
	WP* temp = free_;
	//free_->p = v;
	//free_->value = expr(v, NULL);
	free_->next = head->next;
	free_ = temp->next;
	head = temp;
}

void free_wp(int n)
{
	WP* i, *k, *j;
	for(i = head; i != NULL; i = i->next)
	{
		if(i->next->NO == n)
		{
			j = free_;
			k = i->next->next;
			free_ = i->next;
			i->next->next = j;
			i->next = k;
		}
	}
}

void display_wp()
{
	WP* i;
	for(i = head; i != NULL; i = i->next)
		printf("%s %d %x\n", i->p, i->value, i->value);
}

void check()
{
	WP* i;
    for(i = head; i != NULL; i = i->next)
		if(i->value != expr(i->p, NULL))
			nemu_state.state = NEMU_STOP;
}
