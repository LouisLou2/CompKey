//
// Created by leo on 24-10-28.
//

#ifndef RECORD_RESOLVER_H
#define RECORD_RESOLVER_H
#include <aho_corasick.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "word_extractor.h"

using recordWords = std::vector<std::string>;
using recordWordsList = std::vector<recordWords>;

class RecordResolver {
  static const std::locale defaultLocale;
  static constexpr char defaultSeparator = ',';
  // 配置项
  const std::vector<std::wstring> keywords;
  const std::string srcFilePath;
  std::function<std::string(const std::wstring&, uint32_t index)> fileNameGenerator;
  bool uniqueWordInARecord; // 保持唯一性
  bool rmSemanticallyEmpty;
  bool enableCache;
  // 内部成员
  aho_corasick::wtrie wtrie;
  WordExtractor wordExtractor;
  // 内部缓存
  std::unordered_map<size_t, recordWords> recordWordsCache; //其中key是该record第一个字符的index. value是这条记录对应的分词列表

  // 内部数据
  std::vector<recordWordsList> listForKeyWords;

  // 这里就是charPos的index,
  // 向左向右找到record的起始和结束位置，
  // 注意，charPos不可以是分隔符，只能是record的一部分
  // 返回的左闭右开区间
  static std::pair<size_t,size_t> recordRange(
    const std::wstring& allContent, size_t wordBegin, size_t wordEnd
  );
  // keyword不能含有不可见字符，并不能太长，整个list不能为空
  static bool checkKeywords(const std::vector<std::wstring>& keywords);
  static void writeResult(
    const std::string& fileName,
    const recordWordsList& listForKeyWords
  );
public:
  RecordResolver(
    const std::vector<std::wstring>& keywords,
    std::string  srcFilePath,
    std::function<std::string(const std::wstring&, uint32_t index)> fileNameGenerator,
    bool uniqueWordInARecord = true,
    bool rmSemanticallyEmpty = true,
    bool enableCache = false,
    std::optional<std::set<char>> symbols = std::nullopt
  );
  /*
   * 仅仅提供resolveLessIO，他会一次性读取文件，一次性解析,最大程度减少IO
   * 缺点是，如果文件过大，会导致内存占用过大
   */
  void resolveLessIO();
};

#endif //RECORD_RESOLVER_H
