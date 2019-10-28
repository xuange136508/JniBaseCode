// 文件读写 http://www.cnblogs.com/jianyungsun/archive/2011/02/19/1958587.html

// c/c++文件操作区别 http://blog.csdn.net/u010485442/article/details/46908427

/*
 * 读写文件测试：
 * iostream 库 耗时 230-300 之间
    FIFE  差不多也是耗时260 左右
    结论：效率差不多，一个访问对象成员变量，一个通过指针
    c++部分代码可能直接用汇编写，效率比c慢5%据说
    c++比较直观，面向对象的思想好理解，开发快
 * */
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <jni.h>
#include "file_operate.h"

//日志
#include<android/log.h>
#define LOG_TAG "System.out.c"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int file_open(const char *filename, int flags) {
    int fd;

    fd = open(filename, flags, 0666);
    if (fd == -1)
        return -1;

    return fd;
}

int file_read(int fd, unsigned char *buf, int size) {

    return read(fd, buf, size);
}

int file_write(int fd, const unsigned char *buf, int size) {

    return write(fd, buf, size);
}


int64_t file_seek(int fd, int64_t pos, int whence) {

    if (whence == 0x10000) {
        struct stat st;
        int ret = fstat(fd, &st);
        return ret < 0 ? -1 : st.st_size;
    }
    return lseek(fd, pos, whence);
}

int file_close(int fd) {

    return close(fd);
}



jint Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileOpen( JNIEnv* env, jobject thiz,jstring filename,jint flags){
    LOGI("文件操作 Open ");
    const char *filename_char = env->GetStringUTFChars(filename, NULL);
    return file_open(filename_char, flags);
}

jint Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileRead(JNIEnv* env, jobject thiz,int fd,jbyteArray buf,jint size){
    LOGI("文件操作 Read ");
    unsigned char *buf_char = (unsigned char*)(env->GetByteArrayElements(buf, NULL));
    return file_read(fd, buf_char,  size);
}

jint Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileWrite(JNIEnv* env, jobject thiz,int fd,jbyteArray buf,jint size){
    LOGI("文件操作 Write ");
    unsigned char *buf_char = (unsigned char*)(env->GetByteArrayElements(buf, NULL));
    return file_write(fd, buf_char,  size);
}

jlong Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileSeek(JNIEnv* env, jobject thiz,int fd,jlong Offset,jint whence){
    LOGI("文件操作 Seek ");
    return file_seek(fd, Offset,  whence);
}

jint Java_jni_demo_com_jnidemo_jni_JniUtils_NativeFileClose(JNIEnv* env, jobject thiz,int fd){
    LOGI("文件操作 Close ");
    return file_close(fd);
}