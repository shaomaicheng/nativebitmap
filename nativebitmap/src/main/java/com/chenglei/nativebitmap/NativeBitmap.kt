package com.chenglei.nativebitmap

import com.bytedance.android.bytehook.ByteHook
import com.bytedance.shadowhook.ShadowHook


class NativeBitmap {
    companion object {
        fun init() {
            System.loadLibrary("nativeBitmap")
            ByteHook.init(
                ByteHook.ConfigBuilder().setDebug(true)
                .build())
            ShadowHook.init(ShadowHook.ConfigBuilder().setDebuggable(true)
                .setMode(ShadowHook.Mode.UNIQUE)
//                .setMode(ShadowHook.Mode.SHARED)
                .build())
            NativeBitmapJni().hook()
        }
    }
}