/* 文件操作的头文件 */
#include <jni.h>

#ifndef _Included_jni_demo_com_jnidemo_jni_JniUtils
#define _Included_jni_demo_com_jnidemo_jni_JniUtils
#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileOpen( JNIEnv* env, jobject thiz,jstring filename,jint flags);

JNIEXPORT jint JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileRead(JNIEnv* env, jobject thiz,int fd,jbyteArray buf,jint size);

JNIEXPORT jint JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileWrite(JNIEnv* env, jobject thiz,int fd,jbyteArray buf,jint size);

JNIEXPORT jlong JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileSeek(JNIEnv* env, jobject thiz,int fd,jlong Offset,jint whence);

JNIEXPORT jint JNICALL
Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileClose(JNIEnv* env, jobject thiz,int fd);


#ifdef __cplusplus
}
#endif
#endif
