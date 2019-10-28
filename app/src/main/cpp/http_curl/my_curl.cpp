// CURL库： http://blog.csdn.net/fengshuiyue/article/details/39530093/
// libcurl是一个跨平台的网络协议库，支持http, https, ftp, gopher, telnet, dict, file, 和ldap 协议。
// libcurl同样支持HTTPS证书授权，HTTP POST, HTTP PUT, FTP 上传, HTTP基本表单上传，代理，cookies,和用户认证。

//常用命令：http://www.cnblogs.com/gbyukg/p/3326825.html

//【本项目地址】：http://blog.csdn.net/csdn49532/article/details/50680716

//jni_Utils来自：http://blog.csdn.net/csdn49532/article/details/50635338

/*
在基于LibCurl的程序里，主要采用callback function （回调函数）的形式完成传输任务，用户在启动传输前设置好各类参数和回调函数，当满足条件时libcurl将调用用户的回调函数实现特定功能。下面是利用libcurl完成传输任务的流程：
    1.       调用curl_global_init()初始化libcurl
    2.       调用curl_easy_init()函数得到 easy interface型指针
    3.       调用curl_easy_setopt()设置传输选项
    4.       根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务
    5.       调用curl_easy_perform()函数完成传输任务
    6.       调用curl_easy_cleanup()释放内存
    在整过过程中设置curl_easy_setopt()参数是最关键的，几乎所有的libcurl程序都要使用它。
 */

#include "my_curl.h"
#include "DownloadModule.h"
#include "HttpPostModule.h"
#include "JNIUtil.h"

jobject g_objAc = NULL;

void Java_jni_demo_com_jnidemo_jni_JniUtils_TestDownload(JNIEnv* env,
                                                         jobject obj)
{
    DownLoadModule download;
    const char* url =
            "http://s3.amazonaws.com/hum9-lwg8-qa2w/PsiphonAndroid.apk";
    int nRet = download.DownLoad(url, "/sdcard/Download", "PsiphonAndroid.apk");
    if (nRet == 0)
        LOGI("download success!");
    else
        LOGE("download error code:%d", nRet);
}
size_t PostDispose(char *buffer, size_t size, size_t nmemb, void *userdata)
{
    JNIUtil util;
    jobject jdata = util.String2Jstring(buffer);
    JNIEnv* env = util.GetJNIEnv();
    jclass cMainActivity = env->GetObjectClass(g_objAc);
    jmethodID methodPostDispose = env->GetMethodID(cMainActivity, "postDispose",
                                                   "(Ljava/lang/String;)V");
    env->CallVoidMethod(g_objAc, methodPostDispose, jdata);
    return nmemb;
}
void Java_jni_demo_com_jnidemo_jni_JniUtils_TestHttpPost(JNIEnv* env,
                                                         jobject obj)
{
    HttpPostModule post;
    post.SetTimeOut(60);
    post.SetHttpHead("Content-Type:application/json;charset=UTF-8");
    post.SetWriteFunction(PostDispose);
    post.SetURL("http://api.map.baidu.com/place/v2/suggestion?query=tiananmen&region=131&output=json&ak=fTF5Wt01MNLs7ci9G9G6X76d");
    int nRet = post.SendPostRequest();
    if (nRet == 0)
        LOGI("post success!");
    else
        LOGE("post error code:%d", nRet);
}
void Java_jni_demo_com_jnidemo_jni_JniUtils_Init(JNIEnv* env, jobject obj)
{
    HttpPostModule::Init();
    JavaVM* vm;
    env->GetJavaVM(&vm);
    JNIUtil::Init(env);
    g_objAc = env->NewGlobalRef(obj);
}
void Java_jni_demo_com_jnidemo_jni_JniUtils_Cleanup(JNIEnv* env, jobject obj)
{
    HttpPostModule::Cleanup();
    env->DeleteGlobalRef(g_objAc);
    g_objAc = NULL;
    JNIUtil::CleanUp(env);
}