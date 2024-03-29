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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 50

CPU_state cpu = {}; // said by dtyy: cpu in isa/riscv32/include/isa-def.h
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();

//add code
void check();//function at watchpoint


//add code for itrace
void iput(char *str);
void oput();

#define NUM_rb 20
static struct
{
	char ringbuffer[NUM_rb][120];
	int left;
	int right;
	int mem_error;
} rb = { .left = 0, .right = 0, .mem_error = -1 };

void geterr()
{
	rb.mem_error = rb.right;
	oput();
}

void iput(char *str)
{
	strcpy(rb.ringbuffer[rb.right], str);
	rb.right = (rb.right + 1) % NUM_rb;
	if(rb.left == rb.right) rb.left = (rb.left + 1) % NUM_rb;
}

void oput()
{
	int i = rb.left;
	printf("\n");
	for(; i != rb.right; i = (i + 1) % NUM_rb)
		printf("       %s\n", rb.ringbuffer[i]);
	printf("       memory out of bound\n\n");
}

//add code end

//add code for elf
/*
extern int num_fun;
extern struct Info_elf_function
{
	int fun_value;
	int fun_size;
	char funname[100];
} arr_fun_elf[100];

struct ftrace_info
{
	int sign;
	int pc;
	int fun_add;
	char fun[100];
} ftr_info[10000];
int now_info = 0;

char arr_elf[100][100];
int arr_now = 0;

void ftrace_get_jump(int now_pc, int jump_pc)
{
	char nameoffun[100];

	for(int i = 0; i < num_fun; i ++)
	{
		if(jump_pc < arr_fun_elf[i].fun_value + arr_fun_elf[i].fun_size && jump_pc >= arr_fun_elf[i].fun_value)
			strcpy(nameoffun, arr_fun_elf[i].funname);
	}
	ftr_info[now_info].pc = now_pc;
	ftr_info[now_info].fun_add = jump_pc;
	strcpy(ftr_info[now_info].fun, nameoffun);
	if(!strcmp(arr_elf[arr_now - 1], nameoffun))
	{
		arr_now --;
		ftr_info[now_info].sign= 1;
	}
	else
	{
		ftr_info[now_info].sign = 0;
		strcpy(arr_elf[arr_now ++], ftr_info[now_info].fun);
	}
	now_info ++;
}
*/

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));

	//add code
#ifdef CONFIG_WATCHPOINT
	check();
#endif

#ifdef CONFIG_WATCHPOINT
	iput(_this->logbuf);
#endif
}

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s); //isa_exec_once: isa/riscv32/inst.c
  cpu.pc = s->dnpc;

#ifdef CONFIG_ITRACE
  //said by dtyy : not by me, but by tutorial, the tutorial said ignore it !!!
  //what a wonderful thing! Maybe it's the best message which is in turtorial
  //that I have been heard until now! 

  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4); //MUXDEF: include/common.h
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
#endif
}

static void execute(uint64_t n) {
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  execute(n);
  
  //add code
  //if(rb.erri_getbool) rb.po();

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
      // fall through
    case NEMU_QUIT: statistic();
  }
}
