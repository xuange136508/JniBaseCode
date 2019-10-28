//
// Created by Administrator on 2017/10/11.
//
#include <jni.h>

#ifndef JNIDEMO_CALL_SO_H
#define JNIDEMO_CALL_SO_H


JNIEXPORT jint JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_callSoLib( JNIEnv* env, jobject thiz);


#endif //JNIDEMO_CALL_SO_H
