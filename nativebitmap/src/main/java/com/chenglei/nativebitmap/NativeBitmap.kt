package com.chenglei.nativebitmap

class NativeBitmap {
    companion object {
        @JvmStatic
        fun hello() {
            NativeBitmapJni.hello()
        }
    }
}