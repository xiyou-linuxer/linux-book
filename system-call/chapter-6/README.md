### 信号是什么？

* 信号是一种很古老的IPC(进程间通信)方式。在早期的类Unix操作系统中，信号只能传递很简单的信息，即一个int(信号的编号)值。
* 信号是一种异步的消息传递方式或者说通知机制。
举一个不恰当的例子，你正在很兴奋地解决一个BUG，突然手机响了，女朋友说要分手。你立马放下手上的活，去处理女朋友那件事。手机随时都会响，随时都会中断你当下的事情。所以称之为异步事件。这个例子中的信号就体现在手机铃声响了。你去处理女朋友分手就是对信号做出的响应。
* 虽然信号是一种低级的IPC方式，但同时它保持了很简单的特性。在一些大型服务端程序中，很多时候也要考虑信号造成的影响。还是值得一学的。

从上面的例子可以看出，当产生一个信号(如:手机铃声响)，之后就要去响应这个信号，如何响应这个信号则需要编程人员来写一个函数。当产生信号时，程序去执行之这个函数(如:处理女朋友分手)。

### Linux下信号分两种.
* 第一种: 用于内核向进程通知事件.即传统或者标准信号.(信号编号由1~31)
* 第二种: 实时信号(信号编号由34~64)
* 0号信号用来测试对应进程是否存在或者是否由权限给其发送信号

### 一般程序收到信号时进程的现象:
 1. 忽略信号
 2. 终止(杀死)进程
 3. 产生核心转储文件,同时进程终止(核心转储文件即保存了程序跪掉的现场信息的文件，用来分析程序为何跪掉)。
 4. 停止进程
 5. 于之前暂停后再度恢复执行

### 编程可以做到的
1. 设置信号到来时采取默认的行为.
2. 忽略掉该信号.
3. 执行信号处理器程序.

### 常用信号类型即默认行为

|信号名称|产生的效果|对进程默认的效果|
|-----------|---------------------|----------------|
|SIGINT    |   Ctrl-C终端下产生|终止当前进程|
|SIGABRT|产生SIGABRT信号|默认终止进程，并产生core(核心转储)文件|
|SIGALRM|由定时器如alarm(),setitimer()等产生的定时器超时触发的信号|终止当前进程|
|SIGCHLD|子进程结束后向父进程发送|              忽略|
|SIGBUS|总线错误，即发生了某种内存访问错误|终止当前进程并产生核心转储文件|
|SIGKILL|必杀信号，收到信号的进程一定结束，不能捕获|终止进程|
|SIGPIPE|管道断裂，向已关闭的管道写操作|进程终止|
|SIGIO|使用fcntl注册I/O事件,当管道或者socket上由I/O时产生此信号|终止当前进程|
|SIGQUIT|在终端下Ctrl-\产生|终止当前进程，并产生core文件|
|SIGSEGV|对内存无效的访问导致即常见的“段错误”|终止当前进程，并产生core文件|
|SIGSTOP|必停信号，不能被阻塞，不能被捕捉|停止当前进程|
|SIGTERM|终止进程的标准信号  |终止当前进程|
0号信号用来测试对应进程是否存在或者是否有权限向对应进程发送该信号  (后面就理解了)  

### 如何设计信号处理函数？
* 一般而言，信号处理函数设计的越简单越好，因为当前代码的执行逻辑被打断，最好尽快恢复到刚才被打断之前的状态。从而避免竞争条件的产生。
* 在信号处理函数中，建议不要调用printf等与I/O相关的函数。以及一些慢设备操作。这样会使得信号处理函数的执行时间变长，可能，操作系统就会切换其它程序去在CPU上执行。但如果有特殊需要，则也可以使用。
* 在信号处理函数中，不要使用任何不可重入的函数后面会说到。保证信号处理函数可以安全地执行完。并不会影响主逻辑执行。


