//
// Created by leo on 24-10-28.
//
#include <word_extractor.h>

std::unique_ptr<cppjieba::MixSegment> WordExtractor::seg = nullptr;
const std::string WordExtractor::mpSegDict = "D:/projects/CompKey1/resource/dict/jieba.dict.utf8";
const std::string WordExtractor::hmmSegDict = "D:/projects/CompKey1/resource/dict/hmm_model.utf8";

/* 名词，动词，形容词，副词，成语
 * 数词和英语都不会包含其中, 所以数字，网址，邮箱等都不会被提取
 */
const std::set<char> WordExtractor::defaultSymbols = {'n','i'};

WordExtractor::WordExtractor(
 uint8_t maxWordPerRec,
  bool unique,
  bool rmSemanticallyEmpty,
  std::optional<std::set<char>> symbols
):
  unique(unique),
  rmSemanticallyEmpty(rmSemanticallyEmpty),
  maxWordsPerRec(maxWordPerRec),
  strSet(maxWordPerRec){
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
  uint8_t maxWordPerRec,
  bool unique,
  bool rmSemanticallyEmpty):
  unique(unique),
  rmSemanticallyEmpty(rmSemanticallyEmpty),
  maxWordsPerRec(maxWordPerRec),
  strSet(maxWordPerRec),
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
