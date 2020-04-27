#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#define FIFO 0
#define RR 1
#define SJF 2
#define PSJF 3

#define GET_TIME 334
#define PRINT_TIME 335

#define TIME_INTERVAL 500

#include <sys/types.h>
#include "process.h"

int schedule(int policy, int N, struct process * child);

#endif
