#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp(){
				if(free_==NULL) assert(0);
				WP *point=free_;
				free_=free_->next;
				if(head==NULL) head=point;
				else{
								WP *point_=head;
								while(point_->next!=NULL) point_=point_->next;
								point_->next=point;
				}
				point->next=NULL;
				return point;
}

void free_wp(WP *wp){
				if(!(wp_pool <= wp && wp <=wp_pool+NR_WP)) assert(0);
				WP *point_1=head;
				WP *point_2=free_;
				if(point_1==wp) head=wp->next;
				else{
								while(point_1->next!=wp) point_1=point_1->next;
								point_1->next=NULL;
				}
				if(point_2==NULL){
								free_=wp;
								wp->next=NULL;
				}
				else{
								while(point_2->next!=NULL) point_2=point_2->next;
								point_2->next=wp;
								wp->next=NULL;
				}
}

void set_wp(char *args){
				WP *point=new_wp();
				bool succ;
				int value=expr(args,&succ);
				strcpy(point->expression,args);
				point->value=value;
				printf("Watchpoint : %d  %s\n",point->NO,point->expression);
}

int delete_wp(int n){
        WP *point=head;
				if(head==NULL) printf("No watchpoint!\n");
				while(point!=NULL){
								if(point->NO==n){
												free_wp(point);
												printf("delete watchpoint:NO    expr        value\n");
												printf("                  %d    %s      %d\n",point->NO,point->expression,point->value);
												break;
								}
								point=point->next;
				}
        if(point==NULL) return 0;
				return 1;
}

uint32_t expr(char *e,bool *success);
bool check_wp(){
				WP *point=head;
				bool succ;
				if(point==NULL) return false;
				while(point!=NULL){
							 uint32_t new_value=expr(point->expression,&succ);
						   if(point->value==new_value){
											 point=point->next;
							 }
							 else {
											// printf("NUM    OLD VALUE     NEWVALUE     WHAT\n");
											 //printf("%d     0x%x          0x%x         %s\n",point->NO,point->value,new_value,point->expression);
											 point->value=new_value;
											 return true;
							 }
							 point=point->next;
				}
				return false;
}

int show_wp(){
				WP *point=NULL;
        point=head;
				if(head==NULL){
							 	printf("no watchpoint!\n");
								return 0;
				}
				printf("Num      Value       What\n");
				while(point!=NULL){
								printf("%d        %d        %s\n",point->NO,point->value,point->expression);
								point=point->next;
				}
				return 0;
}
