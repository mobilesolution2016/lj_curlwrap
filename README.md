# lj_curlwrap
A wrap for libcurl in windows/linux for luajit

直接包装的CURL库，在Windows下采取了LoadLibrary然后获取必须的CURL函数，通过指针函数进行处理的方式，省去了对CURL在Windows麻烦的编译和使用库的过程
Linuxx下则直接使用libcurl.a编译

目前为比较初级的状态，仅仅是包装了CURL的一般性功能，但已经可以上传文件了，也可以用Json、Text等类型的数据来做POST请求。
尚未支持断点续传、断点上传或PUT、DELETE请求等更高级的特性。

如果需要异步执行，可以考虑的方式是采用LuaJIT的线程库来实现异步