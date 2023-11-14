#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <utility>
#include <set>
#include <array>
#include "scanner.h"

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains helpers for asm.cc and you don't need to modify it.
 * Read the scanner.h file for a description of the helper functions.
 *
 * This code may be helpful to understand when you write a scanner in a
 * later assignment. However, you do not need to understand how it works
 * to write the assembler.
 */

Token::Token(Token::Kind kind, std::string lexeme):
  kind(kind), lexeme(std::move(lexeme)) {}

  Token:: Kind Token::getKind() const { return kind; }
const std::string &Token::getLexeme() const { return lexeme; }

std::ostream &operator<<(std::ostream &out, const Token &tok) {
  switch (tok.getKind()) {
    case Token::ID:         out << "ID";         break;
    case Token::NUM:      out << "NUM";      break;
    case Token::LPAREN:       out << "LPAREN";       break;
    case Token::RPAREN:      out << "RPAREN";      break;
    case Token::RBRACE:     out << "RBRACE";     break;
    case Token::LBRACE:     out << "LBRACE";     break;
    case Token::RETURN:        out << "RETURN";        break;
    case Token::IF:     out << "IF";     break;
    case Token::ELSE:        out << "ELSE";        break;
    case Token::WHILE: out << "WHILE"; break;
    case Token::PRINTLN:    out << "PRINTLN";    break;
    case Token::WAIN:    out << "WAIN";    break;
    case Token::BECOMES:    out << "BECOMES";    break;
    case Token::INT:    out << "INT";    break;
    case Token::EQ:    out << "EQ";    break;
    case Token::NE:    out << "NE";    break;
    case Token::LT:    out << "LT";    break;
    case Token::GT:    out << "GT";    break;
    case Token::LE:    out << "LE";    break;
    case Token::GE:    out << "GE";    break;
    case Token::PLUS:    out << "PLUS";    break;
    case Token::MINUS:    out << "MINUS";    break;
    case Token::STAR:    out << "STAR";    break;
    case Token::SLASH:    out << "SLASH";    break;
    case Token::PCT:    out << "PCT";    break;
    case Token::COMMA:    out << "COMMA";    break;
    case Token::SEMI:    out << "SEMI";    break;
    case Token::NEW:    out << "NEW";    break;
    case Token::DELETE:    out << "DELETE";    break;
    case Token::LBRACK:    out << "LBRACK";    break;
    case Token::RBRACK:    out << "RBRACK";    break;
    case Token::AMP:    out << "AMP";    break;
    case Token::k_NULL:    out << "NULL";    break;
    case Token::WHITESPACE:    out << "WHITESPACE";    break;
    case Token::COMMENT:    out << "COMMENT";    break;

  }
  out << " " << tok.getLexeme();

  return out;
}

ScanningFailure::ScanningFailure(std::string message):
  message(std::move(message)) {}

const std::string &ScanningFailure::what() const { return message; }

/* Representation of a DFA, used to handle the scanning process.
 */
class AsmDFA {
  public:
    enum State {
      // States that are also kinds
      ZERO,
      NUM,
      LBRACE,
      RBRACE,
      LBRACK,
      RBRACK,
      LPAREN,
      RPAREN,
      PLUS,
      MINUS,
      STAR,
      PCT,
      COMMA,
      SEMI,
      AMP,
      SLASH,
      COMMENT,
      GT,
      GE,
      LT,
      LE,
      BECOMES,
      EQ,
      NE,
      LOW_W,
      LOW_WA,
      LOW_WAI,
      LOW_WAIN,
      ID,
      LOW_WH,
      LOW_WHI,
      LOW_WHIL,
      LOW_WHILE,
      LOW_I,
      LOW_IN,
      LOW_INT,
      LOW_IF,
      LOW_P,
      LOW_PR,
      LOW_PRI,
      LOW_PRIN,
      LOW_PRINT,
      LOW_PRINTL,
      LOW_PRINTLN,
      LOW_E,
      LOW_EL,
      LOW_ELS,
      LOW_ELSE,
      LOW_R,
      LOW_RE,
      LOW_RET,
      LOW_RETU,
      LOW_RETUR,
      LOW_RETURN,
      LOW_N,
      LOW_NE,
      LOW_NEW,
      UP_N,
      UP_NU,
      UP_NUL,
      UP_NULL,
      LOW_D,
      LOW_DE,
      LOW_DEL,
      LOW_DELE,
      LOW_DELET,
      LOW_DELETE,
      WHITESPACE,

