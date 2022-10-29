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

// define
jobject gVMRuntime;
jclass gVMRuntime_class;
jmethodID m;
jmethodID gVMRuntime_addressOf;

void initAboutEnv(JNIEnv* env) {
    const char* vmRuntimeName = "dalvik/system/VMRuntime";
    gVMRuntime_class = (jclass)env->NewGlobalRef(env->FindClass(vmRuntimeName));
    m = env->GetStaticMethodID(gVMRuntime_class, "getRuntime", "()Ldalvik/system/VMRuntime;");
    gVMRuntime =  env->NewGlobalRef(env->CallStaticObjectMethod(gVMRuntime_class, m));
    gVMRuntime_addressOf = env->GetMethodID(gVMRuntime_class, "addressOf", "(Ljava/lang/Object;)J");
}

//hook图片内存分配
void *allocateHeapBitmapStub = nullptr;
void *orig = nullptr;

typedef sk_sp<android::Bitmap> (*bitmap_typede)(size_t size, void *info, size_t rowBytes);

static sk_sp <android::Bitmap> proxy(size_t size, void *info, size_t rowBytes) {
    LOGE("图片分配调用");
    LOGE("size:%zu, rowBytes:%zu", size, rowBytes);
    return ((bitmap_typede) orig)(size, info, rowBytes);
}

// hook android6
void *android6_stub = nullptr;
void *android6_orig = nullptr;

typedef jbyteArray (*newNonMovableArray_typed)(JNIEnv* env, jobject obj, jclass javaElementClass,
                                            jint length);

jbyteArray newNonMovableArray_proxy(JNIEnv* env, jobject obj, jclass javaElementClass,
                                 jint length) {
    LOGE("hook newNonMovableArray 内存分配, 分配length：%d", length);
    jbyteArray fakeArray = ((newNonMovableArray_typed)android6_orig)(env, obj, javaElementClass, 1);
    jlong fakeAddr = env->CallLongMethod(gVMRuntime, gVMRuntime_addressOf, fakeArray);
    *(void**)(fakeAddr - sizeof(int32_t)) = (void*)length;
    if (length != env->GetArrayLength(fakeArray)) {
        LOGE("length和fakearray size不一样！hook失败");
        return ((newNonMovableArray_typed)android6_orig)(env, obj, javaElementClass, length);
    } else {
        LOGE("length和fakearray size一样！hook成功");
        return ((newNonMovableArray_typed)android6_orig)(env, obj, javaElementClass, length);
//        return fakeArray;
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_hook(JNIEnv *env) {
    initAboutEnv(env);
    if (android_get_device_api_level() >= 26) {
        const char *name = "_ZN7android6Bitmap18allocateHeapBitmapEjRK11SkImageInfoj";
        allocateHeapBitmapStub = shadowhook_hook_sym_name(
                "libhwui.so",
                name,
                (void *) proxy,
                (void **) &orig
        );


        if (allocateHeapBitmapStub != nullptr) {
            LOGE("hook成功！");
        } else {
            LOGE("hook失败！");
        }
    } else {
        const char *android6Name = "_ZN3artL28VMRuntime_newNonMovableArrayEP7_JNIEnvP8_jobjectP7_jclassi";
        android6_stub = shadowhook_hook_sym_name("libart.so", android6Name, (void *) newNonMovableArray_proxy,
                                                 (void **) &android6_orig);
        if (android6_stub != nullptr) {
            LOGE("android6 hook成功！");
        } else {
            LOGE("android6 hook失败！");
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_bhook(JNIEnv *env, jobject thiz) {
//    hookAndroid6BitmapAlloc(env);
//    hookAndroid10BitmapAlloc();
//    circleHook();
}