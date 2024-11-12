//
// Created by leo on 24-10-28.
//
#include <charset_converter.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "record_resolver.h"

const std::locale RecordResolver::defaultLocale("zh_CN.UTF-8");

RecordResolver::RecordResolver(
   const std::vector<std::wstring>& keywords,
   std::string  srcFilePath,
   std::function<std::string(const std::wstring&, uint32_t index)> fileNameGenerator,
   bool uniqueWordInARecord,
   bool rmSemanticallyEmpty,
   bool enableCache,
   std::optional<std::set<char>> symbols
) :
   keywords(keywords),
   srcFilePath(std::move(srcFilePath)),
   fileNameGenerator(std::move(fileNameGenerator)),
   uniqueWordInARecord(uniqueWordInARecord),
   rmSemanticallyEmpty(rmSemanticallyEmpty),
   enableCache(enableCache),
   wordExtractor(WordExtractor::maxWordNumPerRecord,uniqueWordInARecord, rmSemanticallyEmpty, symbols ? std::optional(std::move(symbols.value())): std::nullopt) {
  if (!checkKeywords(keywords)) {
    throw std::invalid_argument("Invalid keywords.");
  }
  // 子串匹配项
  for (const auto& keyword : keywords) {
    wtrie.insert(keyword);
  }
  // 初始化
  listForKeyWords.resize(keywords.size());
}

std::pair<size_t, size_t> RecordResolver::recordRange(
  const std::wstring& allContent, size_t wordBegin, size_t wordEnd
) {
  // 边界是tab或换行
  size_t begin = wordBegin - 1;
  size_t end = wordEnd + 1;
  while (begin > 0 && allContent[begin] != L'\t' && allContent[begin] != L'\n') {
    --begin;
  }
  ++begin;
  while (end < allContent.size() && allContent[end] != L'\t' && allContent[end] != L'\n') {
    ++end;
  }
  return {begin, end};
}


void RecordResolver::writeResult(const std::string& fileName, const recordWordsList& listForKeyWords) {
  std::ofstream file(fileName);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << fileName << std::endl;
    return;
  }
  for (const auto& recordWords: listForKeyWords) {
    for (const auto& words: recordWords) {
      // 以string形式写入
      file << words << defaultSeparator;
    }
    file<<'\n';
  }
}

bool RecordResolver::checkKeywords(const std::vector<std::wstring>& keywords) {
  if (keywords.empty()) {
    std::cerr << "Keywords cannot be empty." << std::endl;
    return false;
  }
  for (const auto& keyword : keywords) {
    if (keyword.empty()) {
      std::cerr << "Keyword cannot be empty." << std::endl;
      return false;
    }
    // 不能含有分割符\t\r\n
    for (const auto& ch : keyword) {
      if (ch == L'\t' || ch == L'\r' || ch == L'\n') {
        std::cerr << "Keyword cannot contain tab, return or newline." << std::endl;
        return false;
      }
    }
  }
  return true;
}


void RecordResolver::resolveLessIO() {
  std::wifstream file(srcFilePath);
  // 一次性读取文件
  std::wstringstream buffer;
  file.imbue(defaultLocale);
  buffer << file.rdbuf();
  std::wstring allContent = buffer.str();
  size_t size = allContent.size();
  // 一次性解析
  auto posInfoList = wtrie.parse_text(allContent);
  //
  size_t nowRecordBegin = 0;
  size_t nowRecordEnd = 0;
  std::wstring record;
  for (const auto& posInfo: posInfoList) {
    // 检查是不是在同一个record中
    // if (posInfo.get_end() < nowRecordEnd) // // 因为posInfo的start是按顺序来的，所以这里不用担心nowRecordEnd < nowRecordBegin， 这里说明一定是在同一个record中
    if (posInfo.get_end() >= nowRecordEnd) {
      // 这里不知道是否在同一个record中，所以要重新找
      std::tie(nowRecordBegin, nowRecordEnd) = recordRange(allContent, posInfo.get_start(), posInfo.get_end());
    }
    record = allContent.substr(nowRecordBegin, nowRecordEnd - nowRecordBegin);
    // 提取关键词
    // 现在nowRecordBegin是record的起始位置
    if(enableCache) {
      auto cachedWords = recordWordsCache.find(nowRecordBegin);
      if (cachedWords != recordWordsCache.end()) {
        listForKeyWords[posInfo.get_index()].push_back(cachedWords->second);
        continue;
      }
    }
    // 说明没有缓存
    std::vector<std::string> words;
    std::string recordStr = CharsetConverter::wstrToStr(record);
    wordExtractor.extractWords(recordStr, words);
    // 缓存
    if (enableCache) {
      recordWordsCache[nowRecordBegin] = words;
    }
    // 存储
    listForKeyWords[posInfo.get_index()].push_back(std::move(words));
  }
  std::string outPutFileName;
  for (size_t i = 0; i < keywords.size(); ++i) {
    outPutFileName = fileNameGenerator(keywords[i], i);
    writeResult(outPutFileName, listForKeyWords[i]);
    std::cout << "Write to " << outPutFileName << std::endl;
  }
  std::cout << "Done." << std::endl;
}
