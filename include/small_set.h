//
// Created by leo on 24-10-29.
//

#ifndef SMALL_SET_H
#define SMALL_SET_H
#include <algorithm>
#include <cstdint>
#include <vector>

template<typename T>
class SmallSet {
  std::vector<T> inner;
public:
  explicit SmallSet(uint8_t maxLen);
  void insert(T&& t) {
    inner.push_back(std::move(t));
  }
  bool contains(const T& t) const {
    return std::find(inner.begin(), inner.end(), t) != inner.end();
  }
  // 这里others就是inner的数据来源，所以可以将move进来的内存重新move回other,一定注意inner.size() <= other.size()
  void moveData(std::vector<T> &other) {
    // assert(inner.size() <= other.size());
    for (uint32_t i = 0; i < inner.size(); ++i) {
      other[i] = std::move(inner[i]);
    }
    other.resize(inner.size());
    inner.clear();
  }
};

template<typename T>
SmallSet<T>::SmallSet(uint8_t maxLen) {
  inner.reserve(maxLen);
}

#endif //SMALL_SET_H
