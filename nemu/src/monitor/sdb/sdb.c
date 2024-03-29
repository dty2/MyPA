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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}
//add code
static int cmd_si(char *args) {
    int i = 0, op_num = 0;//op_num operation number 
    bool sign = 1;
    if(args != 0)
    {
        for(;*(args + i) != 0; i ++)
        {
            if(*(args + i) >= '0' && *(args + i) <= '9')
                op_num = (int)(*(args + i) - '0') + op_num * 10;
            else
            {
                sign = 0;
                break;
            }
        }
    }
    else cpu_exec(1);
    if(sign == 0)
        printf("argument fault! Please enter the right argument!\n");
    else cpu_exec(op_num);
    return 0;
}  
static int cmd_info(char *args) {
    if(args != 0 && *args == 'r') isa_reg_display();
	else if(args != 0 && *args == 'w') display_wp();
	else if(args == 0) printf("argument fault! Please enter the argument!\n");
    else printf("argument fault! Please enter the right argument!\n");
    return 0;
}
static int cmd_x(char *args) {
    int i = 0, op_num = 0;//op_num operation number 
	char *argument;
    int argu_num = 0;
    bool sign = 1;
    if(args != 0)
    {
        for(;*(args + i) != ' '; i ++)
        {
            if(*(args + i) >= '0' && *(args + i) <= '9')
                op_num = (int)(*(args + i) - '0') + op_num * 10;
            else
            {
                sign = 0;
                break;
            }
        }
		argument = args + i;
        argu_num = expr(argument, NULL);
    }
    else printf("argument fault! Please enter the argument!\n");
    if(sign == 0)
        printf("argument fault! Please nter the right argument!\n");
    else 
    {
        int j = 0;
        for(;j < op_num; j ++)
            printf("0x%x = 0x%x\n", argu_num + j * 4, paddr_read(argu_num + j * 4, 4));
	}
    return 0;
}
static int cmd_p(char *args) {
	word_t sum = expr(args, NULL);
	if(sum == 0xffff) printf("Error! Please enter again!\n");
	else if(sum == 0xfffff) printf("Division 0! Please check the expression!\n");
	else printf("%d 0x%x\n", sum, sum);
    return 0;
}
static int cmd_w(char *args) {
	new_wp(args);
    return 0;
}
static int cmd_d(char *args) {
	if(strlen(args) > 2 || *args < '0' || *args > '9')
	{
		printf("Argument error, please enter again!\n");
		return 0;
	}
	free_wp(*args - '0');
    return 0;
}
/*
extern struct ftrace_info
{
	int sign;
	int pc;
	int fun_add;
	char fun[100];
} ftr_info[200];
extern int now_info;
static int cmd_rf()
{
	for(int i = 0; i < now_info; i ++)
	{
		if(!ftr_info[i].sign)
			log_write("%x:Call %s %x\n", ftr_info[i].pc, ftr_info[i].fun, ftr_info[i].fun_add);
		else
			log_write("%x:Ret %s %x\n", ftr_info[i].pc, ftr_info[i].fun, ftr_info[i].fun_add);
	}
	return 0;
}
*/
//add end

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "run n step and stop", cmd_si },
  { "info", "printf register", cmd_info },
  { "x", "calculate the expression value and output the content", cmd_x },
  { "p", "print value of expression", cmd_p },
  { "w", "set the watch point", cmd_w },
  { "d", "delete the watch point", cmd_d },
  //{ "rf", "write ftrace to log.txt", cmd_rf },

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
