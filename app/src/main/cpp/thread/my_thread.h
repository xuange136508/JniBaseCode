//
// Created by Administrator on 2017/10/13.
//
#include <jni.h>
#ifndef JNIDEMO_MY_THREAD_H
#define JNIDEMO_MY_THREAD_H


JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_initThread( JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_posixThreads( JNIEnv* env, jobject thiz,jint threads,jint iterations);


JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_nativeFree( JNIEnv* env, jobject thiz);



#endif //JNIDEMO_MY_THREAD_H
