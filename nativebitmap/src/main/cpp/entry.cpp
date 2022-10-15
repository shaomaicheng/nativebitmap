#include <jni.h>

//
// Created by 程磊 on 2022/10/15.
//


#include "base/base.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_chenglei_nativebitmap_NativeBitmapJni_hello(JNIEnv *env, jclass clazz) {
    LOGE("hello,world");
}