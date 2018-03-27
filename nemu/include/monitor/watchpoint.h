#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
	int new_value,old_value;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

#endif
