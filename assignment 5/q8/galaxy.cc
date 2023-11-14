#include <iostream>
#include <string>
using std::string;
using std::istream;
using std::ostream;

//Skip the grammar part of the input.
void skipLine(istream &in) {
  string s;
  getline(in, s);
}

void skipGrammar(istream &in) {
  int i, numTerm, numNonTerm, numRules;

  // read the number of terminals and move to the next line
  in >> numTerm;
  skipLine(in);

  // skip the lines containing the terminals
  for (i = 0; i < numTerm; i++) {
    skipLine(in);
  }

  // read the number of non-terminals and move to the next line
  in >> numNonTerm;
  skipLine(in);

  // skip the lines containing the non-terminals
  for (i = 0; i < numNonTerm; i++) {
    skipLine(in);
  }

  // skip the line containing the start symbol
  skipLine(in);

  // read the number of rules and move to the next line
  in >> numRules;
  skipLine(in);

  // skip the lines containing the production rules
  for (i = 0; i < numRules; i++) {
    skipLine(in);
  }
}

string trim(const string &str) {
  size_t begin = str.find_first_not_of(" \t\n");
  if (begin == string::npos) return "";

  size_t end = str.find_last_not_of(" \t\n");

  return str.substr(begin, end - begin + 1);
}

// Prints the derivation with whitespace trimmed.
void printDerivation(istream &in, ostream &out) {
  string line;

  while (getline(in, line)) {
    out << trim(line) << '\n';
  }
}

int calculate(istream &in) {
    string line;
    getline(in, line);

    if(trim(line) == "term id") return 42;
    if(trim(line) == "S BOF expr EOF")  return calculate(in);
    if(trim(line) == "expr term") return calculate(in);
    if(trim(line) == "term ( expr )") return calculate(in);
    if(trim(line) == "expr expr - term") {
        return calculate(in) - calculate(in);
    }
    std::cerr << "ERROR, invalid production rule found: " << trim(line) << " "<< std::endl;
    return 0;
}

// Reads a .cfg file and prints the derivation
// without leading or trailing spaces.
int main() {
  skipGrammar(std::cin);
//  printDerivation(std::cin, std::cout);
  std::cout << calculate(std::cin) << std::endl;
}
