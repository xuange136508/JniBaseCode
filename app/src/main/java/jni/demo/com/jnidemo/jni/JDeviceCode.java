package jni.demo.com.jnidemo.jni;

import android.util.Log;


public class JDeviceCode {


    static
    {
        System.loadLibrary("jni_spec");
    }

    public static native String getDeviceCode();


}