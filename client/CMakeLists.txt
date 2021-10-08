cmake_minimum_required(VERSION 3.16)
project(MiniSocketClient C)

set(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_FLAGS -pthread)
# 현재 디렉토리의 모든 소스 파일 찾기
# DIR_SRCS 변수에 이름을 저장
aux_source_directory(. DIR_SRCS)

# include 헤더 폴더 추가 
include_directories(./include)

add_executable(${PROJECT_NAME} ${DIR_SRCS})

# 링크 라이브러리 추가
if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
