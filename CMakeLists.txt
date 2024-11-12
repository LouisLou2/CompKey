cmake_minimum_required(VERSION 3.29)
project(compkey)

set(CMAKE_CXX_STANDARD 23)
#cmake_policy(SET CMP0167 NEW)

# 设置源代码目录
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(Boost_ARCHITECTURE "-x64")

#find_package ( uchardet )
# 查找 Boost.Locale 库
set(BOOST_ROOT "D:/dev/boost_1_86_0/")
set(BOOST_LIBRARYDIR "D:/dev/boost_1_86_0/stage/")  # 指定Boost库文件目录
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS locale)

# 递归查找所有的 .cpp 文件
file(GLOB_RECURSE ALL_SOURCE_FILES "${SOURCE_DIR}/*.cpp")
# 遍历所有找到的文件，排除包含 "test" 目录的文件
foreach(FILE_PATH ${ALL_SOURCE_FILES})
    if (FILE_PATH MATCHES "/test/")
        list(REMOVE_ITEM ALL_SOURCE_FILES ${FILE_PATH})
    endif()
endforeach()
# 打印排除后的源文件列表（可选，用于调试）
message("Source files: ${ALL_SOURCE_FILES}")

add_executable(compkey
        main.cpp
        ${ALL_SOURCE_FILES}
        include/comp.h
        include/test_compkey_calc.h
        src/test_compkey_calc.cpp
        test.h
)

target_include_directories(
        compkey PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib_include
        ${Boost_INCLUDE_DIRS}
)

message("Boost include dirs: ${Boost_INCLUDE_DIRS}")
message("cmake source dir: ${CMAKE_SOURCE_DIR}")
message("cmake binary dir: ${CMAKE_BINARY_DIR}")
message("uchardet include dirs: ${uchardet_INCLUDE_DIRS}")

# 链接 uchardet 库
target_link_libraries ( compkey PRIVATE "D:/projects/CompKey/lib/libuchardet-mingw64.a" )
# 链接 Boost.Locale
target_link_libraries(compkey PRIVATE Boost::locale)