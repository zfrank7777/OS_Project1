#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define GET_TIME 314
#define PRINTK 315


void set_affinity(int pid, int affinity){
	cpu_set_t cpu_mask;
	CPU_ZERO(&cpu_mask);
	if (affinity >= 0) CPU_SET(affinity, &cpu_mask);
	if(sched_setaffinity(pid ,sizeof(cpu_set_t), &cpu_mask) != 0)
	{
		perror("sched_setaffinity error");
		exit(EXIT_FAILURE);
	}
}

int block_process(int pid)
{
	struct sched_param param;
	
	/* SCHED_IDLE should set priority to 0 */
	param.sched_priority = 0;

	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}

int wakeup_process(int pid)
{
	set_affinity(pid, CHILD_CPU);
	struct sched_param param;
	
	/* SCHED_OTHER should set priority to 0 */
	param.sched_priority = 0;

	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	param.sched_priority = 90;

	ret = sched_setscheduler(pid, SCHED_FIFO, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}
