//http://www.cnblogs.com/ZrBlog/p/4468774.html
//http://blog.csdn.net/hongdameng/article/details/52524733

#include "my_thread.h"
#include "../onLoad.h"

#include <pthread.h>

//日志
#include<android/log.h>
#include <stdio.h>
#include <unistd.h>

#define LOG_TAG "System.out.c"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

struct NativeWorkerArgs{
    jint id;
    jint iterations;
};

//能被缓存的方法
static jmethodID gOnNativeMessage = NULL;

//java虚拟机接口指针
//static JavaVM* gVm = NULL;
//直接从JNI_ON_LOAD获取

//互斥锁，保证线程的同步
static pthread_mutex_t mutex;

//对象全局引用
static jobject gObj = NULL;

/** 真正线程执行的函数 **/
void nativeWorker(JNIEnv* env, jobject obj,jint id,jint iterations){

    //锁定互斥锁
    if(0!=pthread_mutex_lock(&mutex)){
        jclass except = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(except,"Unable to Lock mutex");
        goto exit;
    }

    for(jint i=0;i<iterations;i++){
        //准备message
        char message[26];
        sprintf(message,"Worker %d Iteration %d",id,i);
        //来自c字符串message
        jstring messageStr = env->NewStringUTF(message);
        env->CallVoidMethod(obj,gOnNativeMessage,messageStr);
        //检查是否产生异常
        if(NULL!=env->ExceptionOccurred()){
            break;
        }
        //睡眠一秒
        sleep(1);
    }


    //解锁互斥锁
    if(0!=pthread_mutex_unlock(&mutex)){
        jclass except = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(except,"Unable to UnLock mutex");
    }

    exit:
    return;
}


/** 相当于Runnable **/
static void* nativeWorkerThread(void *args){
    JNIEnv *env = NULL;
    //将当前程序附加到java虚拟机上  并获得虚拟机指针
    if(0==g_Vm->AttachCurrentThread(&env,NULL)){
        //获取原生worker线程参数
        NativeWorkerArgs* nativeWorkerArgs = (NativeWorkerArgs*)args;
        //[*]线程上运行原生worker
        nativeWorker(env,gObj,nativeWorkerArgs->id,nativeWorkerArgs->iterations);

        //释放原生work线程参数
        delete nativeWorkerArgs;
        //从java虚拟机分离当前线程
        g_Vm->DetachCurrentThread();
    }
    return (void*)1;
}



void Java_jni_demo_com_jnidemo_jni_JniUtils_initThread( JNIEnv* env, jobject thiz){
    LOGD("线程初始化 ");
    //将java对象变成全局
    if(gObj==NULL){
        gObj = env->NewGlobalRef(thiz);
        if(gObj==NULL){
            goto exit;
        }
    }

    //初始化java函数，用于回调
    if(NULL == gOnNativeMessage){
        jclass  clazz = env->GetObjectClass(thiz);
        gOnNativeMessage = env->GetMethodID(clazz,"onNativeMessage","(Ljava/lang/String;)V");
        if(gOnNativeMessage==NULL){
            jclass except = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(except,"Unable to find method");
        }
    }

    //初始化互斥锁
    if(0!=pthread_mutex_init(&mutex,NULL)){
        jclass except = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(except,"Unable to init mutex");
    }

    exit:
        return;
}

void Java_jni_demo_com_jnidemo_jni_JniUtils_nativeFree( JNIEnv* env, jobject obj){
    LOGD("线程释放");

    if(gObj!=NULL){
        env->DeleteGlobalRef(gObj);
        gObj = NULL;
    }

    //销毁互斥锁
    if(0!=pthread_mutex_destroy(&mutex)){
        jclass except = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(except,"Unable to destroy mutex");
    }
}


/** 调用线程 **/
void Java_jni_demo_com_jnidemo_jni_JniUtils_posixThreads( JNIEnv* env, jobject obj,
                                                          jint threads,jint iterations){
    //线程句柄
    pthread_t * handlers = new pthread_t[threads];

    for(jint i=0;i<threads;i++){
        NativeWorkerArgs* nativeWorkerArgs = new NativeWorkerArgs();
        nativeWorkerArgs->id = i;
        nativeWorkerArgs->iterations = iterations;

        //创建线程
        int result = pthread_create(&handlers[i],NULL,nativeWorkerThread,(void*)nativeWorkerArgs);
        if(result!=0){
            jclass except = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(except,"Unable to create thread");
            goto exit;
        }
    }

    //等待线程终止
    for(jint i=0;i<threads;i++){
        void* result = NULL;
        //连接每个线程句柄(等待线程终止)
        if(0!=pthread_join(handlers[i],&result)){
            jclass except = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(except,"Unable to join thread");
        }else{
            //准备message
            char message[26];
            sprintf(message,"Worker %d returned %d",i,result);
            //来自c字符串message
            jstring messageStr = env->NewStringUTF(message);
            env->CallVoidMethod(obj,gOnNativeMessage,messageStr);
            //检查是否产生异常
            if(NULL!=env->ExceptionOccurred()){
                goto exit;
            }
        }
    }

    exit:
    return;
}

/*
 加上互斥锁后：
 10-14 17:36:00.826 28451-28451/jni.demo.com.jnidemo D/System.out.c: 线程初始化
 10-14 17:34:35.283 27409-27679/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 0
10-14 17:34:36.284 27409-27679/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 1
10-14 17:34:37.287 27409-27679/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 2
10-14 17:34:38.290 27409-27679/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 3
10-14 17:34:39.293 27409-27679/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 4
10-14 17:34:40.296 27409-27680/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 0
10-14 17:34:40.297 27409-27409/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 returned 1
10-14 17:34:41.298 27409-27680/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 1
10-14 17:34:42.299 27409-27680/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 2
10-14 17:34:43.301 27409-27680/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 3
10-14 17:34:44.302 27409-27680/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 4
10-14 17:34:45.305 27409-27409/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 returned 1

 没加：
 10-14 17:36:00.826 28451-28451/jni.demo.com.jnidemo D/System.out.c: 线程初始化
10-14 17:36:00.828 28451-28536/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 0
10-14 17:36:00.828 28451-28537/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 0
10-14 17:36:01.828 28451-28536/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 1
10-14 17:36:01.828 28451-28537/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 1
10-14 17:36:02.829 28451-28537/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 2
10-14 17:36:02.829 28451-28536/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 2
10-14 17:36:03.830 28451-28537/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 3
10-14 17:36:03.830 28451-28536/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 3
10-14 17:36:04.831 28451-28536/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 Iteration 4
10-14 17:36:04.832 28451-28537/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 Iteration 4
10-14 17:36:05.834 28451-28451/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 0 returned 1
10-14 17:36:05.834 28451-28451/jni.demo.com.jnidemo I/jniUtils: 回调结果：Worker 1 returned 1

 * **/
