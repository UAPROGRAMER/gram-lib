#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <gram_lib/gram.hpp>
#include <gram_lib/utils.hpp>
#include <ios>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "lexer.hpp"

namespace fs = std::filesystem;
using ios = std::ios;

#define CONTEXT (10)
#define TOKENS (78)

void help() {
  std::cout << "Usage:\n";
  std::cout << "\tgramtool [-h | --help]\n";
  std::cout << "\tgramtool [-t | --train] <trainer> <data>\n";
  std::cout << "\tgramtool [-b | --tobrain] <trainer> <brain>\n";
  std::cout << "\tgramtool [-p | --predict] <brain> <file> <amount>\n";
  std::cout << "\tgramtool [-T | --printtrainer] <trainer> <file>\n";
  std::cout << "\tgramtool [-B | --printbrain] <brain> <file>\n\n";
  std::cout << "Info:\n";
  std::cout << "\t<trainer> | <brain> | <file> - path to file.\n";
  std::cout << "\t<data>                       - path to file or directory.\n";
  std::cout << "\t<amount>                     - u64 value.\n";
}

void train(std::string_view trainer_path, std::string_view training_data) {
  if (!fs::exists(training_data)) return;

  Trainer trainer(CONTEXT, TOKENS);

  if (fs::exists(trainer_path)) {
    std::ifstream trainer_file((fs::path(trainer_path)), ios::binary);
    trainer.read(trainer_file);
  }

  if (fs::is_regular_file(training_data)) {
    std::vector<Token> tokens;

    tokenize(tokens, readToString(training_data));

    trainer.train(tokens);
  } else {
    std::size_t amount = 0;

    for (auto path : fs::recursive_directory_iterator(training_data))
      if (fs::is_regular_file(path)) amount++;

    std::size_t count = 0;

    for (auto file : fs::recursive_directory_iterator(training_data)) {
      if (!fs::is_regular_file(file)) continue;

      std::vector<Token> tokens;

      tokenize(tokens, readToString(file.path().string()));

      trainer.train(tokens);

      count++;

      std::cout << "[" << count << "/" << amount << "]\n";
    }
  }

  std::ofstream trainer_file((fs::path(trainer_path)), ios::binary);

  trainer.write(trainer_file);
}

void tobrain(std::string_view trainer_path, std::string_view brain_path) {
  Trainer trainer(CONTEXT, TOKENS);

  if (fs::exists(trainer_path)) {
    std::ifstream trainer_file((fs::path(trainer_path)), ios::binary);
    trainer.read(trainer_file);
  }

  std::ofstream brain_file((fs::path(brain_path)), ios::binary);

  Brain brain = trainer.toBrain();

  brain.write(brain_file);
}

void predict(std::string_view brain_path, std::string_view text_file,
             std::size_t count) {
  Brain brain(CONTEXT, TOKENS);

  if (fs::exists(brain_path)) {
    std::ifstream trainer_file((fs::path(brain_path)), ios::binary);
    brain.read(trainer_file);
  }

  std::vector<Token> tokens;

  if (fs::exists(text_file)) tokenize(tokens, readToString(text_file));

  for (std::size_t i = 0; i < count; i++)
    tokens.push_back(brain.predict(tokens));

  std::ofstream textFile((fs::path(text_file)));

  tokensWrite(tokens, textFile);
}

void printtrainer(std::string_view trainer_path, std::string_view text_file) {
  Trainer trainer(CONTEXT, TOKENS);

  if (fs::exists(trainer_path)) {
    std::ifstream trainer_file((fs::path(trainer_path)), ios::binary);
    trainer.read(trainer_file);
  }

  std::ofstream textFile((fs::path(text_file)));

  trainer.print(textFile);
}

void printbrain(std::string_view brain_path, std::string_view text_file) {
  Brain brain(CONTEXT, TOKENS);

  if (fs::exists(brain_path)) {
    std::ifstream trainer_file((fs::path(brain_path)), ios::binary);
    brain.read(trainer_file);
  }

  std::ofstream textFile((fs::path(text_file)));

  brain.print(textFile);
}

int main(int argc, char** argv) {
  if (argc < 2) return 1;

  std::string mode(argv[1]);

  try {
    if (mode == "--help" || mode == "-h")
      help();
    else if (mode == "--train" || mode == "-t") {
      if (argc < 4) return 1;

      train(argv[2], argv[3]);
    } else if (mode == "--tobrain" || mode == "-b") {
      if (argc < 4) return 1;

      tobrain(argv[2], argv[3]);
    } else if (mode == "--predict" || mode == "-p") {
      if (argc < 5) return 1;

      predict(argv[2], argv[3], std::stoull(argv[4]));
    } else if (mode == "--printtrainer" || mode == "-T") {
      if (argc < 4) return 1;

      printtrainer(argv[2], argv[3]);
    } else if (mode == "--printbrain" || mode == "-B") {
      if (argc < 4) return 1;

      printbrain(argv[2], argv[3]);
    } else
      return 1;
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}