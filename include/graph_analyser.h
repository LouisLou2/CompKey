//
// Created by 86511 on 2024/11/12.
//

#ifndef GRAPH_ANALYSER_H
#define GRAPH_ANALYSER_H
#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "comp.h"
#include "word_node.h"

class GraphAnalyser {
  static constexpr uint32_t unexistId = std::numeric_limits<uint32_t>::max();
  std::vector<Node> idToWordList;
  std::unordered_map<std::string, uint32_t> wordToIdMap;
  std::vector<std::unordered_map<uint32_t, uint32_t>> graph;

  void parseAndIdWord(const std::vector<std::string>& records);
  void parseGraph(std::ifstream& file);
public:
  void initGraph(const std::string& nodePath, const std::string& graphPath);
  std::vector<Comp> calc(uint32_t id, int orderedNum) const;
  std::vector<Comp> calc(const std::string& word, int orderedNum)const;
  std::vector<std::string> getWords(const std::vector<uint32_t>& ids) const;
  std::vector<std::string> getWords(const std::vector<Comp>& comps) const;

  // 得到id, id=uint32_t的MAX表示不存在
  std::vector<uint32_t> getIds(const std::vector<std::string>& words) const;
};

#endif //GRAPH_ANALYSER_H
