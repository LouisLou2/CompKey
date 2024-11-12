//
// Created by leo on 24-10-29.
//
#include "record_batch_resolver.h"
#include <iostream>

void RecordBatchResolver::updateCountMap(const std::vector<std::string>& words) {
  if (words.size() <= 1) {
    return;
  }
  wordIdsARec.resize(words.size());
  for (uint32_t i = 0;i < words.size(); ++i) {
    // 如果能查到
    auto iter = wordToIdMap.find(words[i]);
    if (iter != wordToIdMap.end()) {
      wordIdsARec[i] = iter->second;
      idToCountList[iter->second] += words.size()-1;
    }else {
      // 需要新增
      wordIdsARec[i] = wordToIdMap.size();
      // 增加word->id的映射
      wordToIdMap[words[i]] = wordIdsARec[i];
      // 增加id->content的映射
      idToContentList.push_back(words[i]);
      // 增加id->count的映射
      idToCountList.push_back(words.size()-1);
    }
  }
  // 开始统计联合
  for (size_t i = 0; i < wordIdsARec.size() ; ++i) {
    for (size_t j = i + 1; j < wordIdsARec.size(); ++j) {
      uint64_t idCorKey = (wordIdsARec[i] < wordIdsARec[j])
                            ? static_cast<uint64_t>(wordIdsARec[i]) << 32 | wordIdsARec[j]
                            : static_cast<uint64_t>(wordIdsARec[j]) << 32 | wordIdsARec[i];
      ++CorOccurCount[idCorKey];
    }
  }
}

void RecordBatchResolver::dump() {
  // note first
  // first line
  // id:ID(Keyword),word:String,num:INT,:LABEL
  nodeFile << "wordId:ID,"<<"word:String,num:INT,:LABEL" << '\n';
  for (uint32_t i=0, size = idToContentList.size(); i < size; ++i) {
    nodeFile << i << ',' << idToContentList[i] << ',' << idToCountList[i] << ',' << nodeLabel << '\n';
  }
  nodeFile.close();
  std::cout << "Write " << idToContentList.size() << " nodes." << std::endl;
  // edge
  // first line
  // ord1:START_ID(Keyword),word2:END_ID(Keyword),num:INT,:TYPE
  edgeFile << ":START_ID,:END_ID," << "num:INT,:TYPE" << std::endl;
  for (const auto& [idCorKey, count]: CorOccurCount) {
    edgeFile << (idCorKey >> 32) << ',' << (idCorKey & 0xFFFFFFFF) << ',' << count << ',' << edgeType << std::endl;
  }
  edgeFile.close();
  std::cout << "Write " << CorOccurCount.size() << " edges." << std::endl;
}

RecordBatchResolver::RecordBatchResolver(
  std::string srcFilePath,
  std::string nodeFilePath,
  std::string edgeFilePath,
  std::string nodeLabel,
  std::string edgeType,
  uint8_t maxWordPerRec
):
srcFilePath(std::move(srcFilePath)),
nodeFilePath(std::move(nodeFilePath)),
edgeFilePath(std::move(edgeFilePath)),
nodeLabel(std::move(nodeLabel)),
edgeType(std::move(edgeType)){
  assert(maxWordPerRec <= WordExtractor::maxWordNumPerRecord);
  words.reserve(maxWordPerRec);// 预留100个空间
  wordIdsARec.reserve(maxWordPerRec);
}

bool RecordBatchResolver::openFile() {
  // 打开文件
  srcFile.open(srcFilePath);
  if (!srcFile.is_open()) {
    std::cerr << "Failed to open file: " << srcFilePath << std::endl;
    return false;
  }
  nodeFile.open(nodeFilePath);
  if (!nodeFile.is_open()) {
    std::cerr << "Failed to open file: " << nodeFilePath << std::endl;
    return false;
  }
  edgeFile.open(edgeFilePath);
  if (!edgeFile.is_open()) {
    std::cerr << "Failed to open file: " << edgeFilePath << std::endl;
    return false;
  }
  return true;
}

void RecordBatchResolver::resolve() {
  if (!openFile()) {
    std::cerr<<"cant process due to file open failed"<<std::endl;
    return;
  }
  std::string line;
  std::string record;
  bool invalidLine = false;
  uint32_t lineCount = 0;
  while (std::getline(srcFile, line)) {
    // 拿到line之后，只关心第五个及之后的字段，'\t'分割
    size_t pos = 0;
    size_t count = 0;
    while (count < 4) {
      pos = line.find('\t', pos);
      if (pos == std::string::npos) {
        std::cerr << "Invalid line: " << line << std::endl;
        invalidLine = true;
        break;
      }
      ++pos;
      ++count;
    }
    if (invalidLine) {
      invalidLine = false;
      continue;
    }
    // 现在pos指向第五个字段，他后面还有很多以'\t'分割的字段
    while (pos < line.size()) {
      size_t nextPos = line.find('\t', pos);
      if (nextPos == std::string::npos) {
        record = line.substr(pos);
      }else {
        record = line.substr(pos, nextPos - pos);
      }
      // 提取关键词
      wordExtractor.extractWords(record, words);
      // 统计
      updateCountMap(words);
      if (nextPos == std::string::npos) {
        break;
      }
      pos = nextPos + 1;
    }
    ++lineCount;
    if (lineCount%1000 == 0) {
      std::cout << "Processed " << lineCount << " lines." << "the latest record: "<< record << std::endl;
    }
  }
  std::cout << "Processed " << lineCount << " lines." << "the latest record: "<< record << std::endl;
}
