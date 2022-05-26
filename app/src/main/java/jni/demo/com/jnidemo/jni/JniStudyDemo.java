package jni.demo.com.jnidemo.jni;


import jni.demo.com.jnidemo.bean.Person;

public class JniStudyDemo {

    static
    {
        System.loadLibrary("jni_spec");
    }

    //测试函数
    public static native String stringFromJNI();


    /**
     * Java 将数据传递到 native 中
     */
    public native void testData(
            boolean b,
            byte b1,
            char c,
            short s,
            long l,
            float f,
            double d,
            String name,
            int age,
            int[] i,
            String[] strs,
            Person person,
            boolean[] bArray
    );

}