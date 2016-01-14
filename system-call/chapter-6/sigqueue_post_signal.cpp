#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

int main (int argc , char * argv[]) {

	if ( 2 != argc ) {
		fprintf (stderr , "Bad argument!\nUsage ./post_signal pid\n") ;
		exit (1) ;
	}

	pid_t 	pid = atoi ( argv[argc-1] ) ;
	printf ("Sending signal to %d , by using sigqueue\n" , pid) ;
	sigval_t 	sigval ;
	sigval.sival_int = 8888 ;
	int errcode = 0 ;

	if ( 0 > ( errcode = sigqueue ( pid , SIGUSR1 , sigval ) )) {
		if ( ESRCH == errcode ) {
			fprintf (stderr , "No such process!\n") ;
			exit (1) ;
		} else {
			fprintf (stderr , "sigqueue error "),perror ("")  ;
			exit (1) ;
		}
	}

	printf ("Finished!\n") ;

	return 0 ;
}

