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
#include <math.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  /* TODO: Add more token types */
//add code
	TK_NUM,
	TK_LAND,TK_LOR,
	TK_BE,TK_SE,TK_NE,
	TK_HEX,
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
  {"&&", TK_LAND},
  {"\\|\\|", TK_LOR},
  {"![^=]", '!'},
  {"!=", TK_NE},
  {"<[^=]", '<'},
  {">[^=]", '>'},
  {"<=", TK_BE},
  {">=", TK_SE},
  {"0[^.]x[^.][0-9]+", TK_HEX},
  {"$[a-z,A-Z]+", '$'},
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
//add code
        switch (rules[i].token_type) {
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
			case TK_LAND: tokens[nr_token ++].type = TK_LAND; break;
			case TK_LOR: tokens[nr_token ++].type = TK_LOR; break;
			case TK_BE: tokens[nr_token ++].type = TK_BE; break;
			case TK_NE: tokens[nr_token ++].type = TK_NE; break;
			case '!': tokens[nr_token ++].type = '!'; break;
			case '<': tokens[nr_token ++].type = '<'; break;
			case '>': tokens[nr_token ++].type = '>'; break;
			case TK_SE: tokens[nr_token ++].type = TK_SE; break;
			case TK_HEX:
				int i = 2;
				long sum = 0;
				for(; i < substr_len; i ++)	
					sum += (int)(*(substr_start + i) - '0') * pow(16, substr_len - 3); 
				sum = (int)sum;
				tokens[nr_token ++].str = sum;
			case '$': 
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

//add type
int sign = 0;
enum { level1 = 1, level2, level3, level4 };
enum { error_1 = 1, error_2 }; // 1 division 0, 2 can't match brackets
struct Lev{
	int level_;
	int size;
	int sym[8];
}lev[] = {
	{1, 2, {'*', '/', '#'}},                            //1.*/             
	{2, 2, {'+', '-', '#'}},                            //2.+-             
	{3, 6, {'>', '<', TK_BE, TK_SE, TK_EQ, TK_NE,'#'}}, //3.>=,==,<=,>,<,!=
	{4, 3, {'!', TK_LAND, TK_LOR, '#'}},                //4.&&,||,!
};
//add funtion
int rn(int a, int b, int c)
{
	switch (c) {
		case '+': return a + b;	break;
		case '-': return a - b;	break;
		case '*': return a * b;	break;
		case '/': return b ? a / b : 0xfffff;
		case '!': return !a; break;	
		case TK_LAND: return a && b; break;	
		case TK_LOR: return a || b; break;	
		case TK_EQ: return a == b; break;
		case '<': return a < b; break;
		case '>': return a > b; break;
		case TK_BE: return a <= b; break;	
		case TK_SE: return a >= b; break;	
		case TK_NE: return a != b; break;	
	  default: return 0xffff;
	}
	return 0;
}
//add funtion
Token* cal(Token* ex, int level, int r)
{
	int i = 0, j, sum = 0;
	int k = -1;
	Token* num = (Token*)malloc(sizeof(Token) * 32);
	Token* point = NULL;
	for(i = 0; i < r; i ++)
	{
		while((ex[i].type != TK_NUM && k < lev[level].size) && ex[i].type != lev[level].sym[++ k]);
		if(k != lev[level].size && k != -1)
		{
			if(ex[i].type == '!')
			{
				sum = rn(ex[i + 1].str, 0, ex[i].type);
				ex[i + 1].str = sum;
				ex[i].type = -1;
			}
			else
			{
				sum = rn(ex[i - 1].str, ex[i + 1].str, ex[i].type);
				if(sum == 0xfffff)
				{
					sign = error_1;
					return NULL;
				}
				ex[i + 1].str = sum;
				ex[i - 1].type = -1;
				ex[i].type = -1;
			}
		}
		k = -1;
	}
	for(j = 0, i = 0; i < r; i ++, j ++)
	{
		if(ex[i].type != -1)
		{
			if(ex[i].type == TK_NUM)
			{
				num[j].str = ex[i].str;
				num[j].type = ex[i].type;
			}
			else num[j].type = ex[i].type;
		}
		else j --;
	}
	if(level != 3)
	{
		point = cal(num, level + 1, j);
		free(num);
		num = point;
	}
	return num;
}
/*
int cal(Token* ex, int r)
{
	int i = 0, j, sum = 0;;
	Token num[32];
	for(i = 0; i < r; i ++)
		if(ex[i].type == '*' || ex[i].type == '/')
		{
			sum = rn(ex[i - 1].str, ex[i + 1].str, ex[i].type);
			if(sum == 0xfffff) return 0xfffff;
			ex[i + 1].str = sum;
			ex[i - 1].type = -1;
			ex[i].type = -1;
		}
	for(j = 0, i = 0; i < r; i ++, j ++)
	{
		if(ex[i].type != -1)
		{
			if(ex[i].type == TK_NUM)
			{
				num[j].str = ex[i].str;
				num[j].type = ex[i].type;
			}
			else num[j].type = ex[i].type;
		}
		else j --;
	}
	for(i = 0; i < j; i ++)
		if(ex[i].type == '+' || ex[i].type == '-')
		{
			ex[i + 1].str = rn(ex[i - 1].str, ex[i + 1].str, ex[i].type);
			ex[i - 1].type = -1;
			ex[i].type = -1;
		}
	
	for(i = 0; i < j; i ++)
	{
		if(num[i].type == TK_NUM)
			printf("%d", num[i].str);
		else
		{
			if(num[i].type == '+' || num[i].type == '-')
				printf("%c", (char)num[i].type);
		}
	}
	printf("\n");
	for(i = 0; i < j; i ++)
		if(num[i].type == '+' || num[i].type == '-')
			num[i + 1].str = rn(num[i - 1].str, num[i + 1].str, num[i].type);
	return num[j - 1].str;
}
*/
//add funtion
int divs(int l, int r)
{
	int i = 0, j = 0;
	int p = 0, f = 0, temp = 0;
	Token* stack = (Token*)malloc(sizeof(Token) * 32);
	Token* point = NULL;
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
			if(f == r && p == 1)
			{
				sign = error_2;
				return 0;
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
	/*
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
	*/
	point = cal(stack, 0, j);
	free(stack);
	return point->str;
}

//add funtion
void init_tokens()
{
	int i = 0;
	for(i = 0; i < 32; i ++)
	{
		tokens[i].str = 0;
		tokens[i].type = 0;
	}
}

word_t expr(char *e, bool *success) {
	int sum = 0;
	if (!make_token(e)) {
	  *success = false;
	  return 0;
	}

    /* TODO: Insert codes to evaluate the expression. */
//add code
    //TODO();
	sum = divs(0, nr_token);
	init_tokens();
	if(sign == error_1 || sign == error_2) return sign;
	else return sum;
	return 0;
}
