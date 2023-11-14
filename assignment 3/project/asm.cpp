#include <iostream>
#include <string>
#include <vector>
#include "scanner.h"

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */
int main() {
  std::string line;
  try {
    while (getline(std::cin, line)) {
      // This example code just prints the scanned tokens on each line.
      std::vector<Token> tokenLine = scan(line);
      if (tokenLine.size()==1 && tokenLine[0].getKind()==Token::WORD) {

      }
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.

  return 0;
}
