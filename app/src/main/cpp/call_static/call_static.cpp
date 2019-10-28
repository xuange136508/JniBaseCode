// http://blog.csdn.net/ningjingsun/article/details/52960355
// [*] http://www.jianshu.com/p/5f29fd671750

//调用本地so库
#include "call_static.h"
#include "include/add.h"

//日志
#include<android/log.h>
#define LOG_TAG "System.out.c"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

jint Java_jni_demo_com_jnidemo_jni_JniUtils_callStaticLib(JNIEnv* env, jobject thiz){
    LOGD("call 外部静态库 ");
    int a = 55;
    int b = 55;
    int c = 0;
    c =  add(a,b);
    return c;
}