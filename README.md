
`entry.cpp` 里面是使用 inline hook 库hook的
`native_bitmap.cpp` 里面使用的是 PLThook hook的。（bhook库）


如何在8以下hook bitmap分配：

1.hook  gVMRuntime_newNonMovableArray
```jave
jbyteArray arrayObj = (jbyteArray) env->CallObjectMethod(gVMRuntime,
                                                             gVMRuntime_newNonMovableArray,
                                                             gByte_class, size);
```
伪代码:
```
jbyteArray proxy(size) {
    return 原函数(新size)
}
```
这里返回的jbyteArray里面存一个标志位，一个假的size，假的size就是bitmap需要的size。
2.hook  gVMRuntime_addressOf
```cpp
jbyte* addr = (jbyte*) env->CallLongMethod(gVMRuntime, gVMRuntime_addressOf, arrayObj);
```

从 array 中获取 bitmapSize，并通过 calloc(bitmapSize，1) 在 Native 堆上为 Bitmap 分配内存；

把分配出来的 bitmap 指针保存到 fakeArray 的 9-12 字节中；

把 bitmap 指针返回，由原生逻辑在后续传递给 skia 使用；


伪代码:
```cpp
jbyte* address_proxy(jbyteArray arrayObj){
    if (hook图片) {
      
        return 地址;
    }else{
       return 原函数();
    }
}

```