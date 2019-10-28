cmake_minimum_required(VERSION 3.4.1) # CMake版本

#增加其他目录的源文件到集合变量中
list(APPEND SRC_LIST
        src/main/cpp/file_operate/file_operate.cpp
        src/main/cpp/jni.cpp
        src/main/cpp/onLoad.cpp

        src/main/cpp/call_so/call_so.cpp
        src/main/cpp/call_static/call_static.cpp
        src/main/cpp/thread/my_thread.cpp
        src/main/cpp/my_stl/my_stl.cpp

        # curl网络请求库
        src/main/cpp/http_curl/my_curl.cpp
        src/main/cpp/http_curl/DownloadModule.cpp
        src/main/cpp/http_curl/HttpPostModule.cpp
        src/main/cpp/http_curl/JNIUtil.cpp



    )

add_library(
            jni_demo # 动态库的名称

            SHARED # 作为共享库（动态引入）

            # 这里用list变量
             ${SRC_LIST}
           )


# 引入本地so
# 步骤1:
#增加so文件动态共享库，${ANDROID_ABI}表示so文件的ABI类型的路径
add_library(ShitAdd SHARED IMPORTED)

# 步骤2:
set_target_properties(ShitAdd  PROPERTIES IMPORTED_LOCATION
     ${CMAKE_SOURCE_DIR}/src/main/cpp/call_so/NetSdklib/libShitAdd.so)

# 步骤3:
#增加include包含的路径
# target_include_directories(ShitAdd PRIVATE src/main/cpp/call_so/include)
# include_directories( src/main/cpp/call_so/include)


# 引入本地静态库
# 步骤1:
add_library(add STATIC IMPORTED)

# 步骤2:
set_target_properties(add  PROPERTIES IMPORTED_LOCATION
     ${CMAKE_SOURCE_DIR}/src/main/cpp/call_static/impl/libadd.a)

# 步骤3:
#增加include包含的路径
# target_include_directories(ShitAdd PRIVATE src/main/cpp/call_static/include)
# include_directories( src/main/cpp/call_static/include)



# 引入libCurl库  -- 如果有不同cup架构 ${ANDROID_ABI}
add_library(curl SHARED IMPORTED)
set_target_properties(curl  PROPERTIES IMPORTED_LOCATION
     ${CMAKE_SOURCE_DIR}/src/main/cpp/http_curl/lib/${ANDROID_ABI}/libcurl.so)




find_library(
               log-lib
               log
           )

# 被链接的目标库的名称
target_link_libraries(
                      jni_demo
                      ShitAdd
                      add
                      # 网络请求
                      curl

                      # 日志
                      ${log-lib}
                    )
