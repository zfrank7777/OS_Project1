#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>
#include <stdbool.h>

#include "scheduler.h"
#include "process.h"
#include <sys/syscall.h>


void WORK()
{
	volatile unsigned long i; 
	for(i=0; i<1000000UL; i++);
}

int cmpfunc(const void * a, const void * b)
{
	struct process *p1 = (struct process *) a;
	struct process *p2 = (struct process *) b;
	if (p1->ready_time == p2->ready_time)
		return (p1->ready_time - p2->ready_time);
	return (p1->pid - p2->pid);
}


int this_round(int policy, int running, struct process * child)
{
	if (policy == FIFO || policy == SJF) return child[running].exec_time;
	else if (policy == RR) 
		return (TIME_INTERVAL < child[running].exec_time)? TIME_INTERVAL: child[running].exec_time;
	else return 1;
}


int pick_next_process(int policy, int N, int running, int* waiting, struct process * child)
{
	if (running != -1)
		if (child[running].exec_time != 0 && (policy == FIFO || policy == SJF)) return running;
	if (policy == FIFO) {
		for (int w=0; w<N; w++)
			if ((waiting[w] != -1) && child[waiting[w]].exec_time > 0)
				return waiting[w];
		return -1;
	} else if (policy == RR) {
		/* new: search waiting process with the most early ready_time */ 
		int min_ready_time = 2147483647;
		for (int w=0; w<N; w++)
			if (waiting[w] != -1 && child[waiting[w]].exec_time > 0 && 
			   child[waiting[w]].ready_time >= 0 && min_ready_time > child[waiting[w]].ready_time)
				min_ready_time = child[waiting[w]].ready_time;
		for (int w=(running+1)%N; w<N; w++)
			if ((waiting[w] != -1) && child[waiting[w]].exec_time > 0 && 
			    child[waiting[w]].ready_time == min_ready_time)
				return waiting[w];
		for (int w=0; w<=running; w++)
			if ((waiting[w] != -1) && child[waiting[w]].exec_time > 0 && 
			    child[waiting[w]].ready_time == min_ready_time)
				return waiting[w];
		return -1;
	} else {  // SJF & PSJF
		int min_exec = 2147483647;
		int idx = -1;
		for (int w=0; w<N; w++)
			if ((waiting[w] != -1) && child[waiting[w]].exec_time > 0)
				if (child[waiting[w]].exec_time < min_exec)
					{min_exec = child[waiting[w]].exec_time; idx = waiting[w];}
		return idx;
	}
}


int schedule(int policy, int N, struct process * child)
{

	/* sort process by ready time*/
	qsort(child, N, sizeof(struct process), cmpfunc);

	/* init children with pid = -1 as not started*/ 
	for (int i=0;i<N;i++) {child[i].pid=-1; strcpy(child[i].start_time, "");}

	int current_time = 0;
	int pid;
	cpu_set_t set;

	/* give cpu to the schduler */
	set_affinity(getpid(), PARENT_CPU);
	struct sched_param param;
	param.sched_priority = HIGH_PRIORITY;
	if(sched_setscheduler(getpid(), SCHED_FIFO, &param) != 0)
	{
		perror("sched_setscheduler error");
		exit(EXIT_FAILURE);
	}

	long long int st_sec, ed_sec;
	int st_nsec, ed_nsec;
	char st[32];
	char ed[32];
	struct timespec ts;
	int waiting_n = 0;
	int finished_n = 0;
	int waiting[MAX_PROCESS] = {};
	for (int i=0; i<N; i++) waiting[i] = -1;
	int running = -1;
	while (finished_n < N)
	{
		for (int i=0;i<N;i++)
		{
			if (child[i].pid == -1 && child[i].ready_time <= current_time)
			{
				CPU_ZERO(&set);
				pid = fork();
				if (pid < 0) 
				{
					perror("fork");
					return -1;
				}
				else if (pid == 0)  // child: wait
				{
					// block_process(getpid());
					// sched_yield();
					fprintf(stdout, "%s %d\n", child[i].name, getpid());
					for (int i=0; i< child[i].exec_time; i++) WORK();
					return 0;
				}
				else  // parent: put child into wait
				{
					child[i].pid = pid;
					waiting[waiting_n] = i;
					waiting_n += 1;
				}
			}
		}
		
		/* pick one process and run */
		running = pick_next_process(policy, N, running, waiting, child);  // index in waiting
		if (running == -1) {current_time ++; continue;}
		int round_time = this_round(policy, running, child);
		if (strcmp(child[running].start_time, "") == 0)
		{
			syscall(GET_TIME, &st_sec, &st_nsec);
			sprintf(st, "%lld.%.9d", st_sec, st_nsec);
			strcpy(child[running].start_time, st);
		}
		set_affinity(child[running].pid, CHILD_CPU);
		wakeup_process(child[running].pid);
	
		for (int i=0; i<round_time; i++) WORK(); 
		current_time += round_time;
		
		/* new: update ready_time for priority in RR */
		child[running].ready_time = current_time;

		child[running].exec_time -= round_time;
		if (child[running].exec_time < 0) printf("ERROR: exec time < 0\n");
		if (child[running].exec_time == 0)
		{
			waitpid(child[running].pid);
			syscall(GET_TIME, &ed_sec, &ed_nsec);
			sprintf(ed, "%lld.%.9d", ed_sec, ed_nsec);
			syscall(PRINT_TIME, child[running].pid, child[running].start_time, ed);
			finished_n += 1;
		}
	}
}
