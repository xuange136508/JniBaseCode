cmake_minimum_required(VERSION 3.4.1) # CMake版本

#增加其他目录的源文件到集合变量中
list(APPEND SRC_LIST

        # src/main/cpp/test.cpp
        # getPackageInfo.cpp
        # getDeviceCode.cpp
        jniStudyDemo.cpp
        onLoad.cpp
    )

add_library(
            jni_spec # 动态库的名称

            SHARED # 作为共享库（动态引入）

            # src/main/cpp/jni_utils.cpp # cpp源文件路径

            # 这里用list变量
             ${SRC_LIST}
           )


find_library(
               log-lib
               log
           )

# 被链接的目标库的名称
target_link_libraries(
                      jni_spec
                      ${log-lib}
                    )
