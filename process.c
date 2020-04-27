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

/*
int proc_exec(struct process proc)
{
	int pid = fork();

	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		unsigned long start_sec, start_nsec, end_sec, end_nsec;
		char to_dmesg[200];
		syscall(GET_TIME, &start_sec, &start_nsec);
		for (int i = 0; i < proc.t_exec; i++) {
			UNIT_T();
#ifdef DEBUG
			if (i % 100 == 0)
				fprintf(stderr, "%s: %d/%d\n", proc.name, i, proc.t_exec);
#endif
		}
		syscall(GET_TIME, &end_sec, &end_nsec);
		sprintf(to_dmesg, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_sec, start_nsec, end_sec, end_nsec);
		syscall(PRINTK, to_dmesg);
		exit(0);
	}
	
	proc_assign_cpu(pid, CHILD_CPU);

	return pid;
}
*/
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
