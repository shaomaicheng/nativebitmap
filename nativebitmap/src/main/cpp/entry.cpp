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
#include "android/api-level.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_hello(JNIEnv *env) {
    int ret = android_get_device_api_level();
    LOGE("level:%d", ret);
}

void* allocateHeapBitmapStub = nullptr;
void *cStub = nullptr;
void *orig = nullptr;

typedef void* (*bitmap_typede)(void *thiz, void* bitmap);


void * proxy(void *thiz,void* bitmap) {
    LOGE("proxy调用");
    void* ret = ((bitmap_typede) orig)(thiz, bitmap);
    return ret;
}

void* orig_api_level = nullptr;

typedef int (*type_api_level)();

int unique_api_level(){
    LOGE("unique_api_level");
    return ((type_api_level) orig_api_level)();
//    return 0;
}

int shared_api_level(){
    LOGE("shared_api_level");
    int r = SHADOWHOOK_CALL_PREV(shared_api_level);
    SHADOWHOOK_POP_STACK();
    return r;
}

void* innerStub = nullptr;
void* innerOrig = nullptr;



class ADemo{
public:
    int demoCount();
    char * toString(){
        return "ADemo#toString";
    }
};

int ADemo::demoCount(){
    return 10;
}


//typedef int(* inner_orig)();
typedef int(* inner_orig)(void* thiz);

//int inner_unique(){
//    LOGE("inner proxy");
//    return 111;
//}

int inner_unique(void* thiz){
    LOGE("inner proxy");
    ADemo* demo =(ADemo*) thiz;
    LOGE("%s",demo->toString());
//    return 111;
    return ((inner_orig)innerOrig)(thiz);
}


int mincount() {
    return 10;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_hook(JNIEnv *env) {


    cStub = shadowhook_hook_sym_name(
            "libc.so",
            "android_get_device_api_level",
            (void *)unique_api_level,
            (void **)&orig_api_level
    );

    if(cStub != nullptr) {
        LOGE("libc hook成功");
    } else{
        LOGE("libc hook失败");
    }

    int ret = android_get_device_api_level();
    LOGE("%d",ret);

    innerStub = shadowhook_hook_sym_name("libnativeBitmap.so","_ZN5ADemo9demoCountEv",
                                         (void *) inner_unique, (void**)&innerOrig
                                         );
    if(innerStub){
        LOGE("内部hook成功");
    } else{
        LOGE("内部hook失败");
    }

//    int c = mincount();
    int c = ADemo().demoCount();
    LOGE("c:%d",c);


    // hook allocateHeapBitmap
    const char* name =
            "_ZN7android6Bitmap18allocateHeapBitmapERK11SkImageInfo";
//            "_ZN7android6Bitmap18allocateHeapBitmapEmRK11SkImageInfom";

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
