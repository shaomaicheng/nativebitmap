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

#include <map>

map<void* ,jobject> globals;

// define
jobject gVMRuntime;
jclass gVMRuntime_class;
jmethodID m;
jmethodID gVMRuntime_addressOf;

void initAboutEnv(JNIEnv *env) {
    const char *vmRuntimeName = "dalvik/system/VMRuntime";
    gVMRuntime_class = (jclass) env->NewGlobalRef(env->FindClass(vmRuntimeName));
    m = env->GetStaticMethodID(gVMRuntime_class, "getRuntime", "()Ldalvik/system/VMRuntime;");
    gVMRuntime = env->NewGlobalRef(env->CallStaticObjectMethod(gVMRuntime_class, m));
    gVMRuntime_addressOf = env->GetMethodID(gVMRuntime_class, "addressOf", "(Ljava/lang/Object;)J");
}

//hook图片内存分配
void *allocateHeapBitmapStub = nullptr;
void *orig = nullptr;

typedef sk_sp<android::Bitmap> (*bitmap_typede)(size_t size, void *info, size_t rowBytes);

static sk_sp<android::Bitmap> proxy(size_t size, void *info, size_t rowBytes) {
    LOGE("图片分配调用");
    LOGE("size:%zu, rowBytes:%zu", size, rowBytes);
    return ((bitmap_typede) orig)(size, info, rowBytes);
}

// hook android6
void *android6_stub = nullptr;
void *android6_orig = nullptr;

// hook android6 art addressof
void *addressOf_stub = nullptr;
void *addressOf_orig = nullptr;

typedef jbyteArray (*newNonMovableArray_typed)(JNIEnv *env, jobject obj, jclass javaElementClass,
                                               jint length);

jbyteArray newNonMovableArray_proxy(JNIEnv *env, jobject obj, jclass javaElementClass,
                                    jint length) {
//    LOGE("hook newNonMovableArray 内存分配, 分配length：%d", length);
    jbyteArray fakeArray = ((newNonMovableArray_typed) android6_orig)(env, obj, javaElementClass,1);
    jlong fakeAddr = env->CallLongMethod(gVMRuntime, gVMRuntime_addressOf, fakeArray);
//    LOGE("分配的fakeAddr地址：%lld", fakeAddr);
    *(void**) (fakeAddr - sizeof(int32_t)) = (void *) length;
    *(int32_t*)fakeAddr = 0x12345678;
    jobject globalRef = env->NewGlobalRef(fakeArray);
    globals[(void*)fakeAddr]=globalRef;

    if (length != env->GetArrayLength(fakeArray)) {
        return ((newNonMovableArray_typed) android6_orig)(env, obj, javaElementClass, length);
    } else {
        return fakeArray;
    }
}

typedef jbyte* (*addressOf_typedef)(JNIEnv* env, jobject, jobject javaArray);

static jbyte* addressOf_proxy(JNIEnv* env, jobject j, jbyteArray javaArray) {
    jbyte *p = ((addressOf_typedef) addressOf_orig)(env, j, javaArray);
    bool isNativeBitmap = *(int32_t*)p == 0x12345678;
    if (isNativeBitmap) {
        // 需要把原先分配在java heap的图片像素数组分配在native heap
        jsize size = env->GetArrayLength(javaArray);
        void* bitmap = calloc(size, 1);
//        LOGE("分配的bitmap地址:%p，此时p指针地址为:%p",bitmap, p);
        *(void**)(p+4) = bitmap;
        return static_cast<jbyte *>(bitmap);
    } else {
        return ((addressOf_typedef) addressOf_orig)(env, j, javaArray);
    }
}

void startHookAddressOf() {
    if (addressOf_stub != nullptr) {
        return;
    }
    const char *addressFunName = "_ZN3artL19VMRuntime_addressOfEP7_JNIEnvP8_jobjectS3_";
    addressOf_stub = shadowhook_hook_sym_name(
            "libart.so", addressFunName, (void *) addressOf_proxy,
            (void **) &addressOf_orig
    );
    if (addressOf_stub != nullptr) {
        LOGE("addressOf hook success");
    } else {
        LOGE("addressOf hook fail");
    }
}

void endHookAddressOf() {
    shadowhook_unhook(addressOf_stub);
    addressOf_stub = nullptr;
}



// hook android6 art DeleteWeakGlobalRef
void *delete_stub = nullptr;
void *delete_orig = nullptr;

typedef void (*delete_typedef)(JNIEnv * env, jweak obj);


void delete_proxy(JNIEnv* env, jweak obj) {
    endHookAddressOf();
    jlong weakAddress = env->CallLongMethod(gVMRuntime, gVMRuntime_addressOf, obj);
//    LOGE("weakAddress:%lld", weakAddress);
    if (weakAddress==0){
        ((delete_typedef) delete_orig)(env, obj);
        startHookAddressOf();
        return;
    }
    jbyte * p = (jbyte*) weakAddress;
    if(*(int32_t*)(p) == 0x12345678) {
        void* bitmap = *(void**)(p+4);
        jobject globalRef = globals[p];
        env->DeleteGlobalRef(globalRef);
        free(bitmap);
    }else{
        LOGE("不是nativebitmap内存释放，忽略！");
    }
    ((delete_typedef) delete_orig)(env, obj);
    startHookAddressOf();
}

void startHookDelete(){
    const char *deleteFunName = "_ZN3art3JNI19DeleteWeakGlobalRefEP7_JNIEnvP8_jobject";
    delete_stub = shadowhook_hook_sym_name(
            "libart.so", deleteFunName, (void *) delete_proxy, (void**)&delete_orig
            );
    if (delete_stub != nullptr) {
        LOGE("delete hook success");
    } else {
        LOGE("delete hook fail");
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_hook(JNIEnv *env, jobject thiz) {
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
        android6_stub = shadowhook_hook_sym_name("libart.so", android6Name,
                                                 (void *) newNonMovableArray_proxy,
                                                 (void **) &android6_orig);
        if (android6_stub != nullptr) {
            LOGE("android6 hook成功！");
        } else {
            LOGE("android6 hook失败！");
        }
        startHookAddressOf();
        startHookDelete();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_bhook(JNIEnv *env, jobject thiz) {
//    hookAndroid6BitmapAlloc(env);
//    hookAndroid10BitmapAlloc();
//    circleHook();
}