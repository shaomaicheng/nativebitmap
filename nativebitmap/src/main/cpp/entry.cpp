#include <jni.h>
#include <base.h>
#include "android/storage_manager.h"
#include <jni.h>
#include <string>
#include <iostream>
#include "bytehook.h"
//
// Created by 程磊 on 2022/10/15.
//

#include "shadowhook.h"
#include "bytehook.h"
#include "include/core/SkImage.h"
#include "Bitmap.h"
#include "native_bitmap.h"

void* innerStub = nullptr;
void* innerOrig = nullptr;

class ADemo{
public:
    static int demoCount(int a);
    char * toString(){
        return "ADemo#toString";
    }

private:
    static int demoCount(int a, int b);
};

int ADemo::demoCount(int a){
    return demoCount(1,2);
}

int ADemo::demoCount(int a, int b) {
    return 20;
}

int inner_unique(int a,int b){
    LOGE("inner proxy");
    return 111;
}


//hook图片内存分配
void* allocateHeapBitmapStub = nullptr;
void *orig = nullptr;

typedef sk_sp<android::Bitmap> (*bitmap_typede)(size_t size, void* info, size_t rowBytes);
static sk_sp<android::Bitmap> proxy(size_t size, void* info, size_t rowBytes) {
    LOGE("图片分配调用");
    LOGE("size:%zu, rowBytes:%zu", size, rowBytes);
    return ((bitmap_typede)orig)(size, info, rowBytes);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_hook(JNIEnv *env) {
    // inline hook框架的demo
    innerStub = shadowhook_hook_sym_name("libnativeBitmap.so","_ZN5ADemo9demoCountEii",(void *) inner_unique, (void**)&innerOrig);
    if(innerStub){
        LOGE("内部hook成功");
    } else{
        LOGE("内部hook失败");
    }

    int c = ADemo().demoCount(1);
    LOGE("c:%d",c);


    // hook allocateHeapBitmap  这里是hook了内存分配的代码，代码在 libhwui.so 里面

    // 这个是使用 shadowhook 去找函数的地址，但是 sysname 好像也得传符号表里面的名字
    /*void *handler = shadowhook_dlopen("libhwui.so");
    void *addr = shadowhook_dlsym(handler, "_ZN7android6Bitmap18allocateHeapBitmapEjRK11SkImageInfoj");
    if (addr != nullptr) {
        LOGE("找到地址");
    } else {
        LOGE("没找到地址");
    }*/

    const char* name ="_ZN7android6Bitmap18allocateHeapBitmapEjRK11SkImageInfoj";
    allocateHeapBitmapStub = shadowhook_hook_sym_name(
            "libhwui.so",
            name,
            (void*) proxy,
            (void**) &orig
    );


    if (allocateHeapBitmapStub != nullptr) {
        LOGE("hook成功！");
    } else {
        LOGE("hook失败！");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_bhook(JNIEnv *env, jobject thiz) {
//    hookAndroid6BitmapAlloc(env);
//    hookAndroid10BitmapAlloc();
    circleHook();
}