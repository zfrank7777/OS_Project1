#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scheduler.h"
#include "process.h"


int main(int argc, char* argv[])
{
	char policy_name[16];
	int policy;
	int N;

	scanf("%s", policy_name);
	scanf("%d", &N);

	struct process *child;
	child = (struct process*)malloc(N*sizeof(struct process));

	for (int i=0; i<N; i++)
	{
		scanf("%s%d%d", child[i].name, &child[i].ready_time, &child[i].exec_time);
	}

	if (strcmp(policy_name, "FIFO") == 0) policy = 0;
	else if (strcmp(policy_name, "RR") == 0) policy = 1;
	else if (strcmp(policy_name, "SJF") == 0) policy = 2;
	else if (strcmp(policy_name, "PSJF") == 0) policy = 3;
	else 
	{
		printf("invalid policy %s", policy_name);
		exit(0);
	}
	schedule(policy, N, child);
	return 0;
}
