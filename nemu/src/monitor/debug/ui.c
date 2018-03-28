#include"monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "monitor/expr.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Work it by single step and stop after N steps",cmd_si},
  { "info","Print register state",cmd_info},
  { "x","Scanning memory",cmd_x},
	{ "p","expression evaluation",cmd_p},
	{ "w","set watchpoint",cmd_w},
  /* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

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

static int cmd_si(char *args){
	char *arg=strtok(NULL," ");
	if(arg==NULL){
		cpu_exec(1);
		return 0;
	}
	int i=atoi(arg);
	if(i>=15){
		printf("The number should be allowed under 15\n");
		cpu_exec(-1);
	}
	else if(i==1||i==0){
		cpu_exec(1);
		return 0;
	}
	else if(i==-1) cpu_exec(-1);
	else cpu_exec(i);
	return 0;
}

static int cmd_info(char *args){
	char *arg=strtok(NULL," ");
	if(strcmp(arg,"r")==0){
		for(int i=0;i<8;i++){
			printf("%s         0x%08x           %d\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
		}
		printf("eip         0x%08x           %d\n",cpu.eip,cpu.eip);
	}
	return 0;
}

static int cmd_x(char *args){
	char *arg=strtok(NULL," ");
	int i=atoi(arg);
	unsigned int j=0,m;
	unsigned int p;
	char *temp = strtok(NULL," ");
	sscanf(temp,"%x",&j);
	printf("Address           Big-Endian           Little-Endian\n");
	for(int k=0;k<i;k++){
		m=paddr_read(j,4);
		printf("0x%08x        0x%08x           ",j,m);
		for(int q=0;q<4;q++){
			p=paddr_read(j+q,1);
			printf("%02x  ",p);
		}
		printf("\n");
		j=j+sizeof(int);
	}
	return 0;
}

uint32_t expr(char *e,bool *success);
static int cmd_p(char *args){
		bool succ;
		char *arg=strtok(NULL," ");
		expr(arg,&succ);
		return 0;
}

WP *new_wp();
void free_wp(WP *wp);
static int cmd_w(char *args){
		char *arg=strtok(NULL," ");
    if(arg==NULL) assert(0);
		bool succ=true;
		int value=expr(arg,&succ);
    WP *point=NULL;
		point=new_wp();
		strcpy(point->expression,arg);
    point->value=value;
	  printf("Watchpoint : %d  %s",point->NO,point->expression);
		return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
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

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
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
