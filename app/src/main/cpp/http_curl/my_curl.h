
#ifndef JNIDEMO_MY_CURL_H
#define JNIDEMO_MY_CURL_H

#include <jni.h>

JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_TestDownload(JNIEnv* env,
                                                         jobject obj);
JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_TestHttpPost(JNIEnv* env,
                                                         jobject obj) ;

JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_Init(JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_Cleanup(JNIEnv* env, jobject obj);




#endif //JNIDEMO_MY_CURL_H
