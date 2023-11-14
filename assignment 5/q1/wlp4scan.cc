#include "scanner.h"
#include <iostream>
#include <string>
#include <vector>

bool inRange(Token token) {
    std::string str = token.getLexeme();
    if(str.length() < 10) return true;
    if(str.length() > 10) return false;
    if(str[0] > '2') return false;
    if(str[0] < '2') return true;
    if(str[1] > '1') return false;
    if(str[1] < '1') return true;
    if(str[2] > '4') return false;
    if(str[2] < '4') return true;
    if(str[3] > '7') return false;
    if(str[3] < '7') return true;
    if(str[4] > '4') return false;
    if(str[4] < '4') return true;
    if(str[5] > '8') return false;
    if(str[5] < '8') return true;
    if(str[6] > '3') return false;
    if(str[6] < '3') return true;
    if(str[7] > '6') return false;
    if(str[7] < '6') return true;
    if(str[8] > '4') return false;
    if(str[8] < '4') return true;
    if(str[9] > '7') return false;
    return true;
}

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
      for(unsigned int i = 0; i < tokenLine.size(); ++i) {
        if(tokenLine[i].getKind() == Token::NUM) {
            if(!inRange(tokenLine[i])) {
                std::cerr << "ERROR: the number is out of range!" << std::endl;
                return 1;
            }
        }
        std::cout << tokenLine[i] << std::endl;
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
