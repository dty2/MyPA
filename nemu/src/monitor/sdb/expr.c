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
			case TK_NOTYPE: break;
			case TK_EQ: tokens[nr_token ++].type = TK_EQ; break;
			case '+': tokens[nr_token ++].type = '+'; break;
			case '-': tokens[nr_token ++].type = '-'; break;
			case '*': tokens[nr_token ++].type = '*'; break;
			case '/': tokens[nr_token ++].type = '/'; break;
			case '(': tokens[nr_token ++].type = '('; break;
			case ')': tokens[nr_token ++].type = ')'; break;
			case TK_NUM:
					  int p = 0;
					  for(; p < substr_len; p ++)
						  tokens[nr_token].str = tokens[nr_token].str * 10 + (int)(*(substr_start + p) - '0');
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

int rn(int a, int b, int c)
{
	if(c == '+') return a + b;	
	if(c == '-') return a - b;	
	if(c == '*') return a * b;	
	if(c == '/') return a / b;	
	return 0;
}

int cal(Token* ex, int r)
{
	int i = 0, j = 0;
	Token stack[32];
	for(; i < r; i ++, j ++)
	{
		if(ex[i].type == '*' || ex[i].type == '/')
		{
			stack[j].str = rn(ex[i - 1].str, ex[i + 1].str, ex[i].type);
			stack[j].type = 0;
		}
		else
		{
			if(ex[i].type == TK_NUM)
			{
				stack[j].str = ex[i].str;
				stack[j].type = TK_NUM;
			}
			else stack[j].type = ex[i].type;
		}
	}
	for(i = 1; i < r; i ++)
		if(stack[i].type != TK_NUM) stack[i + 1].str = rn(ex[i - 1].str, ex[i + 1].str, ex[i].type);
	return stack[i - 1].str;
}

int divs(int l, int r)
{
	int i = 0, j = 0;
	int p = 0, f = 0, temp = 0;
	Token* stack = (Token*)malloc(sizeof(Token) * 32);
	for(i = l, j = 0; i < r; i ++, j ++)
	{
		if(tokens[i].type == '(')
		{
			for(f = i + 1, p = 1; f < r; f ++)
			{
				if(tokens[f].type == '(') p ++;
				if(tokens[f].type == ')') p --;
				if(p == 0)
				{
					temp = f;
					break;
				}
			}
			stack[j].str = divs(i + 1, temp);
			stack[j].type = TK_NUM;
			i = temp;
		}
		else
		{
			if(tokens[i].type == TK_NUM)
			{
				stack[j].str = tokens[i].str;
				stack[j].type = TK_NUM;
			}
			else stack[j].type = tokens[i].type;
		}
	}
	for(i = 0; i < j; i ++)
	{
		if(stack[i].type == TK_NUM)
			printf("%d", stack[i].str);
		else
		{
			if(stack[i].type != TK_NOTYPE)
			printf("%c", (char)stack[i].type);
		}
	}
	printf("\n");
	return cal(stack, j);
}

word_t expr(char *e, bool *success) {
	//int i = 0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
//add code
  //TODO();
	/*for(i = 0; i < nr_token; i ++)
	{
		if(tokens[i].type == TK_NUM)
			printf("%d", tokens[i].str);
		else
		{
			if(tokens[i].type != TK_NOTYPE)
			printf("%c", (char)tokens[i].type);
		}
	}*/
	printf("%d", divs(0, nr_token));
	printf("\n");
  return 0;
}
