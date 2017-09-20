#include	"unp.h"

// SIGCHLD & wait 父进程用信号处理函数来wait回收子进程
void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	pid = wait(&stat);
	printf("child %d terminated\n", pid);
	return;
}
