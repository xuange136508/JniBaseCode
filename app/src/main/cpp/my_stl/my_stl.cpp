//http://blog.csdn.net/conanswp/article/details/23297441
#include "my_stl.h"
#include <iostream>
//日志
#include<android/log.h>
#define LOG_TAG "System.out.c"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

//目录
#include <dirent.h>

#include <vector>
using namespace std;

void ListDir(char* pPath, vector<string>& fileList);

void Java_jni_demo_com_jnidemo_jni_JniUtils_startSTL(JNIEnv* env,jobject obj) {
    LOGI("STL start!!");
    //Int类型
    vector<int>vi;
    for (int i = 0; i < 3; ++i) {
        vi.push_back(i);
    }
    //循环输出
    for (int i = 0; i < 3; ++i) {
        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"print %d",vi[i]);
    }
    //迭代器迭代
    vector<int>::iterator iter;
    for(iter=vi.begin();iter!=vi.end();++iter){
        int num = *iter;
        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"iterator %d",num);
    }

    //String类型
    vector<string> contents;
    contents.push_back(string("vector"));
    const char* str = contents[0].c_str();
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"print %s",str);


}


//遍历
//C++代码遍历java传入的一个文件路径中的所有文件，并返回文件数目。
void Java_jni_demo_com_jnidemo_jni_JniUtils_getPathWithSTL(JNIEnv* env,jobject obj,jstring str) {
    LOGI("STL getSdPath!!");

    char szDir[128];
    const char *p = env->GetStringUTFChars(str, NULL);
    if (NULL == p){
        return;
    }
    //数组复制，p复制到szDir，szDir空间要足够大
    strcpy(szDir, p);
    env->ReleaseStringUTFChars(str, p);
    printf("String from java : %s", szDir);
    vector<string> files;
    ListDir(szDir, files);

    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"文件数目为 %d",files.size());

}
void ListDir(char* pPath, vector<string>& fileList) {
    //获取目录对象
    DIR *pDir = opendir(pPath);
    if (NULL == pDir)
        return ;
    struct dirent *ent;
    char szPath[256], szFile[256];
    while ((ent = readdir(pDir)) != NULL) {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
            continue;
        sprintf(szPath, "%s/%s", pPath, ent->d_name);
        if (ent->d_type & DT_DIR) {
            //递归调用
            ListDir(szPath, fileList);
        }
        else {
            //文件
            fileList.push_back(szPath);
        }
    }
    //释放目录对象
    closedir(pDir);
}