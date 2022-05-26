#include "jniRegisiter.h"

#include <string>
#include <iostream>

//日志
#include<android/log.h>
#define TAG "native-lib"
// __VA_ARGS__ 代表 ...的可变参数
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG,  __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG,  __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG,  __VA_ARGS__);

using namespace std;



//测试函数
jstring Java_jni_demo_com_jnidemo_jni_JniStudyDemo_stringFromJNI(JNIEnv *env, jclass clazz){


    jclass newExcCls = env->FindClass("java/lang/IllegalArgumentException");
                env->ThrowNew(newExcCls, "JNI 中发生了一个异常信息"); // 返回一个新的异常到 Java
    //捕获异常
    jthrowable exc = env->ExceptionOccurred(); // 检测是否发生异常
    if (exc) {//如果发生异常
        env->ExceptionDescribe(); // 打印异常信息
        env->ExceptionClear(); // 清除掉发生的异常
    }


    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"//支持 C 语言代码
JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniStudyDemo_testData(JNIEnv *env, jobject instance,
                                              jboolean jboolean1,
                                              jbyte jbyte1,
                                              jchar jchar1,
                                              jshort jshort1,
                                              jlong jlong1,
                                              jfloat jfloat1,
                                              jdouble jdouble1,
                                              jstring name_,
                                              jint age,
                                              jintArray i_,
                                              jobjectArray strs,
                                              jobject person,
                                              jbooleanArray bArray_
                                             ) {

    //1. 接收 Java 传递过来的 boolean 值
    unsigned char b_boolean = jboolean1;
    LOGD("boolean-> %d", b_boolean);

    //2. 接收 Java 传递过来的 boolean 值
    char c_byte = jbyte1;
    LOGD("jbyte-> %d", c_byte);


    //3. 接收 Java 传递过来的 char 值
    unsigned short c_char = jchar1;
    LOGD("char-> %d", c_char);


    //4. 接收 Java 传递过来的 short 值
    short s_short = jshort1;
    LOGD("short-> %d", s_short);

    //5. 接收 Java 传递过来的 long 值
    long l_long = jlong1;
    LOGD("long-> %d", l_long);

    //6. 接收 Java 传递过来的 float 值
    float f_float = jfloat1;
    LOGD("float-> %f", f_float);

    //7. 接收 Java 传递过来的 double 值
    double d_double = jdouble1;
    LOGD("double-> %f", d_double);

    //8. 接收 Java 传递过来的 String 值
    const char *name_string = env->GetStringUTFChars(name_, 0);
    LOGD("string-> %s", name_string);

    //9. 接收 Java 传递过来的 int 值
    int age_java = age;
    LOGD("int:%d", age_java);

    //10. 打印 Java 传递过来的 int []
    jint *intArray = env->GetIntArrayElements(i_, NULL);
    //拿到数组长度
    jsize intArraySize = env->GetArrayLength(i_);
    for (int i = 0; i < intArraySize; ++i) {
        LOGD("intArray->%d：", intArray[i]);
    }
    //释放数组
    env->ReleaseIntArrayElements(i_, intArray, 0);

    //11. 打印 Java 传递过来的 String[]
    jsize stringArrayLength = env->GetArrayLength(strs);
    for (int i = 0; i < stringArrayLength; ++i) {
        jobject jobject1 = env->GetObjectArrayElement(strs, i);
        //强转 JNI String
        jstring stringArrayData = static_cast<jstring >(jobject1);

        //转 C  String
        const char *itemStr = env->GetStringUTFChars(stringArrayData, NULL);
        LOGD("String[%d]: %s", i, itemStr);
        //回收 String[]
        env->ReleaseStringUTFChars(stringArrayData, itemStr);
    }



    //12. 打印 Java 传递过来的 Object 对象
    //12.1 获取字节码
    const char *person_class_str = "jni/demo/com/jnidemo/bean/Person";
    //12.2 转 jni jclass
    jclass person_class = env->FindClass(person_class_str);
    //12.3 拿到方法签名 javap -a
    const char *sig = "()Ljava/lang/String;";
    jmethodID jmethodID1 = env->GetMethodID(person_class, "getName", sig);

    jobject obj_string = env->CallObjectMethod(person, jmethodID1);
    jstring perStr = static_cast<jstring >(obj_string);
    const char *itemStr2 = env->GetStringUTFChars(perStr, NULL);
    LOGD("Person: %s", itemStr2);
    env->DeleteLocalRef(person_class); // 回收
    env->DeleteLocalRef(person); // 回收


    //13. 打印 Java 传递过来的 booleanArray
    jsize booArrayLength = env->GetArrayLength(bArray_);
    jboolean *bArray = env->GetBooleanArrayElements(bArray_, NULL);
    for (int i = 0; i < booArrayLength; ++i) {
        bool b =  bArray[i];
        jboolean b2 =  bArray[i];
        LOGD("boolean:%d",b)
        LOGD("jboolean:%d",b2)
    }
    //回收
    env->ReleaseBooleanArrayElements(bArray_, bArray, 0);

}



/**JNI 注册函数**/
static JNINativeMethod gMethods[] = {
{"stringFromJNI","()Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JniStudyDemo_stringFromJNI},
{"testData","(ZBCSJFDLjava/lang/String;I[I[Ljava/lang/String;Ljni/demo/com/jnidemo/bean/Person;[Z)V", (void *)Java_jni_demo_com_jnidemo_jni_JniStudyDemo_testData},
};
//注册函数的方法，JNI_LOAD
int register_jni_method(JNIEnv *env) {
    return jniRegisterNativeMethods(env, "jni/demo/com/jnidemo/jni/JniStudyDemo", gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}