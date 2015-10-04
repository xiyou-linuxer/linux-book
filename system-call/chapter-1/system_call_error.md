# 系统调用的错误处理

上一节说到Linux系统调用的C语言接口在使用上和一般的C语言库函数并没有太大的差别，那么这一节先给出一个简单的例子来迈出我们使用系统调用的第一步：

```c
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char const * const str = "Hello World!\n";
    write(STDOUT_FILENO, str, strlen(str));

    return 0;
}
```

乍一看是不是比我们当年学习C语言的那个“Hello World”的例子稍微复杂一点呢。其实仔细看看，不同之处只是新加了头文件“unistd.h”，并把printf换成了write罢了。看来系统调用也没什么困难的吧？

不过需要注意的是，通常情况下，除了必然会成功返回的系统调用（比如getpid）之外，所有的系统调用的使用都不能简单的直接调用了事，而需要判断其是否成功，以便进行相应的错误处理。C语言函数通常通过返回值来表示函数执行的情况或者返回状态，系统调用也不例外。在Linux下可以通过man命令查阅系统调用的对应文档来查阅系统调用的相关信息。甚至在vim编辑器里，按下Esc键退出到普通模式下，当输入光标处于系统调用的函数名之上时，按下“shift+k”键（即大写K），就可以自动打开相关系统调用的man文档（这里有个缺陷，如果有系统命令和系统调用重名，会优先打开系统命令的man文档，需要我们另开一个终端，以“man 2 xxx”的方式指定在系统调用的文档里查看xxx的文档），按下“q”键即可退出到vim界面。

事实上大多数的系统调用以负数（例如返回“-1”）表示系统调用出现错误，而表示具体错误编号的整数存放在全局变量errno（事实上这不是个简单的C语言全局变量，以后会介绍的）里，我们包含了“error.h”这个头文件之后，便可以打印errno的错误信息了，例如：

```c
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char const * const str = "Hello World!\n";
    int rc = write(-1, str, strlen(str));
    if (rc < 0) {
        printf("write error, error code: %d\n", errno);
    }

    return 0;
}
```

编译运行后，输出“write error, error code: 9”，这个错误号9又是什么意思呢？看下表：

```c
code 1: Operation not permitted
code 2: No such file or directory
code 3: No such process
code 4: Interrupted system call
code 5: Input/output error
code 6: No such device or address
code 7: Argument list too long
code 8: Exec format error
code 9: Bad file descriptor
code 10: No child processes
code 11: Resource temporarily unavailable
code 12: Cannot allocate memory
code 13: Permission denied
code 14: Bad address
......
```

哦，看来错误号9指的是错误的文件描述符，是我们的参数有问题。什么是文件描述符？先不用管这个，学习了文件那一章之后自然会明白。这里你只需要知道Linux系统调用是如何告知错误的就可以了。不过，难不成要背下来这张表么？当然不能了，下面这个函数可以帮助我们把errno表示的错误翻译为相应的字符串：

```c
...
printf("write error, error info: %s\n", strerror(errno));
...
```

执行结果是“write error, error info: Bad file descriptor”。这样是不是很棒呢？那么有个这个函数，我们也能写个简单的for循环，打印出上面我给出的那个错误信息表呢。很简单哦，自己试试吧。

如果你觉得这样写“strerror(errno)”还是比较麻烦，也可以用这个函数perror，它会直接把错误信息打出来的。怎么用呢？还记得刚说的man文档查阅的方法么，直接在终端使用man命令查看文档呗。顺便说一下，man文档很多个区段，分别用man n + 要查询的关键字，例如“man 2 write”来检索。区段1是用户命令，2是系统调用，3是C程序库调用，4是设备，5是文件格式，6是游戏，7是杂项……剩下的我不写了，大家自己去谷歌吧。如果大家查阅某个函数在man 2里查不到，那就试试man 3吧，比如perror这个函数的文档。

好了，简单的错误处理就是这样，需要知晓更深入的东西。请大家自行查阅APUE或者TLPI这两本书。
