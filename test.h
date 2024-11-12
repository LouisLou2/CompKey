//
// Created by 86511 on 2024/11/12.
//

#ifndef TEST_H
#define TEST_H
#include <chrono>
#include <fstream>
#include <record_batch_resolver.h>
#include "graph_adapter.h"
#include "graph_analyser.h"
#include "record_resolver.h"
#include <charset_converter.h>
#include <aho_corasick.hpp>
#include <thread>

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

void testBatchResolve() {
    // WordExtractor ex;
    // std::vector<std::string> words;
    // ex.extractWords("借宁波", words);
    std::string srcFile = "D:/projects/CompKey1/resource/user_tag_query.utf8.10W.TRAIN";
    RecordBatchResolver recordBatchResolver(
        srcFile,
        "D:/projects/CompKey1/resource/neo4j_import_new/node.csv",
        "D:/projects/CompKey1/resource/neo4j_import_new/edge.csv"
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
    /*
    *   Processed 100000 lines.the latest record: nct组合
        resolved in 153488ms
        Write 134292 nodes.
        Write 3892065 edges.
        dumped in 6486ms
     */
}


int countLines(const std::string& filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file!" << std::endl;
    return -1;  // 返回-1表示文件打开失败
  }

  int lineCount = 0;
  std::string line;

  // 逐行读取文件并统计行数
  while (std::getline(file, line)) {
    ++lineCount;
  }

  file.close();
  return lineCount;
}

void testGraphAdapter() {
  std::string nodePath = "D:/projects/CompKey1/resource/neo4j_import_new/node.csv";
  std::string edgePath = "D:/projects/CompKey1/resource/neo4j_import_new/edge.csv";
  GraphAdapter graphAnalyser;
  graphAnalyser.getGraphUsingNodeEdge(nodePath, edgePath);
  graphAnalyser.dumpGraph("D:/projects/CompKey1/resource/graph_dump_new/graph.txt");
}

void testCompKey() {
  std::string nodePath = "D:/projects/CompKey1/resource/graph_dump_new/node.csv";
  std::string graphPath = "D:/projects/CompKey1/resource/graph_dump_new/graph.txt";

  auto now = std::chrono::system_clock::now();
  GraphAnalyser graphAnalyser;
  graphAnalyser.initGraph(nodePath, graphPath);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsedSeconds = end - now;
  std::cout << "init graph time: " << elapsedSeconds.count() << "s" << std::endl;

  std::string id = "小米";
  now = std::chrono::system_clock::now();
  auto comps = graphAnalyser.calc(id, 20);
  end = std::chrono::system_clock::now();
  elapsedSeconds = end - now;
  std::cout << "calc time: " << elapsedSeconds.count() << "s" << std::endl;

  std::cout<< "comp keys:"<<std::endl;
  std::vector<std::string> words = graphAnalyser.getWords(comps);
  int preSize = words.size()>20?20:words.size();
  // print the words 20
  for (int i = 0; i < preSize; ++i) {
    std::cout << words[i] << ' ';
  }
  std::cout << std::endl;

  // 更待用户输入：
  std::string input;
  while (true) {
    std::cout << "Please input a word: ";
    std::cin >> input;
    if (input == "exit") {
      break;
    }
    now = std::chrono::system_clock::now();
    comps = graphAnalyser.calc(input, 20);
    end = std::chrono::system_clock::now();
    elapsedSeconds = end - now;
    std::cout << "calc time: " << elapsedSeconds.count() << "s" << std::endl;

    std::cout<< "comp keys:"<<std::endl;
    words = graphAnalyser.getWords(comps);
    // print the words 20
    preSize = words.size()>20?20:words.size();
    for (int i = 0; i < preSize; ++i) {
      std::cout << words[i] << ' ';
    }
    std::cout << std::endl;
  }
}

#endif //TEST_H
