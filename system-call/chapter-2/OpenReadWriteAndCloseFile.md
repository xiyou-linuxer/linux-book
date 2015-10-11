#文件的打开、读写和关闭

在C语言中，我们使用fopen、fscanf、fprintf和fclose等函数来使文件打开、读写和关闭。但在linux系统编程中，我们使用open、read、write和close系统调用对文件进行操作。我们先上代码：

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

int main(int argc , char * argv[])
{
    int fd;

    // 打开文件，得到文件描述符
    if ((fd = open("./1.text", O_WRONLY|O_CREAT|O_TRUNC, 0777)) < 0) {
        perror("open file error: ");
        exit(-1);
    }

    char writedata[100];
    char recvdata[100];

    // 准备要写入文件的数据
    strcpy(writedata, "Hello world!\n");

    // 在这里我们先向目标文件中写入数据
    // 接下来再从目标文件中读取数据

    // 给文件写入数据
    if (0 > write(fd, writedata, strlen(writedata)+1)) {
        perror("write data in file error: ");
        exit(-1);
    }
    // 关闭文件描述符
    close(fd);

    // 打开文件，得到文件描述符
    if ((fd = open("./1.text", O_RDONLY, 0777)) < 0) {
        perror("open file error: ");
        exit(-1);
    }

    // 从文件中读取数据保存到recvdata里
    if (0 > read(fd, &recvdata, 100)) {
        perror("read file error: ");
        exit(-1);
    }

    printf("%s", recvdata);
    close(fd);  //关闭文件描述符

    return EXIT_SUCCESS;
}
```

我们可以先分析一下这个代码。我们首先是使用open在当前目录下创建了一个名为“1.text”的文件。接下来我们使用write系统调用向该文件写“Hello World！”数据。我们关闭了文件描述符（这个必须要关闭，否则数据在缓冲区，并没有实际到达文件里面）。然后我们又再一次打开了该文件，并且得到文件描述符。我们使用read系统调用将文件里面的数据读取并存放到recvdata的字符数组里，打印之。现在有没有对这几个系统调用有个初步的认识呢？下面让我向大家具体来介绍这几个系统调用。

##系统调用——open

open系统调用既可以打开一个已存在的文件，也可以创建一个不存在的文件并打开。

```
#include<sys/stat.h>
#include<fcntl.h>

int open(const char * pathname , int flags , mode_t mode );

成功返回打开文件的文件描述符，失败返回-1并将errno置为相应的错误标识。

```
第一个参数是pathname，我们用它来说明需要打开文件的路径。如果是符号链接的话，会对其解引用（上面的例子，第一个参数是“./1.text”，表示在当前路径之下名为“1.text”文件）。
第二个参数是文件的访问模式（上面的例子，“ O_WRONLY|O_CREAT|O_TRUNC”这些就是文件访问的模式，下面我们具体讲）。
第三个参数表示文件访问权限的初始值（上面的例子， “ 0777”，表示文件所有者，所有者所在的组，其他人的权限都为可读可写可执行（可读（4），可写（2），可执行（1），具体参看一下《鸟哥的私房菜基础篇》）。

函数返回值：规定函数返回值为进程未用文件描述符中数值最小者。

**文件模式（参数flags）**  
文件模式就是说明了文件是用来干什么的，读文件？写文件？读写文件？   

| 文件访问模式 | 描述 |
|:----:|:----|
| O_RDONLY    | 以只读方式打开文件    | 
| O_WRONLY    | 以只写方式打开文件    |
| O_RDWR    | 以读写方式打开文件    |
| O_CREAT    |   若文件不存在则创建之  |
| O_DIRECT    |  无缓冲的输入输出  |
| O_EXCL    |  结合O_CREAT使用，专门用于创建文件  |
| O_TRUNC    | 截断已有文件，并清空文件 |
| O_APPEND    | 在文件的末尾追加数据 |
| O_NONBLOCK    |   已非阻塞方式打开  |
| O_ASYNC    |   当I/O操作可行时，产生信号（signal）通知进程   |
| O_DSYNC    |  提供同步的I/O数据完整性 |
| O_SYNC    | 以同步方式写入文件 |

PS：**当我们使用O_RDONLY、O_WRONLY、O_RDWR这三个访问模式，有且只能使用一种。这些模式，我们需要其中的多个的话，我们使用'|'号将它们取并集使用。**


##系统调用——creat  
早期open函数只有两个参数，并不能创建新的文件。因此，当时使用creat函数来创建并打开新文件。
```
#include<fcntl.h>

int creat(const char * pathname ,  mode_t mode);

成功返回文件描述符，失败返回-1并将errno置为相应的错误标识。
```  

第一个参数是目标文件的路径，若文件已存在，则打开该文件并清空文件内容。等同于：
```
fd = open(pathname , O_WRONLY | O_CREAT | O_TRUNC , mode);

```
不过现在使用open函数很方便，所以creat函数使用已经不太常见。

##系统调用——read

marking...

##系统调用——write

marking...

##系统调用——close

marking...
