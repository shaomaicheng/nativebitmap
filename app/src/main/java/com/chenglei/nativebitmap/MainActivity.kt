package com.chenglei.nativebitmap

import android.app.Application
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.ImageView

class MainActivity : AppCompatActivity() {
    private val img by lazy {
        findViewById<ImageView>(R.id.img)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)
//        NativeBitmap.init()
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.img_demo)
        img.setImageBitmap(bitmap)
//        Log.e("chenglei","size:${bitmap.byteCount}, row:${bitmap.rowBytes}")
    }
}

class MyApp: Application() {
    override fun onCreate() {
        super.onCreate()
        NativeBitmap.init()
    }
}