#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>


#define HIGH_PRIORITY (int)90
#define LOW_PRIORITY (int)0
#define MAX_PROCESS (int) 128
#define PARENT_CPU (int)0
#define CHILD_CPU (int)1


struct process
{
	char name[32];
	pid_t pid;
	int ready_time;
	int exec_time;	
	char start_time[32];
}; 


/* Assign process to specific core */
void set_affinity(int pid, int affinity);

/* Set very low priority tp process */
int block_process(int pid);

/* Set high priority to process */
int wakeup_process(int pid);

#endif
