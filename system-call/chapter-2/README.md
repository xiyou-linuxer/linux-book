# chapter-2 数据的管理和抽象——文件I/O

提到“文件”这个东西，想必大家都不陌生。在使用包括电脑在内的电子设备时，我们经常会使用“下载个文件”，“拷贝个文件”等说法。没错，“文件”便是计算机对于数据存储的一种最常见的抽象方法了。计算机将文字、声音、影像等数据在存储介质上以“文件”的形式组织和管理。而这一章介绍的便是如何使用系统调用函数对文件进行操作。

Linux中有一个很有趣的思想，那就是“一切皆文件”。可能你现在对这句话感到迷惑，没关系，先记下来，到后面学习了更多的内容后，我想你会有自己的理解的。

因此本章会跟大家具体的介绍关于文件I/O（Input/Output）的相关Linux系统调用，以及简单的介绍一下VFS（Virtual File System，虚拟文件系统）。在本章中，我们会有很多的代码贴出来，希望大家也可以自己动手练习。最后，你会发现，关于Linux下一些系统命令，你也可以自己实现。

-------------------------

本章介绍的系统调用包括但不限于：

- [open](http://linux.die.net/man/2/open)
- [creat](http://linux.die.net/man/2/creat)
- [close](http://linux.die.net/man/2/close)
- [read](http://linux.die.net/man/2/read)
- [write](http://linux.die.net/man/2/write)
- [lseek](http://linux.die.net/man/2/lseek)
- [fcntl](http://linux.die.net/man/2/fcntl)
