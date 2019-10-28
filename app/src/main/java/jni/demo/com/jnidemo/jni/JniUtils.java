package jni.demo.com.jnidemo.jni;

import android.util.Log;

/**
 * Created by Administrator on 2017/10/2.
 */

public class JniUtils {

    //保存c++对象
    private static long nativeId = 0;

    static
    {
        //导入本地so
        System.loadLibrary("ShitAdd");
        //网络请求
        System.loadLibrary("curl");
        //jni库
        System.loadLibrary("jni_demo");
    }

    //1:动态注册的方法
    public static native String getJniString();

    public static native void newObject();

    public static native int execute();

    public static native String getTextString(int index);



    //2:文件读写相关函数
    public native int NativeFileOpen(String filename, int flags);

    public native int NativeFileRead(int fd, byte[] buf, int sizes);

    public native int NativeFileWrite(int fd, byte[] buf, int sizes);

    //Offset：偏移量，每一读写操作所需要移动的距离，单位是字节的数量，可正可负（向前移，向后移）。
    public native long NativeFileSeek(int fd, long Offset, int whence);

    public native int NativeFileClose(int fd);



    //3:调用外部so
    public native int callSoLib();

    //4:调用外部静态库
    public native int callStaticLib();

    //5:线程
    public native void initThread();

    public native void nativeFree();

    //线程数量，迭代次数
    public native void posixThreads(int threads,int iterations);


    //6:网络请求和下载(libCurl)
    public native void Init();
    public native void Cleanup();
    public native void TestDownload();
    public native void TestHttpPost();
    //日志
    private void postDispose(String Data)
    {
        Log.i("curlTest", Data);
    }


    /**
     * 原生信息回调
     * (根据包名打印日志： jni.demo.com.jnidemo )
     * */
    public void onNativeMessage(String message){
        Log.i("jniUtils","回调结果："+message);
    }


    //7:STL
    public native void startSTL();
    public native void getPathWithSTL(String str);

}