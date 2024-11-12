//
// Created by leo on 24-11-12.
//
#include "graph_adapter.h"

#include <cassert>
#include <fstream>
#include <iostream>

void GraphAdapter::parseAndIdToWordList(const std::vector<std::string>& records) {
  // 不断寻找\n
  size_t begin = 0;
  size_t end = 0;
  idToWordList.resize(records.size());
  for(const std::string& record: records) {
    // 读取接下来三个字段，使用,分割
    uint32_t id;
    std::string word;
    uint32_t occurCount;
    begin=0;
    end = record.find(L',',begin);
    id = std::stoi(record.substr(begin,end-begin));
    begin = end + 1;
    end = record.find(L',',begin);
    word = record.substr(begin,end-begin);
    begin = end + 1;
    end = record.find(L',',begin);
    occurCount = std::stoi(record.substr(begin,end-begin));
    idToWordList[id] = {word,occurCount};
  }
}

void GraphAdapter::parseEdge(const std::vector<std::string>& records) {
  graph.resize(idToWordList.size());
  for (const std::string& record: records) {
    // 读取接下来三个字段，使用,分割
    uint32_t id1;
    uint32_t id2;
    uint32_t count;
    size_t begin = 0;
    size_t end = record.find(L',',begin);
    id1 = std::stoi(record.substr(begin,end-begin));
    begin = end + 1;
    end = record.find(L',',begin);
    id2 = std::stoi(record.substr(begin,end-begin));
    begin = end + 1;
    end = record.find(L',',begin);
    count = std::stoi(record.substr(begin,end-begin));
    graph[id1][id2] = count;
    graph[id2][id1] = count;
  }
  assert (graph.size() == idToWordList.size());
}

void GraphAdapter::getGraphUsingNodeEdge(std::string nodePath, std::string edgePath) {
  // std::locale locale("C");
  std::ifstream file(nodePath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << nodePath << std::endl;
    return;
  }
  // file.imbue(locale);
  std::vector<std::string> records;
  records.reserve(140000);
  std::string line;
  // 第一行是表头
  std::getline(file, line);
  while (std::getline(file, line)) {
    records.push_back(line);
  }
  // 初始化wordToIdMap和idToWordList
  parseAndIdToWordList(records);
  // 开始使用edge.csv初始化graph
  file.close();
  file.open(edgePath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << edgePath << std::endl;
    return;
  }
  // file.imbue(locale);
  records.clear();
  records.reserve(2400000);
  // 第一行是表头
  std::getline(file, line);
  while (std::getline(file, line)) {
    records.push_back(line);
  }
  // 初始化graph
  parseEdge(records);
}

void GraphAdapter::dumpGraph(std::string targetPath) {
  std::ofstream file(targetPath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << targetPath << std::endl;
    return;
  }
  // 范围for循环遍历graph
  for (uint32_t i = 0; i < graph.size(); ++i) {
    file<<i<<'\n';
    for (const auto& [id,count]: graph[i]) {
      file<<id<<':'<<count<<',';
    }
    file<<'\n';
  }
  file.close();
}
