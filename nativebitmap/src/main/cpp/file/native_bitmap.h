//
// Created by 程磊 on 2022/10/15.
//

#ifndef NATIVEBITMAP_NATIVE_BITMAP_H
#define NATIVEBITMAP_NATIVE_BITMAP_H

#endif //NATIVEBITMAP_NATIVE_BITMAP_H

#include "jni.h"

void hookAndroid10BitmapAlloc();
void hookAndroid6BitmapAlloc(JNIEnv *env);
void circleHook();