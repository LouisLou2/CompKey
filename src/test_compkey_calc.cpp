//
// Created by 86511 on 2024/11/12.
//
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <test_compkey_calc.h>
#include <thread>

uint32_t CompKeyCalcTest::getNumNotEmpty() const {
  uint32_t num = 0;
  for (const auto& comps: compsList) {
    if (!comps.empty()) {
      ++num;
    }
  }
  return num;
}

void CompKeyCalcTest::dump(const std::string &dumpPath) const {
  std::ofstream file(dumpPath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << dumpPath << std::endl;
    return;
  }
  // 设置输出流格式：固定小数点格式并保留四位小数
  file << std::fixed << std::setprecision(4);
  for (int i=0;i<words.size();++i) {
    const std::vector<Comp>& comps=compsList[i];
    std::vector<std::string> candidateWords = analyser->getWords(comps);
    file<<'['<<wordIds[i]<<']'<<words[i]<<":\n";
    for (int j=0;j<comps.size();++j) {
      file<<'['<<comps[i].score<<']'<<'['<<comps[j].id<<']'<<candidateWords[j]<<' ';
    }
    file<<'\n';
  }
  file.close();
}

void CompKeyCalcTest::singleThreadTest(int orderedNum){
  compsList.resize(words.size());
  for (size_t i = 0; i < words.size(); ++i) {
    compsList[i] = analyser->calc(wordIds[i], orderedNum);
  }
}

void CompKeyCalcTest::multiThreadTest(uint16_t threadNum, int orderedNum){
  compsList.resize(words.size());
  std::vector<std::thread> threads;
  threadNum = std::min(static_cast<size_t>(threadNum), words.size());
  std::cout << "threadNum: " << threadNum << std::endl;
  threads.reserve(threadNum);
  // 将任务平均分配给各个线程
  size_t begin = 0;
  size_t end = 0;
  size_t step = words.size() / threadNum;
  for (int i = 0; i < threadNum; ++i) {
    end = begin + step;
    if (i == threadNum - 1) {
      end = words.size();
    }
    threads.emplace_back([this, begin, end, orderedNum](){
      for (size_t i = begin; i < end; ++i) {
        this->compsList[i] = analyser->calc(wordIds[i], orderedNum);
      }
    });
    begin = end;
  }
  for (auto& thread: threads) {
    thread.join();
  }
}
