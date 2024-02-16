package com.julesgrc0.wispy;


import android.app.NativeActivity;
import android.os.Bundle;
import android.widget.Toast;


public class MainActivity extends NativeActivity {
    static {
        System.loadLibrary("main");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
}