//
// Created by 86511 on 2024/11/12.
//

#ifndef WORD_NODE_H
#define WORD_NODE_H
#include <cstdint>
#include <string>

struct Node {
  std::string word;
  uint32_t occurCount;

  Node()=default;
  Node(std::string&& word, uint32_t occurCount): word(std::move(word)), occurCount(occurCount) {}
  Node(const std::string& word, uint32_t occurCount): word(word), occurCount(occurCount) {}
  Node(Node&& other) noexcept : word(std::move(other.word)), occurCount(other.occurCount) {}
  // 赋值
  Node& operator=(Node&& other) noexcept {
    if (this != &other) {
      word = std::move(other.word);
      occurCount = other.occurCount;
    }
    return *this;
  }
};

#endif //WORD_NODE_H
