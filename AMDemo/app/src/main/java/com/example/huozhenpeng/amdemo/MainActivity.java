package com.example.huozhenpeng.amdemo;

import android.Manifest;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private ImageView iv_gif;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        iv_gif= (ImageView) findViewById(R.id.iv_gif);

    }

    private int REQUEST_CAMERA_CODE=1;
    private void requestCameraAccess() {

        ActivityCompat.requestPermissions(this, new String[]{ Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQUEST_CAMERA_CODE);

    }


    public void rquestPermission(View view)
    {
        requestCameraAccess();
    }

    private Bitmap bitmap;
    private int delay;
    private long gifPoint;
    public void  play(View view)
    {
        File file = new File(Environment.getExternalStorageDirectory(), "dashaguai.gif");
        gifPoint=loadGif(file.getAbsolutePath());
        int width=getWidth(gifPoint);
        int height=getHeight(gifPoint);
        bitmap=Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
        delay=updateFrame(bitmap,gifPoint);
        handler.sendEmptyMessageDelayed(0xa1,delay);

    }

    private Handler handler=new Handler()
    {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            iv_gif.setImageBitmap(bitmap);
            handler.sendEmptyMessageDelayed(0xa1,delay);
            delay=updateFrame(bitmap,gifPoint);

        }
    };

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public   native  long  loadGif(String path);


    public native int getWidth(long gifPoint);
    public native int getHeight(long gifPoint);

    public native int updateFrame(Bitmap bitmap,long gifPoint);
}


