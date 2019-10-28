#define TAG "fs_jni"
#include "jniRegisiter.h"
#include <string.h>

#include "CCounter.h"
#include "file_operate/file_operate.h"
#include "call_so/call_so.h"
#include "call_static/call_static.h"
#include "thread/my_thread.h"
#include "http_curl/my_curl.h"
#include "my_stl/my_stl.h"

//日志
#include<android/log.h>
#define LOG_TAG "System.out.c"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

static const char* const kClassPathName = "jni/demo/com/jnidemo/jni/JniUtils";
static jint mGlobalObject = NULL;

//[1] jstring 转换 char*
char* jstringTostring(JNIEnv* env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring =  env->FindClass("java/lang/String");
    jstring strencode =  env->NewStringUTF("utf-8");
    jmethodID mid =  env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen =  env->GetArrayLength(barr);
    jbyte* ba =  env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0){
        rtn = (char*)malloc(alen + 1);
        //memcpy 函数用于 把资源内存拷贝到目标内存size拷贝的字节数
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}


//[2] char* 转换 jstring
jstring stoJstring(JNIEnv* env, const char* pat) {
    jclass strClass =  env->FindClass("java/lang/String");
    jmethodID ctorID =  env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes =  env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    jstring encoding =  env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}


jstring Java_jni_demo_com_jnidemo_jni_JniUtils_getTextString(JNIEnv* env, jclass clazz,jint index){
    jstring returnValue = NULL;
    char *myString = (char *)malloc(sizeof(char)*4);
    if(myString == NULL){
        /**不能分配足够的内存*/
    } else{
        myString[0] = 'a';
        myString[1] = 'b';
        myString[2] = 'c';
        myString[3] = (char)index;
        returnValue = stoJstring(env,myString);
        /**释放内存*/
        free(myString);
        myString = NULL;
    }
    return returnValue;
}

/** char数组使用 **/
jstring  Java_jni_demo_com_jnidemo_jni_JniUtils_getJniString(JNIEnv *env, jclass clazz){
    jchar chars[] = {'a','b','c','d'};
    jint length = sizeof(chars)/sizeof(jchar);

    //查找String的class类型
    jclass string_class_local = env->FindClass("java/lang/String");
    //得到String对象的构造方法
    jmethodID string_init_f = env->GetMethodID(string_class_local,"<init>","([C)V");
    //构造长度为len的字符数组
    jcharArray eleArray = env->NewCharArray(length);
    //将char指针里面从0到len的字符赋值到charArray
    env->SetCharArrayRegion(eleArray,0,length,chars);
    //新建一个字符串
    jstring result = (jstring)env->NewObject(string_class_local,string_init_f,eleArray);
    //对于使用完的字符数组要删除
    env->DeleteLocalRef(eleArray);

    LOGI("result");
    return result;
}

/** new c++对象 **/
void  Java_jni_demo_com_jnidemo_jni_JniUtils_newObject(JNIEnv *env, jclass clazz){
    CCounter* mGlObject = new CCounter(10);
    //全局引用
    mGlobalObject = (jint)mGlObject;
}

/** 调用c++对象 **/
jint  Java_jni_demo_com_jnidemo_jni_JniUtils_execute(JNIEnv *env, jclass clazz){
    CCounter *myObject = (CCounter *)mGlobalObject;
    return (jint)myObject->execute();
}


/******************************JNI 注册函数************************************/

static JNINativeMethod gMethods[] = {

//测试的函数
{"getTextString","(I)Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_getTextString},
{"getJniString","()Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_getJniString},
{"newObject","()V", (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_newObject},
{"execute","()I", (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_execute},

//文件操作相关函数
{"NativeFileOpen",       "(Ljava/lang/String;I)I",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileOpen},
{"NativeFileRead",       "(I[BI)I",                 (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileRead},
{"NativeFileWrite",      "(I[BI)I",                 (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileWrite},
{"NativeFileSeek",       "(IJI)J",                  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileSeek},
{"NativeFileClose",      "(I)I",                    (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileClose},

//调用so库
{"callSoLib",      "()I",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_callSoLib},

//调用静态库
{"callStaticLib",      "()I",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_callStaticLib},

//线程
{"initThread",      "()V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_initThread},
{"nativeFree",      "()V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_nativeFree},
{"posixThreads",      "(II)V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_posixThreads},


//curl网络请求和下载
{"TestDownload",      "()V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_TestDownload},
{"TestHttpPost",      "()V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_TestHttpPost},
{"Init",      "()V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_Init},
{"Cleanup",      "()V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_Cleanup},


//STL
{"startSTL",      "()V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_startSTL},
{"getPathWithSTL",      "(Ljava/lang/String;)V",  (void *)Java_jni_demo_com_jnidemo_jni_JniUtils_getPathWithSTL},




};

//注册函数的方法，JNI_LOAD
int register_jni_method(JNIEnv *env) {
    return jniRegisterNativeMethods(env, kClassPathName, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}

