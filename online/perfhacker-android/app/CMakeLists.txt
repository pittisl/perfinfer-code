cmake_minimum_required(VERSION 3.10)

file(GLOB model_SRC
    "src/main/cpp/models/*.cpp")

add_library(hacklib SHARED
        src/main/cpp/hacklib.cpp
        src/main/cpp/hacklib_jni.cpp
        src/main/cpp/models.cpp
        ${model_SRC})
include_directories(src/main/cpp/include/)

find_library(log-lib log)
target_link_libraries(hacklib ${log-lib})
add_definitions(-DWITH_ANDROID)
