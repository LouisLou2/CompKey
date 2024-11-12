//
// Created by 86511 on 2024/11/12.
//

#ifndef COMP_H
#define COMP_H
#include <cstdint>

struct Comp {
  uint32_t id;
  double score;

  Comp()=default;
  Comp(uint32_t id, double score): id(id), score(score) {}
  //自定义排序规则：仅仅使用score进行排序
  bool operator < (const Comp& other) const {
    return score > other.score;
  }
};

#endif //COMP_H
