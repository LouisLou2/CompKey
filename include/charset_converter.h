//
// Created by leo on 24-10-28.
//

#ifndef CHARSET_CONVERTER_H
#define CHARSET_CONVERTER_H
#include <optional>
#include <string>

class CharsetConverter {
public:
  static std::optional<std::string> detectEncoding(std::string_view file_path);
  static void convertToUtf8(
    const std::string_view &file_path,
    const std::string_view &target_file_path
  ) noexcept(false);
  static std::string wstrToStr(const std::wstring &wstr);
};

#endif //CHARSET_CONVERTER_H
