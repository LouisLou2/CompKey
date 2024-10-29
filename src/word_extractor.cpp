//
// Created by leo on 24-10-28.
//
#include <word_extractor.h>

std::unique_ptr<cppjieba::MixSegment> WordExtractor::seg = nullptr;
const std::string WordExtractor::mpSegDict = "/home/leo/CLionProjects/compkey/resource/dict/jieba.dict.utf8";
const std::string WordExtractor::hmmSegDict = "/home/leo/CLionProjects/compkey/resource/dict/hmm_model.utf8";

/* 名词，动词，形容词，副词，成语
 * 数词和英语都不会包含其中, 所以数字，网址，邮箱等都不会被提取
 */
const std::set<char> WordExtractor::defaultSymbols = {'n', 'v', 'a', 'd','i'};

WordExtractor::WordExtractor(
  bool unique,
  bool rmSemanticallyEmpty,
  std::optional<std::set<char>> symbols
):
  unique(unique),
  rmSemanticallyEmpty(rmSemanticallyEmpty) {
  if (symbols) {
    this->symbolsCopyIfNeed = std::move(symbols.value());
    this->symbols = &this->symbolsCopyIfNeed;
  }else {
    this->symbols = &defaultSymbols;
  }
  assert(!this->symbols->empty());
}

WordExtractor::WordExtractor(
  std::set<char>&& symbols,
  bool unique,
  bool rmSemanticallyEmpty):
  unique(unique),
  rmSemanticallyEmpty(rmSemanticallyEmpty),
  symbolsCopyIfNeed(std::move(symbols)),
  symbols(&symbolsCopyIfNeed) {
  assert(!this->symbols->empty());
}

void WordExtractor::initSeg() {
  if (!seg) {
    seg = std::make_unique<cppjieba::MixSegment>(mpSegDict, hmmSegDict);
  }
}

void WordExtractor::extractWords(
  const std::string& sentence,
  std::vector<std::string>& words
) {
  assert (!symbols->empty());
  if (sentence.empty()) return;
  if (!seg) initSeg();
  seg->Cut(sentence, words);
  filter(words);
}
