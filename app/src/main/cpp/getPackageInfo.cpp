#include "jniRegisiter.h"
#include <string.h>
//#include <stdlib.h>
//#include <jni.h>

//-------------------------------------获取包名-----------------------------------------------------

/**
 * 获取Android的包名信息
 */
jstring  getPackageNameInfo(JNIEnv *env, jclass clazz){
    //获取Android的上下文对象
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID get_Application = env->GetStaticMethodID(activity_thread_clz,"currentActivityThread","()Landroid/app/ActivityThread;");
        if (get_Application != NULL) {
            jobject currentActivityThread = env->CallStaticObjectMethod(activity_thread_clz,get_Application);
            jmethodID getal = env->GetMethodID(activity_thread_clz, "getApplication","()Landroid/app/Application;");
            application = env->CallObjectMethod(currentActivityThread, getal);
        }
    }
    if (application == NULL) {
        return NULL;
    }
    jclass activity = env->GetObjectClass(application);
    jmethodID methodId_pack = env->GetMethodID(activity, "getPackageName", "()Ljava/lang/String;");
    jstring name_str = static_cast<jstring >( env->CallObjectMethod(application, methodId_pack));
    return name_str;
}


//-----------------------------------------获取MD5签名------------------------------------------------------

//将byte数组转换成16进制字符串
void ByteToHexStr(const char *source, char *dest, int sourceLen) {
    short i;
    char highByte, lowByte;
    for (i = 0; i < sourceLen; i++) {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f;
        highByte += 0x30;
        if (highByte > 0x39) {
            dest[i * 2] = highByte + 0x07;
        } else {
            dest[i * 2] = highByte;
        }
        lowByte += 0x30;
        if (lowByte > 0x39) {
            dest[i * 2 + 1] = lowByte + 0x07;
        } else {
            dest[i * 2 + 1] = lowByte;
        }
    }
}
/**
 * 获取签名MD5
 */
jstring getSignMD5Info(JNIEnv *env, jclass clazz){
    // 调用 getContext 方法得到 Context 对象
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        }
        env->DeleteLocalRef(activity_thread_clz);
    }

    //获取应用签名的具体实现
    jclass cls = env->GetObjectClass(application);
    jmethodID mid = env->GetMethodID(cls, "getPackageManager","()Landroid/content/pm/PackageManager;");
    jobject pm = env->CallObjectMethod(application, mid);
    mid = env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
    jstring packageName = (jstring) env->CallObjectMethod(application, mid);
    cls = env->GetObjectClass(pm);
    mid = env->GetMethodID(cls, "getPackageInfo","(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject packageInfo = env->CallObjectMethod(pm, mid, packageName, 0x40); //GET_SIGNATURES = 64;
    cls = env->GetObjectClass(packageInfo);
    jfieldID fid = env->GetFieldID(cls, "signatures", "[Landroid/content/pm/Signature;");
    jobjectArray signatures = (jobjectArray) env->GetObjectField(packageInfo, fid);
    jobject signature = env->GetObjectArrayElement(signatures, 0);
    cls = env->GetObjectClass(signature);
    mid = env->GetMethodID(cls, "toByteArray", "()[B");
    jbyteArray signatureByteArray = (jbyteArray) env->CallObjectMethod(signature, mid);

    // byte数组转MD5字符串
    jclass classMessageDigest = env->FindClass("java/security/MessageDigest");
    jmethodID midGetInstance = env->GetStaticMethodID(classMessageDigest, "getInstance","(Ljava/lang/String;)Ljava/security/MessageDigest;");
    jobject objMessageDigest = env->CallStaticObjectMethod(classMessageDigest, midGetInstance,env->NewStringUTF("md5"));
    jmethodID midUpdate = env->GetMethodID(classMessageDigest, "update", "([B)V");
    env->CallVoidMethod(objMessageDigest, midUpdate, signatureByteArray);
    jmethodID midDigest = env->GetMethodID(classMessageDigest, "digest", "()[B");
    jbyteArray objArraySign = (jbyteArray) env->CallObjectMethod(objMessageDigest, midDigest);
    jsize intArrayLength = env->GetArrayLength(objArraySign);
    jbyte *byte_array_elements = env->GetByteArrayElements(objArraySign, NULL);
    size_t length = (size_t) intArrayLength * 2 + 1;
    char *char_result = (char *) malloc(length);
    memset(char_result, 0, length);
    //将byte数组转换成16进制字符串
    ByteToHexStr((const char *) byte_array_elements, char_result, intArrayLength);
    *(char_result + intArrayLength * 2) = '\0';// 在末尾补\0

    jstring appSignature = env->NewStringUTF(char_result);
    env->ReleaseByteArrayElements(objArraySign, byte_array_elements, JNI_ABORT);
    free(char_result);
    env->DeleteLocalRef(classMessageDigest);
    env->DeleteLocalRef(objMessageDigest);

    return appSignature;
}



/**提供外部调用的函数**/
jstring  Java_jni_demo_com_jnidemo_jni_JNativeInterface_getJniPackageName(JNIEnv *env, jclass clazz){
    jstring name = getPackageNameInfo(env,clazz);
    return name;
}
jstring Java_jni_demo_com_jnidemo_jni_JNativeInterface_getJniMd5(JNIEnv *env, jclass clazz){
    jstring info = getSignMD5Info(env, clazz);
    return info;
}
/**JNI 注册函数**/
static JNINativeMethod gMethods[] = {
{"getJniPackageName","()Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JNativeInterface_getJniPackageName},
{"getJniMd5","()Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JNativeInterface_getJniMd5},
};
//注册函数的方法，JNI_LOAD
int register_jni_method(JNIEnv *env) {
    return jniRegisterNativeMethods(env, "jni/demo/com/jnidemo/jni/JNativeInterface", gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}