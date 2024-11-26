//
// Created by 86511 on 2024/11/12.
//
#include "graph_analyser.h"

#include <cassert>
#include <fstream>
#include <iostream>

void GraphAnalyser::parseAndIdWord(const std::vector<std::string> &records) {
  // 不断寻找\n
  size_t begin = 0;
  size_t end = 0;
  idToWordList.resize(records.size());
  uint32_t id;
  std::string word;
  uint32_t occurCount;
  for(const std::string& record: records) {
    // 读取接下来三个字段，使用,分割
    begin=0;
    end = record.find(L',',begin);
    id = std::stoi(record.substr(begin,end-begin));
    begin = end + 1;
    end = record.find(L',',begin);
    word = record.substr(begin,end-begin);
    begin = end + 1;
    end = record.find(L',',begin);
    occurCount = std::stoi(record.substr(begin,end-begin));
    wordToIdMap[word] = id;
    idToWordList[id] = {std::move(word),occurCount};
  }
}

void GraphAnalyser::parseGraph(std::ifstream &file) {
  graph.resize(idToWordList.size());
  std::string line;
  uint32_t id1;
  uint32_t directedEdgeCount=0;
  while(true) {
    // 读取一行
    if (!std::getline(file, line)) {
      break;
    }
    id1 = std::stoi(line);
    std::getline(file, line);
    // line是一个形如"100:1,101:2,"的字符串,表示与id1有关的边
    size_t begin = 0;
    size_t end = 0;
    uint32_t a;
    uint32_t count;
    if (line.empty()) {
      continue;
    }
    while (end < line.size()-1) {
      end = line.find(':', begin);
      a = std::stoi(line.substr(begin, end - begin));
      begin = end + 1;
      end = line.find(',', begin);
      count = std::stoi(line.substr(begin, end - begin));
      graph[id1][a] = count;
      graph[a][id1] = count;
      // std::cout << id1 << " " << a << " " << count << '\n';
      begin = end + 1;
      ++directedEdgeCount;
    }
  }
  assert (graph.size() == idToWordList.size());
  std::cout << "directed edge count: " << directedEdgeCount << std::endl;
}

void GraphAnalyser::initGraph(const std::string& nodePath, const std::string& graphPath) {
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
  parseAndIdWord(records);
  // 开始使用edge.csv初始化graph
  file.close();
  file.open(graphPath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << graphPath << std::endl;
    return;
  }
  parseGraph(file);
}

// orderedNum=-1表示不排序， INTMAX表示全部排序， s如果是uint32_t的MAX表示不存在
std::vector<Comp> GraphAnalyser::calc(uint32_t s, int orderedNum) const{
  if (s == unexistId) {
    return {};
  }
  std::unordered_map<uint32_t,double> kscores;
  // 寻找所有与id一起出现的词
  const std::unordered_map<uint32_t, uint32_t>& edge = graph[s];

  for (const auto& [a, sa]: edge) {
    // a即是中介词
    double c = static_cast<double>(sa) / idToWordList[s].occurCount / (idToWordList[a].occurCount - sa);
    // if ( idToWordList[s].occurCount == 0) {
    //   int x =0;
    // }
    // if (idToWordList[a].occurCount == sa) {
    //   int x =0;
    // }
    // 寻找所有与a一起出现的词
    const std::unordered_map<uint32_t, uint32_t>& edge2 = graph[a];
    for (const auto& [k, ka]: edge2) {
      if (k == s) {
        continue;
      }
      kscores[k] += c * ka;
    }
  }
  std::vector<Comp> comps;
  comps.reserve(kscores.size());
  for (const auto& [k, score]: kscores) {
    comps.emplace_back(k, score);
  }
  if (orderedNum == -1)
    return comps;
  if (orderedNum == std::numeric_limits<int>::max() || orderedNum >= static_cast<int>(comps.size())) {
    std::sort(comps.begin(), comps.end());
    return comps;
  }
  std::partial_sort(comps.begin(), comps.begin() + orderedNum, comps.end());
  comps.resize(orderedNum);
  return comps;
}

std::vector<Comp> GraphAnalyser::calc(const std::string& word, int orderedNum) const{
  if (!wordToIdMap.contains(word)) {
    return {};
  }
  return calc(wordToIdMap.at(word), orderedNum);
}

std::vector<std::string> GraphAnalyser::getWords(const std::vector<uint32_t> &ids) const{
  std::vector<std::string> words;
  words.resize(ids.size());
  for (size_t i = 0; i < ids.size(); ++i) {
    words[i] = idToWordList[ids[i]].word;
  }
  return words;
}

std::vector<std::string> GraphAnalyser::getWords(const std::vector<Comp> &comps) const{
  std::vector<std::string> words;
  words.resize(comps.size());
  for (size_t i = 0; i < comps.size(); ++i) {
    words[i] = idToWordList[comps[i].id].word;
  }
  return words;
}

std::vector<uint32_t> GraphAnalyser::getIds(const std::vector<std::string> &words) const{
  std::vector<uint32_t> ids;
  ids.resize(words.size());
  for (size_t i = 0; i < words.size(); ++i) {
    // 如果不存在，id=uint32_t的MAX, 使用find
    auto it = wordToIdMap.find(words[i]);
    if (it == wordToIdMap.end()) {
      ids[i] = unexistId;
    } else {
      ids[i] = it->second;
    }
  }
  return ids;
}
