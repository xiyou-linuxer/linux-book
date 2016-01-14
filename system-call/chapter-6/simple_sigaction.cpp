#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#define 	MSG 		"Catch signal SIGINT processing \n"
#define 	MSG_END 	"Finished process SIGINT return \n"

void  do_too_heavy_work () {
	long long s = 0 ;

	for (long long i = 0 ; i < 500000000L ; i++ ) {
		s += i ;	
	}
}

void sig_handler (int signuum ) {
	// 本程序只是为了来进行演示，
	// 在信号处理程序中，尽量不要调用与标准IO相关的，不可重入的函数。
	
	write ( STDOUT_FILENO , MSG , strlen (MSG) ) ;
	do_too_heavy_work();
	write ( STDOUT_FILENO , MSG_END , strlen (MSG_END) ) ;
}

int main() {

	// 注册信号处理函数
	struct sigaction  newact ;

	// 将信号处理函数执行期间掩码设置为空
	sigemptyset (&newact.sa_mask ) ;
	// 将标志设置为0即默认
	newact.sa_flags = 0 ;
	newact.sa_flags |= SA_NODEFER ;
	// 注册信号处理函数
	newact.sa_handler = sig_handler ;

	if ( 0 > sigaction ( SIGINT , &newact , NULL ) ) {
		fprintf (stderr , "sigaction error ") , perror ("") ;
		exit (1) ;
	}

	// 让主程序不退出，挂起，等待信号产生
	while (1) {
		pause () ;
	}

	return EXIT_SUCCESS ;
}

