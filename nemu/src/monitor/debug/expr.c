#include "nemu.h"
#include "stdlib.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,TK_EQ,TK_NQ,TK_AND,TK_OR,TK_NO,REG,HEX,NUMBER,DEFER
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
  {"==", TK_EQ},      	// equal
//	{"!=", TK_NQ},        //not equal
//	{"&&", TK_AND},       //and
 // {"||", TK_OR},	      //or
//	{"!",  TK_NO},        //no
	{"\\$e[a-dsi][xpi]",REG},  //register
	{"0x[A-Fa-f0-9]+",HEX}, //hexadecimal number
	{"[0-9]{0,}",NUMBER},// numberx
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
int m;

static bool make_token(char *e) {
  int position = 0;
  int i;
	m=0;
	char need[32];
  regmatch_t pmatch;  
  nr_token = 0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
       // Log("match rules[%d] = \"  %s\" at position %d with len %d: %.*s",
           // i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
					case TK_NOTYPE:{
							break;
					}	
          case '+':{
							tokens[m].type='+';
							strcpy(tokens[m].str,"1");
						  tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case '-':{
							tokens[m].type='-';
							strcpy(tokens[m].str,"1"); 
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case '*':{
							tokens[m].type='*';
							strcpy(tokens[m].str,"2");	
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case '/':{
							tokens[m].type='/';
							strcpy(tokens[m].str,"2");
						  tokens[m].str[substr_len] = '\0';m++;
							break;
					}
          case '(':{
							tokens[m].type='(';
							strcpy(tokens[m].str,"3");	
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
          case ')':{
							tokens[m].type=')';
							strcpy(tokens[m].str,"3");	
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case TK_EQ:{
							tokens[m].type=TK_EQ;//"=="
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case REG:{
							int neednum=0;
							tokens[m].type=REG;
              strncpy(need,substr_start,substr_len);
              need[4]='\0';
							if(strcmp(need,"$eax")==0) neednum=cpu.eax;
							else if(strcmp(need,"$ecx")==0) neednum=cpu.ecx;
							else if(strcmp(need,"$eip")==0) neednum=cpu.eip;
							else if(strcmp(need,"$edx")==0) neednum=cpu.edx;
						  else if(strcmp(need,"$ebx")==0) neednum=cpu.ebx;
						  else if(strcmp(need,"$esp")==0) neednum=cpu.esp;
							else if(strcmp(need,"$ebp")==0) neednum=cpu.ebp;
							else if(strcmp(need,"$esi")==0) neednum=cpu.esi;
							else if(strcmp(need,"$edi")==0) neednum=cpu.edi;
							sprintf(need,"%d",neednum);
							strcpy(tokens[m].str,need);
							m++;
							break;
					}
          case NUMBER:{
							tokens[m].type=NUMBER;
							strncpy(tokens[m].str,substr_start,substr_len);
							m++;
							break;
					}
					case HEX:{
							unsigned int hex;
              int oct=0;
						  tokens[m].type=HEX;
							strncpy(need,substr_start,substr_len);
							need[substr_len]= '\0';
							sscanf(need,"%x",&hex);
              oct=hex;
							sprintf(need,"%d",oct);
							strcpy(tokens[m].str,need);
							m++;
							break;
					}/*
					case TK_NQ:{
							tokens[m].type=TK_NQ;
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case TK_AND:{
							tokens[m].type=TK_AND;
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case TK_OR:{
						  tokens[m].type=TK_OR;
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}
					case TK_NO:{
							tokens[m].type=TK_NO;
							tokens[m].str[substr_len] = '\0';m++;
							break;
					}*/
					default : TODO();
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

static int check_parentheses(int p,int q){
		int lag=0;
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
		int abc=0;
		int num1[m],op=0;
		char str1[32];
		for(int count=p;count<=q;count++){
				 num1[count]=atoi(tokens[count].str);
		}
		//printf("%d  %d\n",p,q);
		int sta=10;
		if(p>q){
         printf("Bad expression!\n");
				 assert(0);
		}
    else if(p==q){
	     if(tokens[p].type=='+'||tokens[p].type=='-'||tokens[p].type=='*'||tokens[p].type=='/'||tokens[p].type=='('||tokens[p].type==')'){
						 printf("Bad expression\n");
				     assert(0);
       }
		   else{
						//printf("%d   ewr\n",num1[p]);
							 return num1[p];
			}
	 }
   else if(check_parentheses(p,q)==true){
		 return eval(p+1,q-1);
	 }
	 else{
        for(int count=p;count<=q;count++){
							 if(tokens[count].type!=NUMBER&&tokens[count].type!=HEX){
											 if(tokens[count].type=='('){
															 for(;count<=q;count++){
																			 if(tokens[count].type=='(') abc++;
																			 else if(tokens[count].type==')') abc--;
																			 if(tokens[count].type==')'&&abc==0) break;
															 }
											 }
											 else if(tokens[count].type==DEFER){
															 count++;
															 int flag=count;
															 if(tokens[count].type=='('){
																			 for(;count<=q;count++){
																							 if(tokens[count].type=='(') abc++;
																							 else if(tokens[count].type==')') abc--;
																							 if(tokens[count].type==')'&&abc==0){
																											 return paddr_read(eval(flag,count),4);
																											 break;
																							 }
																			 }
															 }
															 else return paddr_read(eval(count,count),4);
											 }
											 else  if(tokens[count].type=='-'){
															if(count==p){
																if(check_parentheses(p+1,q)==1) return -eval(p+1,q);
																 if(tokens[count+1].type==NUMBER){
															 					num1[count+1]=-num1[count+1];
																				for(int i=p;i<=q;i++){
																								num1[i]=num1[i+1];
																							  sprintf(str1,"%d",num1[i]);
																								tokens[i].type=tokens[i+1].type;
																								strcpy(tokens[i].str,str1);
																				}
																				for(int ok=q;ok<=q;ok++){
																								num1[ok]='\0';
																								tokens[ok].type=TK_NOTYPE;
																								strcpy(tokens[ok].str,"\0");
																				}
																			q--;
																}	 
														 }
															
														 else if(tokens[count-1].type!=')'&&tokens[count-1].type!=NUMBER&&tokens[count-1].type!=REG&&tokens[count-1].type!=HEX){
																				num1[count+1]=-num1[count+1];
																				for(int i=count;i<q;i++){
																								num1[i]=num1[i+1];
																							  sprintf(str1,"%d",num1[i]);
																								tokens[i].type=tokens[i+1].type;
																								strcpy(tokens[i].str,str1);
																				}
																			  for(int ok=q;ok<=q;ok++){
																								num1[ok]='\0';
																								tokens[ok].type=TK_NOTYPE;
																								strcpy(tokens[ok].str,"\0");
																			  }
																			q--;
														}
														else if(num1[count]<=sta&&num1[count]!=3){
																	 sta=num1[count];
																 op=count;
														}		
														
											 }
											 else if(num1[count]<=sta&&num1[count]!=3){						 
															 sta=num1[count];
                               op=count;
											 }
											 //printf("%d\n",op);
							 }
				}
        int val1=eval(p,op-1);
			  int val2=eval(op+1,q);
			  switch(tokens[op].type){
								case '+':return val1+val2;
								case '-':return val1-val2;
								case '*':return val1*val2;
								case '/':return val1/val2;
								default :assert(0);
				}	

	 }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
	int p=0,q=m-1;
  for(int i=0;i<m;i++){
					if(tokens[i].type=='*'&&(i==0||tokens[i-1].type=='+'||tokens[i-1].type=='-'||tokens[i-1].type=='*'||tokens[i-1].type=='/'||tokens[i-1].type=='(')){
							tokens[i].type=DEFER;
							strcpy(tokens[i].str,"4");
				  }
	}
	int lag=neg_num(p,q);
  /* TODO: Insert codes to evaluate the expression. */
  int result;
	if(lag==1){ 
					result=eval(p+1,q);
					result=-result;
	}
	else{
					result=eval(p,q);
	}
  return result;
}
