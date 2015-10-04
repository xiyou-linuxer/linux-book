# chapter-2 数据的管理和抽象——文件I/O
从这一章起呢，我们就正式的步入linux系统编程的大门啦。linux之所以成功，就是因为它的"一起皆文件"的思想。因此本章会跟大家具体的介绍关于"文件IO"的相关linux系统调用,以及简单的跟大家介绍一下VFS(虚拟文件系统)。在本章中，我们会有很多的代码贴出来，希望大家也可以自己去动手去写写。最后，你会发现，关于linux下一些命令，你也可以自己实现。等不及了吧？
我们会介绍如下系统调用:

- [open](http://linux.die.net/man/2/open)
- [creat](http://linux.die.net/man/2/creat)
- [close](http://linux.die.net/man/2/close)
- [read](http://linux.die.net/man/2/read)
- [write](http://linux.die.net/man/2/write)
- [lseek](http://linux.die.net/man/2/lseek)
- [fcntl](http://linux.die.net/man/2/fcntl)

...

ps:本章我们假定你已经有C语言知识以及基本使用linux的前提。
