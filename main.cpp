#include <chrono>
#include <fstream>
#include <random>
#include <thread>
#include <cppjieba/MixSegment.hpp>
#include "graph_analyser.h"
#include "test.h"
#include "test_compkey_calc.h"

void shuffle(std::vector<std::string>& vec) {
  std::random_device rd;                         // 获取硬件生成的种子
  std::default_random_engine rng(rd());          // 使用默认的随机数引擎
  // 使用 std::shuffle 打乱元素
  std::shuffle(vec.begin(), vec.end(), rng);
}

std::vector<std::string> readWord(const std::string& filePath) {
  std::vector<std::string> words;
  std::ifstream infile(filePath);
  if (!infile.is_open()) {
    std::cerr << "open file failed: " << filePath << std::endl;
    return words;
  }
  // 每行是一个词
  std::string line;
  while (std::getline(infile, line)) {
    words.push_back(line);
  }
  infile.close();
  return words;
}


int main() {
  std::string nodePath = "D:/projects/CompKey1/resource/graph_dump_new/node.csv";
  std::string graphPath = "D:/projects/CompKey1/resource/graph_dump_new/graph.txt";

  std::vector<std::string> words = readWord("D:/projects/CompKey1/task/task2/test_words.txt");
  shuffle(words);
  std::cout << "provided words size: " << words.size() << std::endl;
  std::cout << "enabled shuffle" << std::endl;

  auto start = std::chrono::system_clock::now();
  GraphAnalyser graphAnalyser;
  graphAnalyser.initGraph(nodePath, graphPath);
  auto end = std::chrono::system_clock::now();
  std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "init graph time: " << elapsed.count() << "ms" << std::endl;

  CompKeyCalcTest test(words, &graphAnalyser);

  start = std::chrono::system_clock::now();
  test.multiThreadTest(16, 200);
  end = std::chrono::system_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "calculate using multithreading time: " << elapsed.count() << "ms" << std::endl;
  uint32_t valid = test.getNumNotEmpty();
  std::cout << "evaluate keywords num: " << valid << std::endl;


  test.dump("D:/projects/CompKey1/task/task2/sample.txt");
  // testCompKey();
  // testBatchResolve();
  // testGraphAdapter();

  return 0;
}
