package jni.demo.com.jnidemo.jni;

import android.util.Log;


public class JNativeInterface {

    //保存c++对象
    private static long nativeId = 0;

    static
    {
        System.loadLibrary("jni_spec");
    }

    public static native String getJniPackageName();

    public static native String getJniMd5();

    public static native String getDeviceCode();


    //日志
    private void postDispose(String Data)
    {
        Log.i("JNativeInterface", Data);
    }

    //测试函数
    public static native String stringFromJNI();

}