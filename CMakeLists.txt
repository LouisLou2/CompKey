cmake_minimum_required(VERSION 3.29)
project(compkey)

set(CMAKE_CXX_STANDARD 23)
# 设置源代码目录
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

find_package ( uchardet )
# 查找 Boost.Locale 库
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
        include/word_extractor.h
        src/word_extractor.cpp
        include/record_resolver.h
        src/record_resover.cpp
)
# 添加 uchardet 的 include 目录
target_include_directories(
    compkey PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib_include
)

# 链接 uchardet 库
target_link_libraries ( compkey PRIVATE ${CMAKE_SOURCE_DIR}/lib/libuchardet.a)
# 链接 Boost.Locale
target_link_libraries(compkey PRIVATE Boost::locale)