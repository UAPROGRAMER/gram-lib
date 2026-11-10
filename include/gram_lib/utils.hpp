#pragma once

#include <cstdint>
#include <format>
#include <fstream>
#include <random>
#include <string>
#include <string_view>

inline std::string readToString(std::string_view path) {
  std::ifstream file((std::string(path)));

  if (!file)
    throw std::runtime_error(std::format("Failed to open file: {}", path));

  file.seekg(0, std::ios::end);

  size_t size = static_cast<size_t>(file.tellg());

  std::string text(size, '\0');

  file.seekg(0);

  file.read(text.data(), size);

  return text;
}

inline float randFloat() {
  static std::default_random_engine e(time(nullptr));
  static std::uniform_real_distribution<> dis(0, 1);  // range [0, 1)
  return dis(e);
}

inline uint32_t randU32() {
  static std::default_random_engine e(time(nullptr));
  static std::uniform_int_distribution<> dis(0, uint32_t(-1));
  return dis(e);
}