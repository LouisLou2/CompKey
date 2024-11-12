#include <chrono>
#include <fstream>
#include <thread>
#include <cppjieba/MixSegment.hpp>
#include "graph_analyser.h"
#include "test.h"
#include "test_compkey_calc.h"


int main() {
  std::string nodePath = "D:/projects/CompKey1/resource/graph_dump_new/node.csv";
  std::string graphPath = "D:/projects/CompKey1/resource/graph_dump_new/graph.txt";

  auto now = std::chrono::system_clock::now();
  GraphAnalyser graphAnalyser;
  graphAnalyser.initGraph(nodePath, graphPath);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsedSeconds = end - now;
  std::cout << "init graph time: " << elapsedSeconds.count() << "s" << std::endl;

  std::vector<std::string> words = {
    "小米", "华为", "苹果", "三星","宝马",  "奥迪", "大众","百度",
    "腾讯", "阿里", "京东","润肤霜","洗面奶", "洗发水", "护发素","笔记本",
    "台式机", "手机", "平板","袁姗姗", "杨幂", "刘亦菲", "赵丽颖","周杰伦",
    "成人高考", "中南大学", "湖南大学", "清华大学","北京大学", "复旦大学", "上海交通大学", "浙江大学","华东师范大学",
    "毛尖茶", "铁观音", "碧螺春", "龙井茶","大红袍", "普洱茶",
    "京剧", "越剧", "黄梅戏", "昆曲","豫剧", "川剧", "粤剧", "沪剧",
    "中国工商银行", "中国农业银行", "中国建设银行", "中国银行","交通银行",
    "中国移动", "中国联通", "中国电信", "阅读网","中国邮政","刺客列传", "三国演义", "红楼梦", "西游记","水浒传"
  };
  CompKeyCalcTest test(words, &graphAnalyser);

  now = std::chrono::system_clock::now();
  test.singleThreadTest(100);
  end = std::chrono::system_clock::now();
  elapsedSeconds = end - now;
  std::cout << "calculate using multithreading time: " << elapsedSeconds.count() << "s" << std::endl;
  uint32_t valid = test.getNumNotEmpty();
  std::cout << "evaluate keywords num: " << valid << std::endl;

  test.dump("D:/projects/CompKey1/resource/compkey/sample.txt");

  // testCompKey();
  // testBatchResolve();
  // testGraphAdapter();

  return 0;
}
