package jni.demo.com.jnidemo;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.UnsupportedEncodingException;

import jni.demo.com.jnidemo.jni.JniUtils;

public class MainActivity extends AppCompatActivity {


    private JniUtils mJniUtils;

    //文件标识符
    private int fd;
    private String filename = Environment.getExternalStorageDirectory()+ File.separator+"jniTest.txt";
    byte[] buf_write;
    byte[] buf_read;

    //文件操作相关Flag
    int O_ACCMODE  =    0003;
    int O_RDONLY   =      00;
    int O_WRONLY   =      01;
    int O_RDWR     =      02;
    int O_CREAT    =    0100; /* not fcntl */
    int O_EXCL     =    0200; /* not fcntl */
    int O_NOCTTY   =   0400; /* not fcntl */
    int O_TRUNC    =   01000; /* not fcntl */
    int O_APPEND   =   02000;
    int O_NONBLOCK =   04000;
    int O_NDELAY   = O_NONBLOCK;
    int O_SYNC     =  010000;
    int O_FSYNC    =  O_SYNC;
    int O_ASYNC    =  020000;

    //Seek相关Flag
    int SEEK_SET   =        0;//将读写位置指向文件头后再增加offset个位移量。
    int SEEK_CUR   =        1;//以目前的读写位置往后增加offset个位移量。
    int EEK_END    =        2;//将读写位置指向文件尾后再增加offset个位移量。

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mJniUtils = new JniUtils();

        //初始化网络请求
        mJniUtils.Init();

        findViewById(R.id.btn_test1).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*测试
                JniUtils.newObject();
                toast(JniUtils.execute()+"");
                toast(JniUtils.getTextString(6));
                */
            }
        });
        findViewById(R.id.btn_test2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //动态库
                //toast(mJniUtils.callSoLib()+"");
                //静态库
                //toast(mJniUtils.callStaticLib()+"");

                //线程相关：
                //初始化
                //mJniUtils.initThread();
                //调用线程(2个线程，每个迭代5次)
                //mJniUtils.posixThreads(2,5);

                //网络请求
                //mJniUtils.TestHttpPost();
                //下载
                //mJniUtils.TestDownload();

                //STL操作
                //mJniUtils.startSTL();
                String ext = Environment.getExternalStorageDirectory() + File.separator + "360";
                mJniUtils.getPathWithSTL(ext);

            }
        });
        findViewById(R.id.btn_test3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {



            }
        });


    }


    private void toast(String mDemo){
        Toast.makeText(this, mDemo,Toast.LENGTH_SHORT).show();
    }


    @Override
    protected void onDestroy() {
        //释放线程
        //mJniUtils.nativeFree();

        //释放网络请求
        //mJniUtils.Cleanup();
        super.onDestroy();
    }

    //文件写
    private void fileWrite(){
        if(new File(filename).exists()){
            toast("文件存在");
        }
        //打开
        fd = mJniUtils.NativeFileOpen(filename, O_CREAT  | O_RDWR);
        System.out.println("fd_write---->" + fd);
        buf_write = "hello Word!!".getBytes();
        //写入
        int ret_write = mJniUtils.NativeFileWrite(fd, buf_write, buf_write.length);
        System.out.println("写入返回结果" + ret_write);
        //关闭
        mJniUtils.NativeFileClose(fd);
    }

    //文件读
    private void fileRead(){
        //打开
        fd = mJniUtils.NativeFileOpen(filename, O_CREAT  | O_RDWR);
        System.out.println("fd_read---->" + fd);
        buf_read = new byte[buf_write.length];
        //读写
        int ret_read = mJniUtils.NativeFileRead(fd, buf_read, buf_write.length);
        System.out.println("读出返回结果" + ret_read);
        try {
            String result = new String(buf_read, "GB2312");
            toast(result);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        //关闭
        mJniUtils.NativeFileClose(fd);
    }


}