      // States that are not also kinds
      FAIL,
      START,
      NOT,

      // Hack to let this be used easily in arrays. This should always be the
      // final element in the enum, and should always point to the previous
      // element.

      LARGEST_STATE = NOT
    };

  private:
    /* A set of all accepting states for the DFA.
     * Currently non-accepting states are not actually present anywhere
     * in memory, but a list can be found in the constructor.
     */
    std::set<State> acceptingStates;

    /*
     * The transition function for the DFA, stored as a map.
     */

    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    /*
     * Converts a state to a kind to allow construction of Tokens from States.
     * Throws an exception if conversion is not possible.
     */
    Token::Kind stateToKind(State s) const {
      switch(s) {
        case ZERO:      return Token::NUM;
        case NUM:       return Token::NUM;
        case LBRACE:    return Token::LBRACE;
        case RBRACE:    return Token::RBRACE;
        case LBRACK:    return Token::LBRACK;
        case RBRACK:    return Token::RBRACK;
        case LPAREN:    return Token::LPAREN;
        case RPAREN:    return Token::RPAREN;
        case PLUS:      return Token::PLUS;
        case MINUS:     return Token::MINUS;
        case STAR:      return Token::STAR;
        case PCT:       return Token::PCT;
        case COMMA:     return Token::COMMA;
        case SEMI:      return Token::SEMI;
        case AMP:       return Token::AMP;
        case SLASH:     return Token::SLASH;
        case COMMENT:   return Token::COMMENT;
        case GT:        return Token::GT;
        case GE:        return Token::GE;
        case LT:        return Token::LT;
        case LE:        return Token::LE;
        case BECOMES:   return Token::BECOMES;
        case EQ:        return Token::EQ;
        case NE:        return Token::NE;
        case LOW_W:     return Token::ID;
        case LOW_WA:    return Token::ID;
        case LOW_WAI:   return Token::ID;
        case LOW_WAIN:  return Token::WAIN;
        case ID:        return Token::ID;
        case LOW_WH:    return Token::ID;
        case LOW_WHI:   return Token::ID;
        case LOW_WHIL:  return Token::ID;
        case LOW_WHILE: return Token::WHILE;
        case LOW_I:     return Token::ID;
        case LOW_IN:    return Token::ID;
        case LOW_INT:   return Token::INT;
        case LOW_IF:    return Token::IF;
        case LOW_P:     return Token::ID;
        case LOW_PR:    return Token::ID;
        case LOW_PRI:   return Token::ID;
        case LOW_PRIN:  return Token::ID;
        case LOW_PRINT: return Token::ID;
        case LOW_PRINTL:    return Token::ID;
        case LOW_PRINTLN:   return Token::PRINTLN;
        case LOW_E:     return Token::ID;
        case LOW_EL:    return Token::ID;
        case LOW_ELS:   return Token::ID;
        case LOW_ELSE:  return Token::ELSE;
        case LOW_R:     return Token::ID;
        case LOW_RE:    return Token::ID;
        case LOW_RET:   return Token::ID;
        case LOW_RETU:  return Token::ID;
        case LOW_RETUR: return Token::ID;
        case LOW_RETURN:    return Token::RETURN;
        case LOW_N:     return Token::ID;
        case LOW_NE:    return Token::ID;
        case LOW_NEW:   return Token::NEW;
        case UP_N:      return Token::ID;
        case UP_NU:     return Token::ID;
        case UP_NUL:    return Token::ID;
        case UP_NULL:   return Token::k_NULL;
        case LOW_D:     return Token::ID;
        case LOW_DE:    return Token::ID;
        case LOW_DEL:   return Token::ID;
        case LOW_DELE:  return Token::ID;
        case LOW_DELET: return Token::ID;
        case LOW_DELETE:    return Token::DELETE;
        case WHITESPACE:    return Token::WHITESPACE;

        default: throw ScanningFailure("ERROR: Cannot convert state to kind.");
      }
    }


