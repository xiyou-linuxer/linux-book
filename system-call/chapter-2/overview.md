#文件概述

在使用linux的过程中，我们会发现一个有趣的现象，那就是linux可以挂载windows文件系统(NTFS)的硬盘，但是反过来却不行。原因其实就是linux有VFS(Virtual File System)。正因为有了它，"一切皆文件"这个思想的到了实现。

上图来说明为什么VFS具有如此功效：

![](images/vfs.png)

