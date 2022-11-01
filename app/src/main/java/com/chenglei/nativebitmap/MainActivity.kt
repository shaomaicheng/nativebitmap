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
import androidx.appcompat.widget.LinearLayoutCompat

class MainActivity : AppCompatActivity() {
    private val imgContainer by lazy {
        findViewById<LinearLayoutCompat>(R.id.imgContainr)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)
//        NativeBitmap.init()
        for (i in 0..5) {
            val bitmap = BitmapFactory.decodeResource(resources, R.drawable.img_demo)
            val img = ImageView(this)
            imgContainer.addView(
                img, LinearLayoutCompat.LayoutParams(
                    LinearLayoutCompat.LayoutParams.WRAP_CONTENT,
                    LinearLayoutCompat.LayoutParams.WRAP_CONTENT
                )
            )
            img.setImageBitmap(bitmap)
        }
//        Log.e("chenglei","这是来自Java层的日志，size:${bitmap.byteCount}, row:${bitmap.rowBytes}")
    }
}

class MyApp: Application() {
    override fun onCreate() {
        super.onCreate()
//        NativeBitmap.init()
    }
}