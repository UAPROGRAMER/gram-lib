#pragma once

#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "gram_lib/token.hpp"
#include "gram_lib/utils.hpp"

struct TokenHash {
  std::size_t operator()(const std::vector<Token>& tokens) const {
    std::size_t hash = 0;
    for (const auto tok : tokens)
      hash ^= std::hash<Token>{}(tok) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
  }
};

class Brain {
 private:
  std::size_t N;
  std::size_t T;

  std::unordered_map<std::vector<Token>, float, TokenHash> probGrams;

 public:
  Brain(std::size_t N, std::size_t T) : N(N), T(T) {
    if (N < 2 || T < 2)
      throw std::runtime_error(
          std::format("Bad brain size: (N : {}, T : {}).", N, T));
  }

  Token predict(const std::vector<Token>& tokens) const {
    std::vector<Token> gram;
    gram.reserve(N);

    for (std::size_t i = 0; i < N - 1; i++)
      gram.push_back(
          tokens.size() < N - 1 - i ? 0 : tokens[tokens.size() - N + 1 + i]);
    gram.push_back(0);

    float val = randFloat();

    for (std::size_t i = 0; i < T; i++) {
      gram[N - 1] = i;

      if (probGrams.contains(gram)) val -= probGrams.at(gram);

      if (val <= 0) return i;
    }

    return Token(randU32() % T);
  }

  void write(std::ostream& stream) const {
    std::size_t temp = probGrams.size();
    std::size_t i = 0;
    stream.write((char*)(&temp), sizeof(std::size_t));
    for (const auto& gram : probGrams) {
      for (const auto& tok : gram.first)
        stream.write((char*)(&tok), sizeof(Token));

      stream.write((char*)(&gram.second), sizeof(float));

      if (++i % 10000 == 0)
        stream.flush();
    }
  }

  void read(std::istream& stream) {
    std::size_t size;
    std::vector<Token> gram;
    gram.reserve(N);

    stream.read((char*)(&size), sizeof(std::size_t));

    for (std::size_t i = 0; i < size; i++) {
      for (std::size_t j = 0; j < N; j++) {
        Token temp;
        stream.read((char*)(&temp), sizeof(Token));
        gram.push_back(temp);
      }

      float val;
      stream.read((char*)(&val), sizeof(float));
      probGrams.emplace(gram, val);

      gram.clear();
    }
  }

  void print(std::ostream& stream) const {
    for (const auto& gram : probGrams) {
      stream << '(';
      for (const auto& tok : gram.first) stream << tok << ',';
      stream << ':' << gram.second << ")\n";
    }
  }

  void set(
      std::unordered_map<std::vector<Token>, float, TokenHash>&& newProdGrams) {
    probGrams = std::move(newProdGrams);
  }
};

class Trainer {
 private:
  std::size_t N;
  std::size_t T;

  std::unordered_map<std::vector<Token>, uint32_t, TokenHash> nMinusGrams;
  std::unordered_map<std::vector<Token>, uint32_t, TokenHash> nGrams;

 public:
  Trainer(std::size_t N, std::size_t T) : N(N), T(T) {
    if (N < 2 || T < 2)
      throw std::runtime_error(
          std::format("Bad trainer size: (N : {}, T : {}).", N, T));
  }

  void train(const std::vector<Token>& tokens) {
    std::vector<Token> gram;
    gram.reserve(N);

    for (std::size_t i = 0; i < tokens.size(); i++) {
      for (std::size_t j = N - 1; j > 0; j--)
        gram.push_back(i < j ? 0 : tokens[i - j]);

      if (nMinusGrams.contains(gram))
        nMinusGrams[gram]++;
      else
        nMinusGrams.emplace(gram, 1);

      gram.push_back(tokens[i]);

      if (nGrams.contains(gram))
        nGrams[gram]++;
      else
        nGrams.emplace(gram, 1);

      gram.clear();
    }
  }

  void write(std::ostream& stream) const {
    std::size_t temp = nMinusGrams.size();
    std::size_t i = 0;
    stream.write((char*)(&temp), sizeof(std::size_t));
    for (const auto& gram : nMinusGrams) {
      for (const auto& tok : gram.first)
        stream.write((char*)(&tok), sizeof(Token));
      stream.write((char*)(&gram.second), sizeof(uint32_t));

      if (++i % 10000 == 0)
        stream.flush();
    }

    temp = nGrams.size();
    stream.write((char*)(&temp), sizeof(std::size_t));
    for (const auto& gram : nGrams) {
      for (const auto& tok : gram.first)
        stream.write((char*)(&tok), sizeof(Token));
      stream.write((char*)(&gram.second), sizeof(uint32_t));

      if (++i % 10000 == 0)
        stream.flush();
    }
  }

  void read(std::istream& stream) {
    std::size_t size;
    std::vector<Token> gram;
    gram.reserve(N);

    stream.read((char*)(&size), sizeof(std::size_t));

    for (std::size_t i = 0; i < size; i++) {
      for (std::size_t j = 0; j < N - 1; j++) {
        Token temp;
        stream.read((char*)(&temp), sizeof(Token));
        gram.push_back(temp);
      }

      uint32_t val;
      stream.read((char*)(&val), sizeof(uint32_t));
      nMinusGrams.emplace(gram, val);

      gram.clear();
    }

    stream.read((char*)(&size), sizeof(std::size_t));

    for (std::size_t i = 0; i < size; i++) {
      for (std::size_t j = 0; j < N; j++) {
        Token temp;
        stream.read((char*)(&temp), sizeof(Token));
        gram.push_back(temp);
      }

      uint32_t val;
      stream.read((char*)(&val), sizeof(uint32_t));
      nGrams.emplace(gram, val);

      gram.clear();
    }
  }

  void print(std::ostream& stream) const {
    for (const auto& gram : nMinusGrams) {
      stream << "($:";
      for (const auto& tok : gram.first) stream << tok << ',';
      stream << ':' << gram.second << ")\n";
    }

    stream << '\n';

    for (const auto& gram : nGrams) {
      stream << '(';
      for (const auto& tok : gram.first) stream << tok << ',';
      stream << ':' << gram.second << ")\n";
    }
  }

  Brain toBrain() const {
    std::unordered_map<std::vector<Token>, float, TokenHash> probGrams;

    std::vector<Token> gram;
    gram.reserve(N);

    for (const auto& lowGram : nMinusGrams) {
      for (const auto& tok : lowGram.first) gram.push_back(tok);

      gram.push_back(0);

      for (std::size_t i = 0; i < T; i++) {
        gram[N - 1] = i;

        if (nGrams.contains(gram))
          probGrams.emplace(
              gram, float(double(nGrams.at(gram) / double(lowGram.second))));
      }

      gram.clear();
    }

    Brain brain(N, T);

    brain.set(std::move(probGrams));

    return brain;
  }
};