#include <charset_converter.h>
#include <fstream>
#include <aho_corasick.hpp>
#include <chrono>
#include <record_batch_resolver.h>
#include <cppjieba/MixSegment.hpp>

#include "record_resolver.h"

void testOfEncodingDetect() {
    std::string file_path = "/home/leo/CLionProjects/compkey/resource/user_tag_query.10W.TRAIN";
    std::string target_file_path = "/home/leo/CLionProjects/compkey/resource/user_tag_query.utf8.10W.TRAIN";
    CharsetConverter::convertToUtf8(file_path, target_file_path);
}

void testOnlyExtractMainWords() {
    auto now = std::chrono::high_resolution_clock::now();
    std::vector<std::wstring> keywords = {
        L"动物", L"植物", L"水果", L"蔬菜", L"葡萄酒", L"华为", L"苹果", L"鼠标", L"电脑",
        L"手机", L"平板", L"电视", L"音响", L"耳机", L"汽车", L"冰箱", L"空调", L"书籍",
        L"玩具", L"服装", L"鞋子"
    };
    std::string precessedDataPrefix = "/home/leo/CLionProjects/compkey/resource/word/";
    RecordResolver recordResolver(
        keywords,
        "/home/leo/CLionProjects/compkey/resource/user_tag_query.utf8.10W.TRAIN",
        [&precessedDataPrefix](const std::wstring &s, uint32_t index) {
            return precessedDataPrefix + '[' + std::to_string(index) + ']' + CharsetConverter::wstrToStr(s) + ".word_1";
        },
        true,
        true,
        true // 仅仅当你的关键词之间关联较大时才使用，要不然缓存不能发挥优势，并且占用大量内存
    );
    recordResolver.resolveLessIO();
    auto end = std::chrono::high_resolution_clock::now();
    // milli
    std::cout << "generated " << keywords.size() << " files in ";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count() << "ms" << std::endl;
}

int main() {
    // WordExtractor ex;
    // std::vector<std::string> words;
    // ex.extractWords("借宁波", words);
    std::string srcFile = "/home/leo/CLionProjects/compkey/resource/user_tag_query.utf8.10W.TRAIN";
    RecordBatchResolver recordBatchResolver(
        srcFile,
        "/home/leo/CLionProjects/compkey/resource/stat_new/node.csv",
        "/home/leo/CLionProjects/compkey/resource/stat_new/edge.csv"
    );
    auto now = std::chrono::high_resolution_clock::now();
    recordBatchResolver.resolve();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "resolved in ";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count() << "ms" << std::endl;
    now = std::chrono::high_resolution_clock::now();
    recordBatchResolver.dump();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "dumped in ";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count() << "ms" << std::endl;
    return 0;
    /*
    *   Processed 100000 lines.the latest record: nct组合
        resolved in 221601ms
        Write 162485 nodes.
        Write 7328068 edges.
        dumped in 16749ms
     */
}


// #include <iostream>
// #include "lac/lac.h"
// int main() {
//   // 选择不同的模型进行装载
// auto lac = LAC( "./lac_model");
// // lac = LAC(model_path = "./seg_model")
//
// // 可选，装载干预词典
// lac.load_customization("custom.txt");
//
// // 执行并返回结果
// auto lac_res = lac.run("百度是一家高科技公司");
//
// // 打印结果
// for (int i=0; i<lac_res.size(); i++)
//   std::cout<<lac_res[i].word<<"\001"<<lac_res[i].tag<<" ";
// return 0;
// }