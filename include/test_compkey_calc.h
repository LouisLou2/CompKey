//
// Created by 86511 on 2024/11/12.
//

#ifndef TEST_COMPKEY_CALC_H
#define TEST_COMPKEY_CALC_H
#include <cassert>
#include <graph_analyser.h>
#include <optional>
#include <string>
#include <vector>

class CompKeyCalcTest {
  std::vector<std::string> words;
  std::vector<uint32_t> wordIds;
  const GraphAnalyser* analyser;
  std::vector<std::vector<Comp>> compsList;
public:
  explicit CompKeyCalcTest(const std::vector<std::string>& words, const GraphAnalyser* analyser): words(words), analyser(analyser) {
    assert (analyser != nullptr);
    wordIds = analyser->getIds(words);
  }
  explicit CompKeyCalcTest(std::vector<std::string>&& words, const GraphAnalyser* analyser): words(words), analyser(analyser) {
    assert (analyser != nullptr);
    wordIds = analyser->getIds(words);
  }
  uint32_t getNumNotEmpty() const;
  void dump(const std::string& dumpPath) const;
  void singleThreadTest(int orderedNum);
  void multiThreadTest(uint16_t threadNum, int orderedNum);
};

#endif //TEST_COMPKEY_CALC_H