  public:
    /* Tokenizes an input string according to the Simplified Maximal Munch
     * scanning algorithm.
     */
    std::vector<Token> simplifiedMaximalMunch(const std::string &input) const {
      std::vector<Token> result;

      State state = start();
      std::string munchedInput;

      // We can't use a range-based for loop effectively here
      // since the iterator doesn't always increment.
      for (std::string::const_iterator inputPosn = input.begin();
           inputPosn != input.end();) {

        State oldState = state;
        state = transition(state, *inputPosn);

        if (!failed(state)) {
          munchedInput += *inputPosn;
          oldState = state;

          ++inputPosn;
        }

        if (inputPosn == input.end() || failed(state)) {
          if (accept(oldState)) {
            result.push_back(Token(stateToKind(oldState), munchedInput));

            munchedInput = "";
            state = start();
          } else {
            if (failed(state)) {
              munchedInput += *inputPosn;
            }
            throw ScanningFailure("ERROR: Simplified maximal munch failed on input: "
                                 + munchedInput);
          }
        }
      }

      return result;
    }

    /* Initializes the accepting states for the DFA.
     */
    AsmDFA() {
      acceptingStates = {ZERO, NUM, LBRACE, RBRACE, LBRACK, RBRACK, LPAREN, RPAREN, PLUS, MINUS, STAR, PCT,
                         COMMA, SEMI, AMP, SLASH, COMMENT, GT, GE, LT, LE, BECOMES, EQ, NE, LOW_W, LOW_WA,
                         LOW_WAI, LOW_WAIN, ID, LOW_WH, LOW_WHI, LOW_WHIL, LOW_WHILE, LOW_I, LOW_IN, LOW_INT,
                         LOW_IF, LOW_P, LOW_PR, LOW_PRI, LOW_PRIN, LOW_PRINT, LOW_PRINTL, LOW_PRINTLN, LOW_E,
                         LOW_EL, LOW_ELS, LOW_ELSE, LOW_R, LOW_RE, LOW_RET, LOW_RETU, LOW_RETUR, LOW_RETURN,
                         LOW_N, LOW_NE, LOW_NEW, UP_N, UP_NU, UP_NUL, UP_NULL, LOW_D, LOW_DE, LOW_DEL, LOW_DELE,
                         LOW_DELET, LOW_DELETE, WHITESPACE};
      //Non-accepting states are START FAIL NOT

      // Initialize transitions for the DFA
      for (size_t i = 0; i < transitionFunction.size(); ++i) {
        for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
          transitionFunction[i][j] = FAIL;
        }
      }

