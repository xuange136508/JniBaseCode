
#ifndef JNIDEMO_MY_STL_H
#define JNIDEMO_MY_STL_H

#include <jni.h>

JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_startSTL(JNIEnv* env,jobject obj);

JNIEXPORT void JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_getPathWithSTL(JNIEnv* env,jobject obj,jstring str);

#endif //JNIDEMO_MY_STL_H
