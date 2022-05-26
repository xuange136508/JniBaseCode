#include "jniRegisiter.h"
#include <string.h>
//#include <stdlib.h>
//#include <jni.h>

#include <stdarg.h>
#include <stdio.h>

//日志
#include<android/log.h>
#define LOG_TAG "System.out.c"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

jstring stoJstring(JNIEnv* env, const char* pat) {
    jclass strClass =  env->FindClass("java/lang/String");
    jmethodID ctorID =  env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes =  env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    jstring encoding =  env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

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
 * 获取设备唯一标识
 * */
jstring getDeviceCode(JNIEnv *env, jclass clazz){
    jobject instance = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            instance = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        }
        env->DeleteLocalRef(activity_thread_clz);
    }
    // 反射获取ContentResolver对象
    jclass activity = env->GetObjectClass(instance);
    jmethodID method = env->GetMethodID(activity, "getContentResolver","()Landroid/content/ContentResolver;");
    jobject resolverInstance = env->CallObjectMethod(instance, method);
    // 通过Settings$Secure获取android_id
    jclass androidSettingsClass = env->FindClass("android/provider/Settings$Secure");
    jmethodID methodId = env->GetStaticMethodID(androidSettingsClass, "getString","(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");
    jstring param_android_id = env->NewStringUTF("android_id");
    jstring android_id = (jstring) env->CallStaticObjectMethod(androidSettingsClass, methodId, resolverInstance,param_android_id);

    //*添加释放的代码
    env->DeleteLocalRef(instance);
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(resolverInstance);
    return android_id;
}


jstring  getPackageNameInfo(JNIEnv *env, jclass clazz){
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

    //*添加释放的代码
    env->DeleteLocalRef(application);
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(activity_thread_clz);
    return name_str;
}

jint checkPermission(JNIEnv *env, jclass clazz){
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        }
        env->DeleteLocalRef(activity_thread_clz);
    }
    //获取PackageManager对象
    jclass cls = env->GetObjectClass(application);
    jmethodID mid = env->GetMethodID(cls, "getPackageManager","()Landroid/content/pm/PackageManager;");
    jobject pm = env->CallObjectMethod(application, mid);
    //检查权限
    jclass packageManagerClass = env->GetObjectClass(pm);
    jmethodID packageManagerMid = env->GetMethodID(packageManagerClass, "checkPermission","(Ljava/lang/String;Ljava/lang/String;)I");
    const char* permName = "android.permission.READ_PHONE_STATE";
    jstring packageName = getPackageNameInfo(env,clazz);//"com.jni.base"
    const char* pkgName = jstringTostring(env,packageName);
    LOGI("包名为 :%s",pkgName);
    jint status = env->CallIntMethod(pm, packageManagerMid,stoJstring(env,permName),stoJstring(env,pkgName));
    LOGI("检查权限结果码为 :%d",(int)status);

    //*添加释放的代码
    env->DeleteLocalRef(application);
    env->DeleteLocalRef(cls);
    env->DeleteLocalRef(pm);
    env->DeleteLocalRef(packageManagerClass);
    return status;
}

