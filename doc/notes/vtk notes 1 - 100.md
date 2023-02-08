#### 1. VTK 类所属库文件

VTK 有许多库，在编写 VTK 应用程序时，使用不用的类需要调用不用的库，那如何分辨某一类文件所属哪一个库文件？

以 vtkSmartPointer.h 类文件为例，该文件在源码中的路径为 \VTK-9.1.0\src\Common\Core，Common + Core，则该类所在的库文件名即 vtkCommonCore-9.1.lib。

