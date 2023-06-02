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
#include <elf.h>
#include <memory/paddr.h>
//add code 
#include <common.h>
//add end
void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  //Log("Exercise: Please remove me in the source code and compile NEMU again.");
  //assert(0);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
//add code 
static char *elf = NULL;
//add end
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
	//add code
    {"elf"      , required_argument, NULL, 'f'},
	//add end
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:f:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
	  case 'l': log_file = optarg; break;
      case 'f': elf = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case  1 : img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}
//add code
#define number_of_fun 100
info_elf_funciotn arr_fun_elf[number_of_fun];

int num_fun = 0;

void handleelf(FILE *file)
{
	Elf32_Ehdr *ELF_header = NULL;
	Elf32_Shdr *Section_header = NULL;
	Elf32_Sym *Symtab_header= NULL;
	
	/** ELF head parsing **/
	fseek(file, 0, SEEK_SET);
	ELF_header = malloc(sizeof(Elf32_Ehdr));	
	int x = fread(ELF_header, sizeof(Elf32_Ehdr), 1, file);
	x += 1; //meaningless , just for warning

	/** ELF section head parsing **/
	fseek(file, ELF_header->e_shoff, SEEK_SET);
	Section_header = malloc(sizeof(Elf32_Shdr) * ELF_header->e_shnum);	
	int y = fread(Section_header, sizeof(Elf32_Shdr), ELF_header->e_shnum, file);
	y += 1; //meaningless , just for warning

	//the position of shstrtab
	Elf32_Shdr *shstrtab = ELF_header->e_shstrndx + Section_header;
	int numofsym = 0;
	Elf32_Off stroffset = 0;
	for(int i = 0; i < ELF_header->e_shnum; i ++)
	{
		char shstrtabname[shstrtab->sh_size];
		fseek(file, shstrtab->sh_offset + Section_header->sh_name, SEEK_SET);
		int z = fread(shstrtabname, shstrtab->sh_size, 1, file);
		z += 1; //meaningless , just for warning
		if(!strcmp(shstrtabname, ".symtab"))
		{
			fseek(file, Section_header->sh_offset, SEEK_SET);
			numofsym = Section_header->sh_size / sizeof(Elf32_Sym);
			Symtab_header = malloc(Section_header->sh_size);
			int gg = fread(Symtab_header, sizeof(Elf32_Sym), numofsym, file);
			gg += 1;
		}
		if(!strcmp(shstrtabname, ".strtab"))
		{
			stroffset = Section_header->sh_offset;
		}
		Section_header ++;
	}

	for(int i = 0; i < numofsym; i ++)
	{
		if(ELF32_ST_TYPE(Symtab_header->st_info) == STT_FUNC)
		{
			char strname[sizeof_fun_string];
			arr_fun_elf[num_fun].fun_size = Symtab_header->st_size;
			fseek(file, stroffset + Symtab_header->st_name, SEEK_SET);
			int gg = fread(strname, sizeof_fun_string, 1, file);
			gg += 1;
			strcpy(arr_fun_elf[num_fun].funname, strname);
			arr_fun_elf[num_fun].fun_value = Symtab_header->st_value;
			num_fun ++;
		}
		Symtab_header ++;
	}
	//for(int i = 0; i < num_fun; i ++)
		//printf("%d %x %s\n", arr_fun_elf[i].fun_size, arr_fun_elf[i].fun_value, arr_fun_elf[i].funname);

	/** Release memory **/
	free(ELF_header);
	free(Section_header);
	free(Symtab_header);
}

void init_elf()
{
	FILE *Elf = fopen(elf, "rb");
	handleelf(Elf);
	fclose(Elf);
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand(); //said by dtyy : in utils

  /* Open the log file. */
  init_log(log_file); //at utils

  //add code
  /* Open the elf file. */
  init_elf();

  /* Initialize memory. */
  init_mem(); //said by dtyy : in paddr.c

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
