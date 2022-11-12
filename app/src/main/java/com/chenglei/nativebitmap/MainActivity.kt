package com.chenglei.nativebitmap

import android.app.Application
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.widget.Button
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.LinearLayoutCompat
import androidx.core.view.forEach

class MainActivity : AppCompatActivity() {
    private val imgContainer by lazy {
        findViewById<LinearLayoutCompat>(R.id.imgContainr)
    }

    private val btnRecycle by lazy {
        findViewById<Button>(R.id.btnRecycle)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)
        NativeBitmap.init()

        val bitmaps = mutableListOf<Bitmap>()

        for (i in 0..50) {
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

        btnRecycle.setOnClickListener {
            imgContainer.removeAllViews()
            bitmaps.forEach {
                it.recycle()
            }
        }
    }
}

class MyApp: Application() {
    override fun onCreate() {
        super.onCreate()
//        NativeBitmap.init()
    }
}