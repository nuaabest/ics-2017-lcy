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
	{"[0-9]{0,}",NUMBER},// number
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

       // Log("match rules[%d] = \"sdf  %s\" at position %d with len %d: %.*s",
         //   i, rules[i].regex, position, substr_len, substr_len, substr_start);
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
							tokens[m].type='+';
							strcpy(tokens[m].str,"1");
						  tokens[m].str[substr_len] = '\0';	
							break;
					}
					case '-':{
							tokens[m].type='-';
							strcpy(tokens[m].str,"1"); 
							tokens[m].str[substr_len] = '\0';
							break;
					}
					case '*':{
							tokens[m].type='*';
							strcpy(tokens[m].str,"2");	
							tokens[m].str[substr_len] = '\0';
							break;
					}
					case '/':{
							tokens[m].type='/';
							strcpy(tokens[m].str,"2");
						  tokens[m].str[substr_len] = '\0';	
							break;
					}
          case '(':{
							tokens[m].type='(';
							strcpy(tokens[m].str,"3");	
							tokens[m].str[substr_len] = '\0';
							break;
					}
          case ')':{
							tokens[m].type=')';
							strcpy(tokens[m].str,"3");	
							tokens[m].str[substr_len] = '\0';
							break;
					}
					case 258:{
							tokens[m].type=258;//"=="
							tokens[m].str[substr_len] = '\0';
					}
          case 257:{
						//	printf("%s",substr_start);
							tokens[m].type=NUMBER;
							strncpy(tokens[m].str,substr_start,substr_len);
						  tokens[m].str[substr_len] = '\0';
							break;
					}
					default : TODO();
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
//		printf("%d %d\n",p,q);
		for(int cou=p;cou<=q;cou++){
				if(tokens[cou].type=='(')  lag++;
				else if(tokens[cou].type==')')  lag--;
				if(lag<0){
						printf("The expression is wrong!\n");
						return 0;
				}
				if(lag==0&&cou!=q) return 0;
		}
		if(lag==0) return 1;
		else return 0;
}

int neg_num(int p,int q){
				if(tokens[p].type=='-'&&check_parentheses(p+1,q)==true){
							return 1;
				}
				return 0;
}

int eval(int p,int q){
		//deal with '-'
		int lag=0,abc=0;
		int num1[m],op=0;
		for(int count=p;count<=q;count++){
				 num1[count]=atoi(tokens[count].str);
		}
		printf("%d  %d\n",p,q);
		int sta=10;
		if(p>q){
         printf("Bad expression!\n");
				 assert(0);
		}
   else if(p==q){
	     if(tokens[p].type=='+'||tokens[p].type=='-'||tokens[p].type=='*'||tokens[p].type=='/'){
		         printf("%d\n",tokens[p].type);
							 printf("Bad expression\n");
				     assert(0);
       }
		   else{
			     // printf("The result of the expression is:%d\n",num1[p]);
						return num1[p];
			}
    
	 }
   else if(check_parentheses(p,q)==true){
		 return eval(p+1,q-1);
	 }//
	 else{
        for(int count=p;count<=q;count++){
							 if(tokens[count].type!=NUMBER){
											 if(tokens[count].type=='('){
															 for(;count<=q;count++){
																			 if(tokens[count].type=='(') abc++;
																			 else if(tokens[count].type==')') abc--;
																			 if(tokens[count].type==')'&&abc==0){
																							printf("%d\n",count);
																							 break;}
															 }
											 }
											 
											 else  if(tokens[count].type=='-'){
															if(count==p){
																 if(tokens[count+1].type==NUMBER){
															 					num1[count+1]=-num1[count+1];
																			//	p++;
																				for(int i=count;i<q-p;i++){
																								num1[i]=num1[i+1];
																								tokens[i].type=tokens[i+1].type;
																								strcpy(tokens[i].str,tokens[i].str);
																				}
																				for(int ok=q;ok<=q;ok++){
																								num1[ok]='\0';
																								tokens[ok].type=TK_NOTYPE;
																								strcpy(tokens[ok].str,"hello");
																				}
																			//	p--;
																			q--;
																}	
														 }
															
														 else if(tokens[count-1].type!=')'&&tokens[count-1].type!=NUMBER){
																				num1[count+1]=-num1[count+1];
																			//	p++;
																				for(int i=count;i<q-p;i++){
																								num1[i]=num1[i+1];
																								tokens[i].type=tokens[i+1].type;
																								 strcpy(tokens[i].str,tokens[i].str);
																				}
																			  for(int ok=q;ok<=q;ok++){
																								num1[ok]='\0';
																								tokens[ok].type=TK_NOTYPE;
																								strcpy(tokens[ok].str,"hello");
																				}	
																			//	p--;
																			q--;
														}
												}

                       
											 else if(num1[count]<sta&&num1[count]!=3){
															 sta=num1[count];
                               op=count;
															 printf("%d\n",op);
											 }
							 }
				}
       // printf("mm%d             %dnn        %d\n",p,op,q);	
        int val1=eval(p,op-1);
				if((lag=neg_num(p,op-1))==1){val1=-val1;}
			  int val2=eval(op+1,q);
				if((lag=neg_num(op+1,q))==1){ val2=-val2;}
			  switch(tokens[op].type){
								case '+':return val1+val2;
								case '-':return val1-val2;
								case '*':return val1*val2;
								case '/':return val1/val2;
								default :assert(0);
				}	

	 }
   return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
//	printf("%d",m);
	int p=0,q=m-1;
	int lag=neg_num(p,q);
  /* TODO: Insert codes to evaluate the expression. */
  int result;//t=eval(p,q);
	if(lag==1){ 
					result=eval(p+1,q);
					result=-result;
					printf("result=%d\n",result);
	}
	else{
					result=eval(p,q);
					printf("result=%d\n",result);
	}
  return 0;
}
