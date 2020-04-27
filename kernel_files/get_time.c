#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>

asmlinkage void sys_get_time(long *sec, long *nsec) {
	struct timespec ts;
	getnstimeofday(&ts);
	*sec = ts.tv_sec;
	*nsec = ts.tv_nsec;
}
