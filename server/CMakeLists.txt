cmake_minimum_required(VERSION 3.16)
project(MiniSocketServer C)

set(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_FLAGS -pthread)
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 添加 include 头文件夹
include_directories(./include)

add_executable(${PROJECT_NAME} ${DIR_SRCS})

#添加链接库
if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()