#include "nemu.h"
#include "stdlib.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,NUMBER,TK_EQ
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
 	{"\\+", '+'},         // plus
	{"-", '-'},         // minus
	{"\\*", '*'},         // multiply
	{"/", '/'},           // divide
	{"\\(", '('},         // left
	{"\\)", ')'},       	// right
	{"[1-9]{0,}",NUMBER},// number
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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
  char str[32];
} Token;

Token tokens[32];
int nr_token;
static int m=0;
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

        Log("match rules[%d] = \"sdf  %s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
					case ' ':{
							continue;	break;
					 }	
          case '+':{
							tokens[m].type='+';strcpy(tokens[m].str,"hello");break;
					}
					case '-':{
							tokens[m].type='-';strcpy(tokens[m].str,"hello");break;
					}
					case '*':{
							tokens[m].type='*';strcpy(tokens[m].str,"hello");	break;
					}
					case '/':{
							tokens[m].type='/';strcpy(tokens[m].str,"hello");	break;
					}
          case '(':{
							tokens[m].type='(';strcpy(tokens[m].str,"hello");	break;
					}
          case ')':{
							tokens[m].type=')';strcpy(tokens[m].str,"hello");	break;
					}
					case 258:{
							tokens[m].type=258;//"=="
					}
          default:{
						//	printf("%s",substr_start);
							tokens[m].type=NUMBER;
							strcpy(tokens[m].str,substr_start);
							break;
					}
        }
				m++;
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


static int check_parentheses(int p,int q){
		int lag=0;
		for(int cou=0;cou<m-1;cou++){
				printf("mm%d  %d\n",cou,tokens[cou].type);
				if(tokens[cou].type=='('){ lag++;
				printf("pp%d\n",lag);}
				else{ lag--;
				printf("qq%d\n",lag);}
				if(lag<0){
						printf("The expression is wrong!\n");
						return 0;
				}
		}
		if(lag==0) return 1;
		else return 0;
}

static void eval(int p,int q){
		if(p>q){
         printf("Bad expression!\n");
				 assert(0);
		}
   else if(p==q){
	     if(strcmp(tokens[p].str,"hello")==0){
		         printf("Bad expression\n");
				     assert(0);
       }
		   else{

						int num=atoi(tokens[p].str);
			      printf("The result of the expression is:%d\n",num);
			}
    
	 }

   else if(check_parentheses(p,q)==true){
		//	return eval(p+1,q-1);
		printf("true\n");	
	 }
	 //else{
			

	 //}

}



uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
//	printf("%d",m);
	int p=0,q=m-1;

  /* TODO: Insert codes to evaluate the expression. */
  eval(p,q);
  return 0;
}
