#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

// 注册SIGUSR1的信号处理函数
// man sigaction 显示，当前第三个参数无卵用，第一个参数是信号编号，第二个是携带的信息
void sig_handler (int signo , siginfo_t * info , void * extra ) {
	// print signo
	printf ("Catch SIGUSR1\n") ;
	printf ("signo is %d\n" , signo) ;	
	// print info -> si_value.sival_ptr
	printf ("sigval is %d\n" , info->si_value.sival_int ) ;
}

int main () {

	struct sigaction act ;
	act.sa_sigaction = sig_handler ;
	act.sa_flags = 0 ;
	act.sa_flags |= SA_SIGINFO ;
	sigemptyset (&act.sa_mask) ;

	printf ("My pid is %d\n" , getpid() ) ;

	if ( 0 > sigaction (SIGUSR1 , &act , NULL ) ) {
		fprintf (stderr , "sigaction error ") , perror ("") ;
		exit (1) ;
	}

	while (1) {
		pause () ;
	}

	return 0 ;
}

