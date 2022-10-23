//
// Created by 程磊 on 2022/10/15.
//

#include <base.h>
#include "native_bitmap.h"
#include "bytehook.h"
#include <jni.h>
#include "Bitmap.h"


// 尝试用bhook代理
void *hookAndroid10BitmapStub= nullptr;
sk_sp<android::Bitmap> allocateHeapBitmap_proxy(size_t size, const SkImageInfo& i, size_t rowBytes) {
    LOGE("allocateHeapBitmap_proxy 代理");
    LOGE("代理函数里的分配,size:%zu, rowBytes:%zu", size, rowBytes);
    return BYTEHOOK_CALL_PREV(allocateHeapBitmap_proxy, size, i, rowBytes);
}


void callback_10(
        bytehook_stub_t task_stub, //存根，全局唯一，用于unhook
        int status_code, //hook执行状态码
        const char *caller_path_name, //调用者的pathname或basename
        const char *sym_name, //函数名
        void *new_func, //新函数地址
        void *prev_func, //原函数地址
        void *arg)//添加hook回调时指定的hooked_arg值
{
    LOGE("callback10 status_code:%d", status_code);
}

void hookAndroid10BitmapAlloc() {
    LOGE("hookAndroid10BitmapAlloc");
    hookAndroid10BitmapStub = bytehook_hook_single(
            "libhwui.so",
            nullptr,
            "_ZN7android6Bitmap18allocateHeapBitmapEjRK11SkImageInfoj",
            reinterpret_cast<void *>(allocateHeapBitmap_proxy),
            callback_10,
            nullptr
    );
}





void* hookAndroid6BitmapStub = nullptr;

jobject VMRuntime_newNonMovableArray_proxy(JNIEnv* env, jobject, jclass javaElementClass,
                                     jint length) {
    LOGE("hook newNonMovableArray 内存分配");
    return nullptr;
}


jclass make_globalref(JNIEnv* env, const char classname[])
{
    jclass c = env->FindClass(classname);
    return (jclass) env->NewGlobalRef(c);
}


 void callback(
        bytehook_stub_t task_stub, //存根，全局唯一，用于unhook
        int status_code, //hook执行状态码
        const char *caller_path_name, //调用者的pathname或basename
        const char *sym_name, //函数名
        void *new_func, //新函数地址
        void *prev_func, //原函数地址
        void *arg)//添加hook回调时指定的hooked_arg值
 {
     LOGE("status_code:%d", status_code);
 }

void hookAndroid6BitmapAlloc(JNIEnv *env) {
    hookAndroid6BitmapStub = bytehook_hook_single(
            "libart.so",
            nullptr,
            "VMRuntime_newNonMovableArray",
            reinterpret_cast<void*>(VMRuntime_newNonMovableArray_proxy),
            callback,
            nullptr
            );

    jclass gVMRuntime_class = make_globalref(env, "dalvik/system/VMRuntime");
    jmethodID m = env->GetStaticMethodID(gVMRuntime_class, "getRuntime", "()Ldalvik/system/VMRuntime;");
    jobject gVMRuntime = env->NewGlobalRef(env->CallStaticObjectMethod(gVMRuntime_class, m));
    jmethodID gVMRuntime_newNonMovableArray = env->GetMethodID(gVMRuntime_class, "newNonMovableArray",
                                                     "(Ljava/lang/Class;I)Ljava/lang/Object;");

    jclass c;
    c = env->FindClass("java/lang/Byte");
    env->NewGlobalRef(
            env->GetStaticObjectField(c, env->GetStaticFieldID(c, "TYPE", "Ljava/lang/Class;")));

    jclass gByte_class = (jclass) env->NewGlobalRef(
            env->GetStaticObjectField(c, env->GetStaticFieldID(c, "TYPE", "Ljava/lang/Class;")));

    jbyteArray arrayObj = (jbyteArray) env->CallObjectMethod(gVMRuntime,
                                       gVMRuntime_newNonMovableArray,
                                       gByte_class, 10);



    int length = env->GetArrayLength(arrayObj);
    LOGE("length:%d", length);

}




// 循环调用的demo
void *circleStub= nullptr;

void* circle_hook_proxy(size_t __byte_count) {
    BYTEHOOK_STACK_SCOPE();
    if (__byte_count == 200) {
        LOGE("分配300");
        return BYTEHOOK_CALL_PREV(circle_hook_proxy, 300);
    }else{
        LOGE("circle_hook_proxy 代理，分配数量：%d", __byte_count);
        return BYTEHOOK_CALL_PREV(circle_hook_proxy,__byte_count);
    }
}

void circleHook(){
    circleStub = bytehook_hook_single(
            "libnativeBitmap.so",
            nullptr,
            "malloc",
            reinterpret_cast<void *>(circle_hook_proxy),
            nullptr,
            nullptr);
    malloc(200);
    malloc(100);
}