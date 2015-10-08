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

marking...

##系统调用——read

marking...

##系统调用——write

marking...

##系统调用——close

marking...