jstring getIMEI(JNIEnv *env, jclass clazz){
    //先检查权限，不然安全异常闪退
    jint code = checkPermission(env,clazz);
    if(code == 0){
        try{
            jobject instance = NULL;
            jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
            if (activity_thread_clz != NULL) {
                jmethodID currentApplication = env->GetStaticMethodID(activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
                if (currentApplication != NULL) {
                    instance = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
                }
                env->DeleteLocalRef(activity_thread_clz);
            }
            jclass cls_context = env->FindClass("android/content/Context");
            jmethodID getSystemService = env->GetMethodID(cls_context, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
            jfieldID TELEPHONY_SERVICE = env->GetStaticFieldID(cls_context, "TELEPHONY_SERVICE", "Ljava/lang/String;");
            jstring str = (jstring)env->GetStaticObjectField(cls_context, TELEPHONY_SERVICE);

            jobject telephonyManager = env->CallObjectMethod(instance, getSystemService, str);
            jclass cls_tm = env->FindClass("android/telephony/TelephonyManager");
            jmethodID getDeviceId = env->GetMethodID(cls_tm, "getDeviceId", "()Ljava/lang/String;");
            jstring deviceId = (jstring)env->CallObjectMethod(telephonyManager, getDeviceId);

            //*添加释放的代码
            env->DeleteLocalRef(instance);
            env->DeleteLocalRef(cls_context);
            env->DeleteLocalRef(telephonyManager);
            env->DeleteLocalRef(cls_tm);
            return deviceId;
        }catch(char *str){
            //cout << str << endl;
            //return stoJstring(env,str);
            return env->NewStringUTF("0");
        }
    }else{
        return env->NewStringUTF("0");
    }
}

jstring getMacAddress(JNIEnv *env, jclass clazz){
    jclass cls_networkInterface = env->FindClass("java/net/NetworkInterface");
    if (cls_networkInterface == 0) {
        return env->NewStringUTF("0");
    }
    //找到getByName方法
    jmethodID jmethodID1 = env->GetStaticMethodID(cls_networkInterface, "getByName", "(Ljava/lang/String;)Ljava/net/NetworkInterface;");
    if (jmethodID1 == 0) {
        return env->NewStringUTF("0");
    }
    jstring jss2 = env->NewStringUTF("wlan0");
    //调用getByname方法返回NetworkInterface的实例
    jobject jobject1 = env->CallStaticObjectMethod(cls_networkInterface, jmethodID1, jss2);
    //找到getHardAddress方法
    jmethodID getHardwareAddress = env->GetMethodID(cls_networkInterface, "getHardwareAddress", "()[B");
    if (getHardwareAddress == 0)
        return env->NewStringUTF("");
    //调用getHardAddress方法获取MAC地址的byte[]数组
    jbyteArray jbyte1 = (jbyteArray)env->CallObjectMethod(jobject1, getHardwareAddress);
    //下面一些列流程就是讲byte[]数组转换成char类型字符在转换成字符串
    jsize  oldsize = env->GetArrayLength(jbyte1);
    int len = (int)oldsize;
    char* data = (char*)env->GetByteArrayElements(jbyte1, 0);
    char *temp = new char[len*2 + 1];
    memset(temp,0,len*2 +1);
    for (int i = 0; i < len; i++) {
        char * buffer = new char[2];
        memset(buffer, '0', 2);
        sprintf(buffer, "%02X", data[i]);
        memcpy(temp+i*2, buffer, 2);
        delete[] (buffer);
    }
    jstring jMac = stoJstring(env, temp);
    delete[] temp;

    //*添加释放的代码
    env->DeleteLocalRef(cls_networkInterface);
    env->DeleteLocalRef(jobject1);
    return jMac;
}


jstring reflexDeviceInfo(JNIEnv *env,const char* params){
    jclass androidBuildClass = env->FindClass("android/os/Build");
    jfieldID fieldInfo = env->GetStaticFieldID(androidBuildClass, params, "Ljava/lang/String;");
    return  (jstring) env->GetStaticObjectField(androidBuildClass, fieldInfo);
}


jstring getDeviceUUID(JNIEnv *env, jclass clazz,int count, ...){
    va_list args;
    va_start(args, count);

    char totalStr[50];
    strcpy(totalStr,  "35");
    for (int i = 0; i < count; ++i){
        const char*  arg = va_arg(args,char*);
        LOGI("arg is :%s",arg);
        jstring mRes = reflexDeviceInfo(env,arg);
        char* res = jstringTostring(env,mRes);
        LOGI("result :%s",res);
        //整型转换为char*,用于拼接字符串
        int strLength = strlen(res)%10;
        char *cast = static_cast<char *>(malloc(sizeof(char *)));
        sprintf(cast,"%d",strLength);
        /* C++写法
        stringstream ss;
        ss << i;
        const char *cString = ss.str( ).c_str( );
         */
        strcat(totalStr,cast);
    }
    LOGI("total res is :%s",totalStr);
    va_end(args);

    jstring uuidCode = stoJstring(env,totalStr);
    return uuidCode;
}

jstring getDeviceSerial(JNIEnv *env, jclass clazz){
    jstring mSERIAL = reflexDeviceInfo(env,"SERIAL");
    LOGI("SERIAL is :%s",jstringTostring(env,mSERIAL));

    return mSERIAL;
}

jstring getDeviceBuildInfo(JNIEnv *env, jclass clazz){
    /*
   String serial = null;
   String mDevId = "35" +
                   Build.BOARD.length()%10+ Build.BRAND.length()%10 +   // 主板 系统定制商
                   Build.CPU_ABI.length()%10 + Build.DEVICE.length()%10 +  //CPU指令集 设备参数
                   Build.DISPLAY.length()%10 + Build.HOST.length()%10 +  //显示设备号
                   Build.ID.length()%10 + Build.MANUFACTURER.length()%10 +  //设备硬件id  厂商
                   Build.MODEL.length()%10 + Build.PRODUCT.length()%10 +  //设备型号 产品名
                   Build.TAGS.length()%10 + Build.TYPE.length()%10 +  //设备标签  设备版本类型
                   Build.USER.length()%10 ;*/

    //变长参数去获取uuid
    jstring deviceUUID = getDeviceUUID(env,clazz,13,"BOARD","BRAND","CPU_ABI","DEVICE","DISPLAY","HOST","ID","MANUFACTURER",
                                       "MODEL","PRODUCT","TAGS","TYPE","USER");

    return deviceUUID;
}


/**获取唯一标识**/
jstring getDeviceUniqueId(JNIEnv *env, jclass clazz){
    //获取 IMEI 需要权限，不然报安全异常
    char* imei = jstringTostring(env,getIMEI(env, clazz));LOGI("IMEI: %s",imei);
    char* deviceId = jstringTostring(env,getDeviceCode(env, clazz));LOGI("DEVICEID: %s",deviceId);
    char* macAddress = jstringTostring(env,getMacAddress(env, clazz));LOGI("MAC: %s",macAddress);
    char* serial = jstringTostring(env,getDeviceSerial(env,clazz));LOGI("SERIAL: %s",serial);
    char* buildInfo = jstringTostring(env,getDeviceBuildInfo(env,clazz));LOGI("BUILDINFO: %s",buildInfo);
    //拼接
    //if(strlen(deviceId)!=0)
    char spellStr[200];
    strcpy(spellStr, imei);
    strcat(spellStr, deviceId);
    strcat(spellStr, macAddress);
    strcat(spellStr, serial);
    strcat(spellStr, buildInfo);


    LOGI("拼接后唯一标识为: %s",spellStr);
    //char*转jbyteArray
    size_t len = strlen(reinterpret_cast<const char *>(spellStr));
    jbyteArray uuidByteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(uuidByteArray, 0, len, reinterpret_cast<jbyte *>(spellStr));
    //MD5算法
    jclass classMessageDigest = env->FindClass("java/security/MessageDigest");
    jmethodID midGetInstance = env->GetStaticMethodID(classMessageDigest, "getInstance","(Ljava/lang/String;)Ljava/security/MessageDigest;");
    jobject objMessageDigest = env->CallStaticObjectMethod(classMessageDigest, midGetInstance,env->NewStringUTF("md5"));
    jmethodID midUpdate = env->GetMethodID(classMessageDigest, "update", "([B)V");
    env->CallVoidMethod(objMessageDigest, midUpdate, uuidByteArray);
    jmethodID midDigest = env->GetMethodID(classMessageDigest, "digest", "()[B");
    jbyteArray objArray = (jbyteArray) env->CallObjectMethod(objMessageDigest, midDigest);
    //初始化字符串
    jsize intArrayLength = env->GetArrayLength(objArray);
    jbyte *byte_array_elements = env->GetByteArrayElements(objArray, NULL);
    size_t length = (size_t) intArrayLength * 2 + 1;
    char *char_result = (char *) malloc(length);
    memset(char_result, 0, length);
    //将byte数组转换成16进制字符串
    ByteToHexStr((const char *) byte_array_elements, char_result, intArrayLength);
    *(char_result + intArrayLength * 2) = '\0';// 在末尾补\0

    LOGI("MD5后唯一标识为: %s",char_result);
    jstring uuidCode = env->NewStringUTF(char_result);

    //释放资源
    env->ReleaseByteArrayElements(objArray, byte_array_elements, JNI_ABORT);
    free(char_result);
    env->DeleteLocalRef(classMessageDigest);
    env->DeleteLocalRef(objMessageDigest);
    return uuidCode;
}



/**提供外部调用的函数**/
jstring Java_jni_demo_com_jnidemo_jni_JNativeInterface_getDeviceCode(JNIEnv *env, jclass clazz){
    jstring info = getDeviceUniqueId(env,clazz);
    return info;
}

/**JNI 注册函数**/
static JNINativeMethod gMethods[] = {
{"getDeviceCode","()Ljava/lang/String;", (void *)Java_jni_demo_com_jnidemo_jni_JNativeInterface_getDeviceCode},
};
//注册函数的方法，JNI_LOAD
int register_jni_method(JNIEnv *env) {
    return jniRegisterNativeMethods(env, "jni/demo/com/jnidemo/jni/JNativeInterface", gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}