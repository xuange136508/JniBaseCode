#define TAG "fs_jni"
#include "jniRegisiter.h"
#include <string.h>


//日志
#include<android/log.h>
#define LOG_TAG "System.out.c"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

static const char* const kClassPathName = "jni/demo/com/jnidemo/jni/JniTest";
static jint mGlobalObject = NULL;


//cmd中使用命令行:keytool -list -keystore D:\Desktop\app_key


jobject getApplication(JNIEnv *env) {
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID get_Application = env->GetStaticMethodID(activity_thread_clz,
                                                           "currentActivityThread",
                                                           "()Landroid/app/ActivityThread;");
        if (get_Application != NULL) {
            jobject currentActivityThread = env->CallStaticObjectMethod(activity_thread_clz,
                                                                        get_Application);
            jmethodID getal = env->GetMethodID(activity_thread_clz, "getApplication",
                                               "()Landroid/app/Application;");
            application = env->CallObjectMethod(currentActivityThread, getal);
        }
        return application;
    }
    return application;
}

jstring  Java_jni_demo_com_jnidemo_jni_JniTest_getJniString(JNIEnv *env, jclass clazz){
    jobject context = getApplication(env);
    if (context == NULL) {
        LOGI("context is null!");
        return NULL;
    }
    jclass activity = env->GetObjectClass(context);
    jmethodID methodId_pack = env->GetMethodID(activity, "getPackageName", "()Ljava/lang/String;");
    jstring name_str = static_cast<jstring >( env->CallObjectMethod(context, methodId_pack));

    return name_str;
}


//-----------------------------------------------------------------------------------------------

//获取MD5的方法
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


// byte数组转MD5字符串
jstring ToMd5(JNIEnv *env, jbyteArray source) {
    // MessageDigest类
    jclass classMessageDigest = env->FindClass("java/security/MessageDigest");
    // MessageDigest.getInstance()静态方法
    jmethodID midGetInstance = env->GetStaticMethodID(classMessageDigest, "getInstance",
                                                      "(Ljava/lang/String;)Ljava/security/MessageDigest;");
    // MessageDigest object
    jobject objMessageDigest = env->CallStaticObjectMethod(classMessageDigest, midGetInstance,
                                                           env->NewStringUTF("md5"));

    // update方法，这个函数的返回值是void，写V
    jmethodID midUpdate = env->GetMethodID(classMessageDigest, "update", "([B)V");
    env->CallVoidMethod(objMessageDigest, midUpdate, source);

    // digest方法
    jmethodID midDigest = env->GetMethodID(classMessageDigest, "digest", "()[B");
    jbyteArray objArraySign = (jbyteArray) env->CallObjectMethod(objMessageDigest, midDigest);

    jsize intArrayLength = env->GetArrayLength(objArraySign);
    jbyte *byte_array_elements = env->GetByteArrayElements(objArraySign, NULL);
    size_t length = (size_t) intArrayLength * 2 + 1;
    char *char_result = (char *) malloc(length);
    memset(char_result, 0, length);

    // 将byte数组转换成16进制字符串，发现这里不用强转，jbyte和unsigned char应该字节数是一样的
    ByteToHexStr((const char *) byte_array_elements, char_result, intArrayLength);
    // 在末尾补\0
    *(char_result + intArrayLength * 2) = '\0';

    jstring stringResult = env->NewStringUTF(char_result);
    // release
    env->ReleaseByteArrayElements(objArraySign, byte_array_elements, JNI_ABORT);
    // 释放指针使用free
    free(char_result);
    env->DeleteLocalRef(classMessageDigest);
    env->DeleteLocalRef(objMessageDigest);

    return stringResult;
}

//获取应用签名
 jstring loadSignature(JNIEnv *env, jobject context) {
    // 获得Context类
    jclass cls = env->GetObjectClass(context);
    // 得到getPackageManager方法的ID
    jmethodID mid = env->GetMethodID(cls, "getPackageManager",
                                     "()Landroid/content/pm/PackageManager;");

    // 获得应用包的管理器
    jobject pm = env->CallObjectMethod(context, mid);

    // 得到getPackageName方法的ID
    mid = env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
    // 获得当前应用包名
    jstring packageName = (jstring) env->CallObjectMethod(context, mid);

    // 获得PackageManager类
    cls = env->GetObjectClass(pm);
    // 得到getPackageInfo方法的ID
    mid = env->GetMethodID(cls, "getPackageInfo",
                           "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    // 获得应用包的信息
    jobject packageInfo = env->CallObjectMethod(pm, mid, packageName, 0x40); //GET_SIGNATURES = 64;
    // 获得PackageInfo 类
    cls = env->GetObjectClass(packageInfo);
    // 获得签名数组属性的ID
    jfieldID fid = env->GetFieldID(cls, "signatures", "[Landroid/content/pm/Signature;");
    // 得到签名数组
    jobjectArray signatures = (jobjectArray) env->GetObjectField(packageInfo, fid);
    // 得到签名
    jobject signature = env->GetObjectArrayElement(signatures, 0);

    // 获得Signature类
    cls = env->GetObjectClass(signature);
    // 得到toCharsString方法的ID
    mid = env->GetMethodID(cls, "toByteArray", "()[B");
    // 返回当前应用签名信息
    jbyteArray signatureByteArray = (jbyteArray) env->CallObjectMethod(signature, mid);

    return ToMd5(env, signatureByteArray);
}


jstring stoJstring(JNIEnv* env, const char* pat) {
    jclass strClass =  env->FindClass("java/lang/String");
    jmethodID ctorID =  env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes =  env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    jstring encoding =  env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

//检测签名是否匹配
jstring getSignature(JNIEnv *env, jobject context) {

    jstring appSignature = loadSignature(env,context); // 当前 App 的签名
    const char *charAppSignature = env->GetStringUTFChars(appSignature, NULL);

    jstring result = stoJstring(env,charAppSignature);
    env->ReleaseStringUTFChars(appSignature, charAppSignature);
    return result;
}


/**
 * 检查加载该so的应用的签名，与预置的签名是否一致
 */
jstring Java_jni_demo_com_jnidemo_jni_JniTest_getJniMd5(JNIEnv *env, jclass clazz){

    // 调用 getContext 方法得到 Context 对象
    jobject application = NULL;
        jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
        if (activity_thread_clz != NULL) {
            jmethodID currentApplication = env->GetStaticMethodID(
                    activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
            if (currentApplication != NULL) {
                application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
            } else {
                //           LOGE("Cannot find method: currentApplication() in ActivityThread.");
            }
            env->DeleteLocalRef(activity_thread_clz);
        } else {
    //        LOGE("Cannot find class: android.app.ActivityThread");
        }


    jstring signatureValid = getSignature(env, application);
    return signatureValid;
}




/******************************JNI 注册函数************************************/

static JNINativeMethod gMethods[] = {

{"getJniString","()Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JniTest_getJniString},
{"getJniMd5","()Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JniTest_getJniMd5},

};

//注册函数的方法，JNI_LOAD
int register_jni_method(JNIEnv *env) {
    return jniRegisterNativeMethods(env, kClassPathName, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}


