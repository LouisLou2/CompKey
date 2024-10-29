//
// Created by leo on 24-10-28.
//
#include "charset_converter.h"
#include <codecvt>
#include <fstream>
#include <iostream>
#include <uchardet.h>
#include <boost/locale.hpp>

std::optional<std::string> CharsetConverter::detectEncoding(std::string_view file_path) {
  // 创建 uchardet 检测器实例
  uchardet_t ud = uchardet_new();
  if (!ud) {
    std::cerr << "Failed to create uchardet instance." << std::endl;
    return std::nullopt;
  }
  // 打开文件，以二进制模式读取
  std::ifstream file((file_path.data()), std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << file_path << std::endl;
    uchardet_delete(ud);
    return std::nullopt;
  }
  // 读取文件内容并传递给 uchardet 检测器
  char buffer[4096];
  while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
    int status = uchardet_handle_data(ud, buffer, file.gcount());
    if (status != 0) {
      std::cerr << "Error during uchardet processing." << std::endl;
      uchardet_delete(ud);
      file.close();
      return std::nullopt;
    }
  }
  // 通知 uchardet 数据传输结束
  uchardet_data_end(ud);
  // 获取检测到的字符编码
  const char *detected_encoding = uchardet_get_charset(ud);
  if (!detected_encoding)
    return std::nullopt;
  std::string detected(detected_encoding);
  // 清理
  uchardet_delete(ud);
  file.close();
  return detected;
}

void CharsetConverter::convertToUtf8(
    const std::string_view& file_path,
    const std::string_view& target_file_path
  ) noexcept(false) {
  auto encodingRes = detectEncoding(file_path);
  if (!encodingRes) {
    throw std::runtime_error("Failed to detect encoding or encoding is unsupported.");
  }
  std::string encoding = *encodingRes;
  if (encoding == "GB18030")
    encoding.insert(0, "zh_CN.");
  std::locale loc = boost::locale::generator().generate(encoding);
  // 开始读取文件
  std::ifstream file((file_path.data()), std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + std::string(file_path));
  }
  std::wofstream target_file(target_file_path.data());
  // 循环读取文件内容
  std::string srcText;
  std::wstring targetText;
  std::locale china("zh_CN.UTF-8");
  if (!target_file.is_open()) {
    throw std::runtime_error("Failed to open file: " + std::string(target_file_path));
  }
  target_file.imbue(china);
  while (std::getline(file, srcText)) {
    // 转换编码并写入文件
    targetText = boost::locale::conv::to_utf<wchar_t>(srcText, loc);
    target_file << targetText << L'\n';
  }
  file.close();
  target_file.close();
}

std::string CharsetConverter::wstrToStr(const std::wstring& wstr) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(wstr);
}