      registerTransition(START, "0", ZERO);
      registerTransition(START, "123456789", NUM);
      registerTransition(START, "{", LBRACE);
      registerTransition(START, "}", RBRACE);
      registerTransition(START, "(", LPAREN);
      registerTransition(START, ")", RPAREN);
      registerTransition(START, "[", LBRACK);
      registerTransition(START, "]", RBRACK);
      registerTransition(START, "+", PLUS);
      registerTransition(START, "-", MINUS);
      registerTransition(START, "*", STAR);
      registerTransition(START, "%", PCT);
      registerTransition(START, ",", COMMA);
      registerTransition(START, ";", SEMI);
      registerTransition(START, "&", AMP);
      registerTransition(START, "/", SLASH);
      registerTransition(START, ">", GT);
      registerTransition(START, "<", LT);
      registerTransition(START, "=", BECOMES);
      registerTransition(START, "!", NOT);
      registerTransition(START, "w", LOW_W);
      registerTransition(START, "i", LOW_I);
      registerTransition(START, "p", LOW_P);
      registerTransition(START, "e", LOW_E);
      registerTransition(START, "r", LOW_R);
      registerTransition(START, "n", LOW_N);
      registerTransition(START, "N", UP_N);
      registerTransition(START, "d", LOW_D);
      registerTransition(START, "abcfghjklmoqstuvxyzABCDEFGHIJKLMOPQRSTUVWXYZ", ID);
      registerTransition(ID, isalnum, ID);
      registerTransition(NUM, isdigit, NUM);
      registerTransition(SLASH, "/", COMMENT);
      registerTransition(START, isspace, WHITESPACE);
      registerTransition(GT, "=", GE);
      registerTransition(LT, "=", LE);
      registerTransition(BECOMES, "=", EQ);
      registerTransition(NOT, "=", NE);
      registerTransition(LOW_W, "a", LOW_WA);
      registerTransition(LOW_W, "h", LOW_WH);
      registerTransition(LOW_W, "bcdefgijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_WA, "i", LOW_WAI);
      registerTransition(LOW_WA, "abcdefghjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_WAI, "n", LOW_WAIN);
      registerTransition(LOW_WAI, "abcdefghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_WAIN, isalnum, ID);
      registerTransition(LOW_WH, "i", LOW_WHI);
      registerTransition(LOW_WH, "abcdefghjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_WHI, "l", LOW_WHIL);
      registerTransition(LOW_WHI, "abcdefghijkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_WHIL, "e", LOW_WHILE);
      registerTransition(LOW_WHIL, "abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_WHILE, isalnum, ID);
      registerTransition(LOW_I, "n", LOW_IN);
      registerTransition(LOW_I, "f", LOW_IF);
      registerTransition(LOW_I, "abcdeghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_IN, "t", LOW_INT);
      registerTransition(LOW_IN, "abcdefghijklmnopqrsuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_INT, isalnum, ID);
      registerTransition(LOW_IF, isalnum, ID);
      registerTransition(LOW_P, "r", LOW_PR);
      registerTransition(LOW_P, "abcdefghijklmnopqstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ZERO);
      registerTransition(LOW_PR, "i", LOW_PRI);
      registerTransition(LOW_PR, "abcdefghjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_PRI, "n", LOW_PRIN);
      registerTransition(LOW_PRI, "abcdefghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_PRIN, "t", LOW_PRINT);
      registerTransition(LOW_PRIN, "abcdefghijklmnopqrsuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_PRINT, "l", LOW_PRINTL);
      registerTransition(LOW_PRINT, "abcdefghijkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_PRINTL, "n", LOW_PRINTLN);
      registerTransition(LOW_PRINTL, "abcdefghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_PRINTLN, isalnum, ID);
      registerTransition(LOW_N, "e", LOW_NE);
      registerTransition(LOW_N, "abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_NE, "w", LOW_NEW);
      registerTransition(LOW_NE, "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_NEW, isalnum, ID);
      registerTransition(LOW_E, "l", LOW_EL);
      registerTransition(LOW_E, "abcdefghijkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_EL, "s", LOW_ELS);
      registerTransition(LOW_EL, "abcdefghijklmnopqrtuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_ELS, "e", LOW_ELSE);
      registerTransition(LOW_ELS, "abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_ELSE, isalnum, ID);
      registerTransition(LOW_R, "e", LOW_RE);
      registerTransition(LOW_R, "abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_RE, "t", LOW_RET);
      registerTransition(LOW_RE, "abcdefghijklmnopqrsuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_RET, "u", LOW_RETU);
      registerTransition(LOW_RET, "abcdefghijklmnopqrstvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_RETU, "r", LOW_RETUR);
      registerTransition(LOW_RETU, "abcdefghijklmnopqstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_RETUR, "n", LOW_RETURN);
      registerTransition(LOW_RETUR, "abcdefghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_RETURN, isalnum, ID);
      registerTransition(UP_N, "U", UP_NU);
      registerTransition(UP_N, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTVWXYZ0123456789", ID);
      registerTransition(UP_NU, "L", UP_NUL);
      registerTransition(UP_NU, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(UP_NUL, "L", UP_NULL);
      registerTransition(UP_NUL, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(UP_NULL, isalnum, ID);
      registerTransition(LOW_D, "e", LOW_DE);
      registerTransition(LOW_D, "abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_DE, "l", LOW_DEL);
      registerTransition(LOW_DE, "abcdefghijkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_DEL, "e", LOW_DELE);
      registerTransition(LOW_DEL, "abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_DELE, "t", LOW_DELET);
      registerTransition(LOW_DELE, "abcdefghijklmnopqrsuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_DELET, "e", LOW_DELETE);
      registerTransition(LOW_DELET, "abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", ID);
      registerTransition(LOW_DELETE, isalnum, ID);
      registerTransition(COMMENT, [](int c) -> int { return c != '\n'; }, COMMENT);
    }

    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
        State newState) {
      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {

      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }

    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }

    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
      return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};

std::vector<Token> scan(const std::string &input) {
  static AsmDFA theDFA;

  std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);

  // We need to:
  // * Throw exceptions for WORD tokens whose lexemes aren't ".word".
  // * Remove WHITESPACE and COMMENT tokens entirely.

  std::vector<Token> newTokens;

  for (auto &token : tokens) {
    if (token.getKind() != Token::WHITESPACE && token.getKind() != Token::Kind::COMMENT) {
      newTokens.push_back(token);
    }
  }

  return newTokens;
}
