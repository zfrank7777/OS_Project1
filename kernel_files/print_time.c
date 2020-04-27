#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>

asmlinkage void sys_print_time(int pid, char* st, char* ed) {
	printk("[Project1] %d %s %s\n", pid, st, ed);
}
