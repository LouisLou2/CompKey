//
// Created by leo on 24-10-29.
//

#ifndef RECORD_BATCH_RESOLVER_H
#define RECORD_BATCH_RESOLVER_H
#include <fstream>
#include <string>
#include <word_extractor.h>

#include "small_set.h"

class RecordBatchResolver {
  // 配置项
  const std::string srcFilePath;
  const std::string nodeFilePath;
  const std::string edgeFilePath;
  const std::string nodeLabel;
  const std::string edgeType;

  // 内部
  std::ifstream srcFile;
  std::ofstream nodeFile;
  std::ofstream edgeFile;
  WordExtractor wordExtractor;

  // for dump
  std::unordered_map<std::string,uint32_t> wordToIdMap;
  std::vector<std::string> idToContentList;
  std::vector<uint32_t> idToCountList;
  std::unordered_map<uint64_t, uint32_t> CorOccurCount;
  // for cache
  std::vector<uint32_t> wordIdsARec;
  std::vector<std::string> words;

  bool openFile();
  void updateCountMap(const std::vector<std::string>& words);
public:
  RecordBatchResolver(
    std::string srcFilePath,
    std::string nodeFilePath,
    std::string edgeFilePath,
    std::string nodeLabel = "SWORD",
    std::string edgeType = "CO_OCCUR",
    uint8_t maxWordPerRec = WordExtractor::maxWordNumPerRecord
  );
  void resolve();
  void dump();
};

#endif //RECORD_BATCH_RESOLVER_H
