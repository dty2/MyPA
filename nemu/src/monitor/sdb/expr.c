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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
//add code
	TK_NUM
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus //by dtyy: '\\' means '\' in regex and '\+' means symbol "+"
  {"==", TK_EQ},        // equal
//add code 
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"[0-9]+", TK_NUM},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  int str;
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
//add code
			case TK_NOTYPE: tokens[nr_token ++].type = TK_NOTYPE; break;
			case TK_EQ: tokens[nr_token ++].type = TK_EQ; break;
			case '+': tokens[nr_token ++].type = '+'; break;
			case '-': tokens[nr_token ++].type = '-'; break;
			case '*': tokens[nr_token ++].type = '*'; break;
			case '/': tokens[nr_token ++].type = '/'; break;
			case '(': tokens[nr_token ++].type = '('; break;
			case ')': tokens[nr_token ++].type = ')'; break;
			case TK_NUM:
					  for(i = 0; i < substr_len; i ++)
						  tokens[nr_token].str = tokens[nr_token].str * 10 + *(substr_start + i);
					  tokens[nr_token ++].type = TK_NUM;
					  break;
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int stack[50];

/*void change()
{
	int i = 0, p = 0, sum = 0;
	for(; i < nr_token; i ++)
		if(tokens[i].type == TK_NUM)
			for(p = 0; p < ; p ++)
				sum = (*(tokens[i].str + p) - '0') + sum * 10;
	printf("%d", sum);
}*/
void cal2(int l, int r)
{

}

word_t expr(char *e, bool *success) {
	int i = 0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
//add code
  //TODO();
	for(i = 0; i < nr_token; i ++)
	{
		if(tokens[i].type == TK_NUM)
			printf("%d", tokens[i].str);
		else
		{
			if(tokens[i].type != TK_NOTYPE)
			printf("%c", (char)tokens[i].type);
		}
	}
    //change();
	printf("\n");

  return 0;
}