### 信号的发送与处理
来简单看一个实例，看看信号最简单的使用方式(当键入Ctrl-C时候即程序收到SIGINT信号时进行处理。)：
```C
//  signal 的函数原型
//  void (*signal(int sig , void (*func)(int)))(int);
//  至于这个怎么理解，这里就不再赘述了，请参考 《C Traps and Pitfalls》2.1节即理解函数声明。
// filename : simple_signal.cpp
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
	// 在信号处理程序中，尽量不要调用与标准IO相关的和不可重入的函数。
	
	write ( STDOUT_FILENO , MSG , strlen (MSG) ) ;
	do_too_heavy_work();
	write ( STDOUT_FILENO , MSG_END , strlen (MSG_END) ) ;
}

int main() {

	// 注册信号处理函数
	
	if ( SIG_ERR == signal ( SIGINT , sig_handler ) ) {
		fprintf (stderr , "signal error ") , perror ("") ;
		exit (1) ;
	}

	// 让主程序不退出，挂起，等待信号产生
	while (1) {
		pause () ;
	}

	return EXIT_SUCCESS ;
}

```
程序会一直停着等待用户行为。当我们键入Ctrl-C时程序打印相关信息，之后程序自己退出。那么程序的执行流程就类似这样：
```shell
[tutu@localhost Linux-Book]$ gcc simple_signal.cpp
[tutu@localhost Linux-Book]$ ./a.out 
^CCatch signal SIGINT processing 
Finished process SIGINT return 
^CCatch signal SIGINT processing 
Finished process SIGINT return 
^CCatch signal SIGINT processing 
Finished process SIGINT return 
```
这是一种古老的注册信号并设置信号处理函数的方式。现在我们使用新的信号注册函数即sigaction函数。它提供了更多的控制字段（旧的signal已经使用sigaction进行了实现。祥见glibc源码，我自己的Fedora 22 glibc 版本2.21可以在glibc官网[Glibc](http://ftp.gnu.org/gnu/glibc/)下载Glibc源码，对应路径下glibc/glibc-2.21/sysdeps/posix/signal.c看到）包括屏蔽信号集，设置高级信号处理函数等(稍后会详细讲述，别担心).

#### 为什么不是用signal来进行信号注册了？

* signal 无法设置在执行信号处理程序时要屏蔽哪些信号的产生。
* signal 函数注册的信号处理函数只能携带很少的信息(也不常用)，在信号处理函数进行信号处理时。 
* signal 无法设置一些标志位来执行一些动作(后面再讲)。
* signal 只能设置所给信号的处理方式但sigaction还可以获取之前这个信号的处理方式

#### 废话这么多，大家都嫌弃了，来一个真实的例子吧，和上面的程序功能一样，但是使用sigaction进行处理。
```C
// filename : simple_sigaction.cpp
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
```
执行效果和刚才的一样，在这里就不再贴过来了。
#### sigaction是怎么做到上述功能
首先sigaction比signal多了参数：
```C
int sigaction ( int sig , const struct sigaction * restrict act ,\
                struct sigaction * restrict oact ) ;
```
而结构体 struct sigaction act 里有什么呢？ 我们来看看：
man 3 sigaction 可以看到对应的sigaction结构体中的内容。

| Member Tyep|Member Name|Description|
|---------|:--------:|-----------|
|void (*)(int)|sa_handler|指向一个信号处理函数，或者使用SIG_IGN,SIG_DFL|
|sigset_t|sa_mask|在执行信号处理函数时，屏蔽sa_mask中的信号的产生|
|int |sa_flags|特殊的标志位，影响信号的处理行为|
|void (*) ( int , siginfo_t *   ,void*)|sa_sigaction|指向一个信号处理函数(当设置了SA_SIGINFO标志位时)|
以下几个函数也与上述内容相关：
```C
// 设置信号集为全空
int sigemptyset (sigset_t * set ) ;
// 设置信号集为全满
int sigfillset ( sigset_t * set ) ;
// 将信号signum添加到信号集中
int sigaddset ( sigset_t * set , int signum) ;
// 去除信号集中signum信号
int sigdelset ( sigset_t * set , int signum ) ;
// 判断signum信号是否在信号集set中
int sigismember ( const sigset_t * set , int signum ) ;
```
flags相关的选项理解：

* SA_SIGINFO: 设置此标志位表示可以使用sa_sigaction字段进行注册信号处理函数
* SA_RESTART: 被中断的系统调用自动重启
* SA_NODEFER: 在执行信号处理函数期间不屏蔽引起信号处理函数执行的信号

更多的标志位信息还请各位man 3 sigaction 这里只说常用的。

实际上判断某个信号是否在对应的信号集中，即判断对应信号在信号集中的那一个位是否被置1。去除信号集中的signum信号只需要对应位置0即可。
以上函数可以操作与sigset_t类型相关的变量。

当然除了在执行信号处理函数时候可以用sigaction设置要屏蔽的信号时，在程序中也可以给进程设置要屏蔽的信号。需要使用sigprocmask这个系统调用。
```C
int sigprocmask ( int how , const sigset_t *set , sigset_t * oldset ) ;
```
how字段的含义，具体使用大家man手册一下喵～
* SIG_SETMASK : 将set中的信号集设置为当前进程的阻塞信号集。
* SIG_BLOCK:将set中的信号集加到当前进程的阻塞信号集中。
* SIG_UNBLOCK:将set中的信号集从当前阻塞信号集中去除。

那么问题来了，为什么要设置屏蔽信号集？简单理解，就和你设置接电话的黑名单一样，你不想让这些电话中断你正在干的事情(什么原因我就不知道了)，同样的你不想让这些信号中断你程序正常运行(SIGSTOP和SIGKILL不能设置忽略或者捕捉，由于内核必须保证能够杀死(终止)一个进程，所以这两个信号就是这个作用)。通常情况下我们是要对SIGINT(终端下Ctrl-C),SIGTERM(kill 的默认信号),SIGQUIT(终端下Ctrl+\),SIGHUP(终端掉线网络断开收到)做一些处理(通常做一些IO同步如将还没有写到数据库中的要同步到数据库，将还在缓冲的东西同步到文件等或者是做一些清理销毁对象等工作)，假如你要写大型程序的时候。
#### 说了这么多关于，该说说如何如何发送信号了，各位久等了

1. 在终端下可以用kill/killall命令发送(缺省为SIGTERM信号)，如下：

```shell
[tutu@localhost Linux-Book]$ ps
  PID TTY          TIME CMD
 2711 pts/0    00:00:00 bash
 3990 pts/0    00:00:00 a.sh
 3991 pts/0    00:00:00 sleep
 3992 pts/0    00:00:00 ps
// kill 可以给一个进程组发送信号 详细的大家 man kill 
[tutu@localhost Linux-Book]$ kill -SIGKILL 3990
[tutu@localhost Linux-Book]$ ps
  PID TTY          TIME CMD
 2711 pts/0    00:00:00 bash
 3991 pts/0    00:00:00 sleep
 3993 pts/0    00:00:00 ps
[1]+  已杀死               ./a.sh
[tutu@localhost Linux-Book]$ 
```

2. 可以在终端下使用Ctrl+C(SIGINT信号),Ctrl+\(SIGQUIT信号).Ctrl+z(SIGSTOP信号)...
3. 在程序中可以使用:

```shell
// 给指定pid的进程发送信号
int kill(pid_t pid , int sig) ; 详细使用，大家man 2 kill
// 给当前进程发信号，也就是给自己发信号
int raise (int signo) ;
// 给进程号为pid的进程发送sig信号，并携带参数value(需要设置sigaction的flags的SA_SIGINFO标志才能使用)
int sigqueue ( pid_t pid , int sig , const union sigval value ) ;
// 虽然功能很好，但自己平时使用的比较少，或者基本不用
typedef union sigval {
     int sival_int;
  // sival_ptr 字段使用的较少，或者说几乎不用
     void __user *sival_ptr;
} sigval_t;
// 对应的结构定义如上。在sigaction函数中，对应struct sigaction中的信号处理函数的使用
```
4.与信号相关的函数

```C
// 一下详细内容均可以 man 手册！！！！！！
// 古老的定时函数，用来测试程序，粗略的定时函数
unsigned int alarm (unsigned int seconds) ;
// 新的定时函数，用于定时，当然也是简单的定时实现
int setitimer ( int which , const struct itimerval *new_value , \
                struct itimerval *old_value ) ;
// 产生SIGABRT信号，一般用于程序出现异常的情况下调用，如malloc分配内存失败。
void abort (void) ;
// 挂起进程，等待设置在set中的信号产生，通过sig返回发生的并在set中设置的信号。不屏蔽其它信号
int sigwait (const sigset_t * set , int *sig) ;
// 挂起进程等待设置在set中的信号产生，并接受携带的额外信息，返回产生的信号。不屏蔽其它信号
int sigwaitinfo (const sigset_t *set , siginfo_t *info ) ;
// 挂起进程等待信号产生，mask中的信号集在等待信号产生期间进行屏蔽
int sigsuspend (const sigset_t * mask ) ;
// 以上内容不能逐一举例，还望大家多加操练操练～喵～
```
说了这么多废话，来看一个实例吧 (例子比较简单)
```C
[tutu@localhost Linux-Book]$ cat sigqueue_post_signal.cpp
// 信号发送端：
// filename : sigqueue_post_signal.cpp
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
[tutu@localhost Linux-Book]$ gcc sigqueue_post_signal.cpp -o post_signal
[tutu@localhost Linux-Book]$ cat sigqueue_wait.cpp
// filename sigqueue_wait.cpp
// 信号接收端
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
[tutu@localhost Linux-Book]$ gcc sigqueue_wait.cpp -o wait_signal
[tutu@localhost Linux-Book]$ 
```
首先执行wait_signal，wait_signal 会打印自己的pid，再执行post_signal并传参数即pid，之后就可以看到效果了。
对于info->sig_value.sival_ptr字段，在这里暂时不讨论其使用，有兴趣的大家可以自行研究，因为用的十分少。
#### 信号的可靠性概述
上面说了，Linux下的信号分为：
* 不可靠信号：    [1~31]均为不可靠信号    
* 可靠信号：[34~64]为可靠信号或者叫实时信号

可靠信号是为了弥补Linux的不可靠信号以及用户可使用的信号太少的缺陷。
怎样理解可靠与不可靠？下面要引进几个概念：
* 未决信号：已经产生，但是没有给进程递送的信号(即还未处理)。
* 已被递送信号：已经递送给进程并处理过。

当我们在执行第一个示例程序的时候，如果在打印完
Catch signal SIGINT processing
之后，我们很快多次按下Ctrl-C。会发现当打印完
Finished process SIGINT return
后，仅会再响应一次信号。这是为什么？为什么我们按下那么多次Ctrl-C却只响应那么一次。原因其时就在于SIGINT是一个不可靠信号。
** 注意，不是说用sigaction注册的信号就是可靠的信号了。**
根本原因在于Linux的SIGINT本身就不是可靠的，也就是说，在信号处理函数处理期间或者信号被屏蔽期间多次产生该信号，当信号处理函数结束或者进程解除对该信号的屏蔽时只会再报告一次该信号。因为对于不可靠信号系统并没有给他们排队。也就多次产生只记录一次了。
说了这么多，测试一下：
```C
[tutu@localhost Linux-Book]$ gcc simple_signal.cpp 
[tutu@localhost Linux-Book]$ ./a.out 
^CCatch signal SIGINT processing 
^C^C^C^C^C^CFinished process SIGINT return 
Catch signal SIGINT processing 
Finished process SIGINT return 
```
证实了SIGINT不是可靠信号，因为多次发生后会丢失。如何验证信号被屏蔽后多次发生，再解除屏蔽后只会递送一次？需要用到sigprocmask函数。再自己写一个简单的脚本多次发送信号，就可以了。这里就不多进行演示了。
验证可靠信号(其时就是将第一个程序的注册信号改成SIGRTMIN，再改改信号处理函数中打印的信息)：
```C
// filenam : simple_realiable_signal.cpp
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#define 	MSG 		"Catch signal SIGRTMIN processing \n"
#define 	MSG_END 	"Finished process SIGRTMIN return \n"

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
	
	if ( SIG_ERR == signal ( SIGRTMIN , sig_handler ) ) {
		fprintf (stderr , "signal error ") , perror ("") ;
		exit (1) ;
	}

	// 让主程序不退出，挂起，等待信号产生
	while (1) {
		pause () ;
	}

	return EXIT_SUCCESS ;
}

```
还有一个脚本，用来发送信号：
```shell
#!/bin/bash
# 过滤出realiable_signal的pid
pid=$(ps aux|grep realiable_signal | grep -v grep | awk '{print $2}')
# 循环发送5次信号
for((i=0;i<5;i++))
do
	kill -34 $pid
done
```
执行一下看看，首先编译realiable_signal
```shell
[tutu@localhost Linux-Book]$ gcc simple_realiable_signal.cpp -o realiable_signal 
[tutu@localhost Linux-Book]$ ./realiable_signal

```
再起一个终端运行脚本：
```shell
[tutu@localhost Linux-Book]$ ./kill_SIGRTMIN.sh 
[tutu@localhost Linux-Book]$
```
看看运行结果：
```shell
[tutu@localhost Linux-Book]$ gcc simple_realiable_signal.cpp -o realiable_signal 
[tutu@localhost Linux-Book]$ ./realiable_signal 
Catch signal SIGRTMIN processing 
Finished process SIGRTMIN return 
Catch signal SIGRTMIN processing 
Finished process SIGRTMIN return 
Catch signal SIGRTMIN processing 
Finished process SIGRTMIN return 
Catch signal SIGRTMIN processing 
Finished process SIGRTMIN return 
Catch signal SIGRTMIN processing 
Finished process SIGRTMIN return 

```
讲讲本质上的原因，或许现在不能很快理解，想详细了解本质上的原因，大家可以去看内核源码的东西，这个是本质上的东西。我们看到的都是表象，内核里写的才是真实/原理的东西。
每个进程都会有一个结构体，来记录未决信号集，用户可以通过
```C
int sigpending (sigset_t *set ) ;
```
来获取进程的未决信号集(就是收到信号但是还没有处理的信号集)，对于不可靠信号集，当被屏蔽或者在信号处理函数正在执行的时候多次发生，只会将对应信号位置1。多次将一位置1的效果和只置一次1的效果相同，更核心的原因是，不可靠信号内核不负责给它排队记录。这就造成了不可靠。
对于可靠信号，存在一个双向链表，内核判断是否是可靠信号，如果是，那么即使是屏蔽了该可靠信号或者正在处理可靠信号的信号处理函数，在这期间多次发生，也会将其记录下来，链接到一个双向链表上，当进程解除了该信号的屏蔽或者处理完信号处理函数之后，将其再次递送给进程，直到链表上的信号全部递送完。这个在struct sigpending 结构体中(现在别看哈，直到这个概念就好，过早陷入内核，有点不知头绪，先打好数据结构，基础算法，C语言，一定哦^o^)。
#### 被中断的系统调用
当系统调用正在被执行的时候，发生了信号肿么办？一般情况下我们会这样做：
```C
ssize_t ret ;

while ( len != 0 && ( ret = read ( fd , buf , len ) != 0 ) {
	if ( -1 == ret ) {
// 通过判断errno即错误类型来片判断read出错是由于什么具体原因造成的
		if ( errno == EINTR ) {
			continue ;
		}	
		perror ("read error") ;
		break ;
	}
	len -= ret ;
	buf += ret ;
}
```
```C
ssize_t ret  ;

while (  0 != len && ( ret = write ( fd , buf , len)) != 0 ) {
	if ( -1 == ret ) {
		if ( errno == EINTR ) {
			continue ;
		}
		perror ( "write error ") ;
		break ;
	} 
	len -= ret ;
	buf += ret ;
}

```
但在这里想说的是：如read，write这样的系统调用内核已经支持其自动重启了，不需要放太多注意力在这里。当然有些系统调用或者说库函数，还是会出现这样的情况，相关牵扯到的还有sigaction 的flags的SA_RESTART标志位，详细的man 7 signal 介绍的很详细很详细。这里就不抄过来了。
### 几种常见的信号处理函数的设计方式：
1. 信号处理函数设置全局性标志变量并退出。主程序对此标志进行周期性检查，一旦标志被置位，则进行相应的处理动作（如果主程序监听一个或者多个文件描述符的I/O状态而无法进行自旋检测标志是否被设置，则可以创建一个管道，通过对管道的文件描述符进行监听，就可以在信号处理函数中向管道fd写入一字节内容，通过主程序中对管道fd事件的处理从而达到检测全局标志改变的事件。）。
简单看看一个例子：

```C
/*
   这个文件是用来实现通过设置标志位来判断程序是否在规定事件内完成输入，在超时事件内如果完成输入则打印输入内容，如果未完成则打印错误(超时)信息。
*/
//filename : sig_flag.cpp
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>

#define TIMED_OUT 5
#define BUFFER_SIZE 1024

// 这里是一个全局的标志
static volatile sig_atomic_t timedout_flag = 0;

// 原子的将 timedout_flag 置 1
void sig_handler(int signum) { 
/* 产生超时事件*/
 timedout_flag = 1;
}

int main() {
	struct sigaction old_act, new_act;

	new_act.sa_handler = sig_handler ;
	new_act.sa_flags = 0 ;
	sigemptyset (&new_act.sa_mask) ;

	// 注册信号处理函数， 对应定时器到时时调用sig_handler
	if (0 > sigaction(SIGALRM, &new_act, &old_act)) {
		fprintf(stderr, "sigaction error "), perror("");
		exit(1);
	}

	struct itimerval new_timer ;

	bzero (&new_timer , sizeof new_timer ) ;

	new_timer.it_value.tv_sec = TIMED_OUT;
	new_timer.it_value.tv_usec = 0;

	// 在这里启动定时器，开始等待是否超时。

	if ( 0 > setitimer (ITIMER_REAL , (const struct itimerval *)&new_timer , NULL ) ) {
		fprintf(stderr, "setitimer error , line %d\n" , __LINE__) ;
		exit(1);
	}

	// 在这里用read 来模拟任何阻塞的系统调用
	char buffer[BUFFER_SIZE] = {0};
	int bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);

	if ( 0 < bytes ) {
		printf("正常收到键盘输入。\n");
		// 设置全局标志为 0
		timedout_flag = 0 ;
	}

	// 判断是否产生超时事件 。
	if (1 == timedout_flag) {
		printf("等待超时!\n");
		// 在这里做一些清理操作，比如 close (fd) 等， free() 空间等。

		printf("Quit programm now .\n");
	} else {
		// 未发生超时事件，即正常输入了数据。
		// 取消闹钟
		new_timer.it_value.tv_usec = 0 ;
		if (0 > setitimer(ITIMER_REAL, &new_timer, NULL)) {
			fprintf(stderr, "setitmer error "), perror("");
			exit(1);
		}
		write(STDOUT_FILENO, buffer, strlen(buffer));
	}

	return 0;
}
```

2.信号处理函数值某种类型的清理操作。接着终止进程或者使用非本地跳转。将控制返回到主程序的预定位置。
// 这里涉及到非本地跳转，即:

```C
int setjmp ( jmp_buf env) ;
int longjmp ( jmp_buf env , int val) ;
int sigsetjmp ( sigjmp_buf env , int savesigs) ;
int siglongjmp ( sigjmp_buf env , int val) ;
```
首先需要理解上述函数的含义，两组函数的区别就是是否在跳转后恢复信号屏蔽集。前两个函数不保留，即在跳转后不恢复信号屏蔽集，后两个函数则恢复(当savesigs为非0详细的 man sigsetjmp)。
**  由于只是初学，就不研究这个例子了 **

#### 设置信号处理函数需要注意到的

* 应该了解到，每一个线程只有一个errno值，所以信号处理程序可能会修改errno的值。因此，作为一个通用的规则，应当在调用那些可能改变errno的函数之前保存errno值，之后再进行恢复。
* 在信号处理函数中调用一个非可重入函数，其结果未知的，所以尽量不要在信号处理
函数中使用非可重入函数 ， 判断是否是可重入函数有一个简单的原则：
1. 函数内部不使用操作静态变量。
2. 不使用与标准I/O相关函数。
3. 不使用malloc ，free() 函数。
4. 不调用其它非可重入函数。
关于可重入的概念，大家自行查找一下，或参考APUE，TLPI相关章节。简单理解就是可以同时被多个进程/线程执行的一段代码(函数)而不会出现错误。


本章完

参考文献及博客：

《Unix环境高级编程 第三版》

《The Linux Programming Interface》

http://blog.chinaunix.net/uid-24774106-id-4061386.html

上述链接共4篇

http://www.ibm.com/developerworks/cn/linux/l-ipc/part2/index1.html

上述链接共2篇


** 初学者以前两本书为主，当有Linux操作系统知识后可以详细参考后两个链接中内容。十分详实。喵～ **

.
