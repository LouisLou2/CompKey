//
// Created by leo on 24-10-28.
//

#ifndef WORD_EXTRACTOR_H
#define WORD_EXTRACTOR_H
#include <memory>
#include <cppjieba/MixSegment.hpp>

class WordExtractor {
  static std::unique_ptr<cppjieba::MixSegment> seg;
  static const std::set<char> defaultSymbols;
  static const std::string mpSegDict;
  static const std::string hmmSegDict;
  static constexpr u_int32_t MAX_WORD_LENGTH = 1000;

  std::set<char> symbolsCopyIfNeed;
  // 配置项
  const std::set<char>* symbols;
  bool unique; // 保持唯一性
  bool rmSemanticallyEmpty;

  std::set<std::string> wordsSet; // store the words that have been extracted, to ensure the uniqueness of the words, 每次使用前清空

  inline void filter(std::vector<std::string>& words);
  [[nodiscard]] inline bool isValidWord(const std::string& word, const std::string& tag) const;
public:
  static void initSeg();
  /*
   * Extract words from a sentence
   * @param sentence: the sentence to extract words from
   * @param words: the vector to store the extracted words
   * @param symbols: the set of symbols to extract
   */
  explicit WordExtractor(
    bool unique = true,
    bool rmSemanticallyEmpty = true,
    std::optional<std::set<char>> symbols = std::nullopt
  );
  explicit WordExtractor(
    std::set<char>&& symbols,
    bool unique = true,
    bool rmSemanticallyEmpty = true
   );

  // 这里如果sentence可以使用string_view会更好，奈何接口不支持
  void extractWords(
    const std::string& sentence,
    std::vector<std::string>& words
  );
};

inline bool WordExtractor::isValidWord(const std::string& word, const std::string& tag) const {
  if (tag.empty() || !symbols->contains(tag[0])) {
    return false;
  }
  if (rmSemanticallyEmpty) {
    if (tag[0] == 'v' && word.size() < 2) {
      return false;
    }
  }
  if (unique && wordsSet.contains(word)) {
    return false;
  }
  return true;
}

inline void WordExtractor::filter(std::vector<std::string>& words) {
  if (unique) {
    wordsSet.clear();
  }
  uint32_t avaBegin = 0;
  uint32_t avaEnd = 0;
  std::string tag;
  while (avaEnd < words.size()) {
    tag = seg->LookupTag(words[avaEnd]);
    if (!isValidWord(words[avaEnd], tag)) {
      ++avaEnd;
      continue;
    }
    if (unique) {
      wordsSet.insert(words[avaEnd]);
    }
    if (avaBegin != avaEnd) {
      words[avaBegin] = std::move(words[avaEnd]);
    }
    ++avaBegin;
    ++avaEnd;
  }
  words.resize(avaBegin);
}

#endif //WORD_EXTRACTOR_H
