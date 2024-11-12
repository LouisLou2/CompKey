//
// Created by leo on 24-11-12.
//

#ifndef GRAPH_ADAPTER_H
#define GRAPH_ADAPTER_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "word_node.h"

class GraphAdapter {
  std::vector<Node> idToWordList;
  std::vector<std::unordered_map<uint32_t, uint32_t>> graph;

  void parseAndIdToWordList(const std::vector<std::string>& records);
  void parseEdge(const std::vector<std::string>& records);
public:
  void getGraphUsingNodeEdge(std::string nodePath, std::string edgePath);
  void dumpGraph(std::string targetPath);
};

#endif //GRAPH_ADAPTER_H