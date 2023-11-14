/*  CS241 Scanner
    Starter code for the CS 241 assembler (assignments 3 and 4).
    Code contained here may be included in submissions for CS241
    assignments at the University of Waterloo.
    ---------------------------------------------------------------
    To compile on a CSCF linux machine, use:
            g++ -g asm.cc -o asm
    To run:
            ./asm           < source.asm > program.mips
            valgrind ./asm  < source.asm > program.mips
 */

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
using namespace std;

//======================================================================
//========= Declarations for the scan() function =======================
//======================================================================

// Each token has one of the following kinds.

enum Kind {
    ID,                 // Opcode or identifier (use of a label)
    INT,                // Decimal integer
    HEXINT,             // Hexadecimal integer
    REGISTER,           // Register number
    COMMA,              // Comma
    LPAREN,             // (
    RPAREN,             // )
    LABEL,              // Declaration of a label (with a colon)
    DOTWORD,            // .word directive
    WHITESPACE,         // Whitespace
    NUL                 // Bad/invalid token
};

// kindString(k) returns string a representation of kind k
// that is useful for error and debugging messages.
string kindString(Kind k);

// Each token is described by its kind and its lexeme.

struct Token {
    Kind      kind;
    string    lexeme;
    /* toInt() returns an integer representation of the token. For tokens
     * of kind INT (decimal integer constant) and HEXINT (hexadecimal integer
     * constant), returns the integer constant. For tokens of kind
     * REGISTER, returns the register number.
     */
    int       toInt();
};

// scan() separates an input line into a vector of Tokens.
vector<Token> scan(string input);

// =====================================================================
// The implementation of scan() and associated type definitions.
// If you just want to use the scanner, skip to the next ==== separator.

// States for the finite-state automaton that comprises the scanner.

enum State {
    ST_NUL,
    ST_START,
    ST_DOLLAR,
    ST_MINUS,
    ST_REGISTER,
    ST_INT,
    ST_ID,
    ST_LABEL,
    ST_COMMA,
    ST_LPAREN,
    ST_RPAREN,
    ST_ZERO,
    ST_ZEROX,
    ST_HEXINT,
    ST_COMMENT,
    ST_DOT,
    ST_DOTW,
    ST_DOTWO,
    ST_DOTWOR,
    ST_DOTWORD,
    ST_WHITESPACE
};

// The *kind* of token (see previous enum declaration)
// represented by each state; states that don't represent
// a token have stateKinds == NUL.

Kind stateKinds[] = {
    NUL,            // ST_NUL
    NUL,            // ST_START
    NUL,            // ST_DOLLAR
    NUL,            // ST_MINUS
    REGISTER,       // ST_REGISTER
    INT,            // ST_INT
    ID,             // ST_ID
    LABEL,          // ST_LABEL
    COMMA,          // ST_COMMA
    LPAREN,         // ST_LPAREN
    RPAREN,         // ST_RPAREN
    INT,            // ST_ZERO
    NUL,            // ST_ZEROX
    HEXINT,         // ST_HEXINT
    WHITESPACE,     // ST_COMMENT
    NUL,            // ST_DOT
    NUL,            // ST_DOTW
    NUL,            // ST_DOTWO
    NUL,            // ST_DOTWOR
    DOTWORD,        // ST_DOTWORD
    WHITESPACE      // ST_WHITESPACE
};

State delta[ST_WHITESPACE+1][256];

#define whitespace "\t\n\r "
#define letters    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define digits     "0123456789"
#define hexDigits  "0123456789ABCDEFabcdef"
#define oneToNine  "123456789"

void setT(State from, string chars, State to) {
    for(unsigned i = 0; i < chars.length(); i++ ) delta[from][chars[i]] = to;
}

void initT(){
    int i, j;

    // The default transition is ST_NUL (i.e., no transition
    // defined for this char).
    for ( i=0; i<=ST_WHITESPACE; i++ ) {
        for ( j=0; j<256; j++ ) {
            delta[i][j] = ST_NUL;
        }
    }
    // Non-null transitions of the finite state machine.
    // NB: in the third line below, letters digits are macros
    // that are replaced by string literals, which the compiler
    // will concatenate into a single string literal.
    setT( ST_START,      whitespace,     ST_WHITESPACE );
    setT( ST_WHITESPACE, whitespace,     ST_WHITESPACE );
    setT( ST_START,      letters,        ST_ID         );
    setT( ST_ID,         letters digits, ST_ID         );
    setT( ST_START,      oneToNine,      ST_INT        );
    setT( ST_INT,        digits,         ST_INT        );
    setT( ST_START,      "-",            ST_MINUS      );
    setT( ST_MINUS,      digits,      	 ST_INT        );
    setT( ST_START,      ",",            ST_COMMA      );
    setT( ST_START,      "(",            ST_LPAREN     );
    setT( ST_START,      ")",            ST_RPAREN     );
    setT( ST_START,      "$",            ST_DOLLAR     );
    setT( ST_DOLLAR,     digits,         ST_REGISTER   );
    setT( ST_REGISTER,   digits,         ST_REGISTER   );
    setT( ST_START,      "0",            ST_ZERO       );
    setT( ST_ZERO,       "x",            ST_ZEROX      );
    setT( ST_ZERO,       digits,      	 ST_INT        );
    setT( ST_ZEROX,      hexDigits,      ST_HEXINT     );
    setT( ST_HEXINT,     hexDigits,      ST_HEXINT     );
    setT( ST_ID,         ":",            ST_LABEL      );
    setT( ST_START,      ";",            ST_COMMENT    );
    setT( ST_START,      ".",            ST_DOT        );
    setT( ST_DOT,        "w",            ST_DOTW       );
    setT( ST_DOTW,       "o",            ST_DOTWO      );
    setT( ST_DOTWO,      "r",            ST_DOTWOR     );
    setT( ST_DOTWOR,     "d",            ST_DOTWORD    );

    for ( j=0; j<256; j++ ) delta[ST_COMMENT][j] = ST_COMMENT;
}

static int initT_done = 0;

vector<Token> scan(string input){
    // Initialize the transition table when called for the first time.
    if(!initT_done) {
        initT();
        initT_done = 1;
    }

    vector<Token> ret;

    int i = 0;
    int startIndex = 0;
    State state = ST_START;

    if(input.length() > 0) {
        while(true) {
            State nextState = ST_NUL;
            if(i < input.length())
                nextState = delta[state][(unsigned char) input[i]];
            if(nextState == ST_NUL) {
                // no more transitions possible
                if(stateKinds[state] == NUL) {
                    throw("ERROR in lexing after reading " + input.substr(0, i));
                }
                if(stateKinds[state] != WHITESPACE) {
                    Token token;
                    token.kind = stateKinds[state];
                    token.lexeme = input.substr(startIndex, i-startIndex);
                    ret.push_back(token);
                }
                startIndex = i;
                state = ST_START;
                if(i >= input.length()) break;
            } else {
                state = nextState;
                i++;
            }
        }
    }

    return ret;
}

int Token::toInt() {
    if(kind == INT) {
        long long l;
        sscanf( lexeme.c_str(), "%lld", &l );
	if (lexeme.substr(0,1) == "-") {
            if(l < -2147483648LL)
                throw("ERROR: constant out of range: "+lexeme);
	} else {
	    unsigned long long ul = l;
            if(ul > 4294967295LL)
                throw("ERROR: constant out of range: "+lexeme);
	}
        return l;
    } else if(kind == HEXINT) {
        long long l;
        sscanf( lexeme.c_str(), "%llx", &l );
	unsigned long long ul = l;
        if(ul > 0xffffffffLL)
            throw("ERROR: constant out of range: "+lexeme);
        return l;
    } else if(kind == REGISTER) {
        long long l;
        sscanf( lexeme.c_str()+1, "%lld", &l );
	unsigned long long ul = l;
        if(ul > 31)
            throw("ERROR: constant out of range: "+lexeme);
        return l;
    }
    throw("ERROR: attempt to convert non-integer token "+lexeme+" to Int");
}

// kindString maps each kind to a string for use in error messages.

string kS[] = {
    "ID",           // ID
    "INT",          // INT
    "HEXINT",       // HEXINT
    "REGISTER",     // REGISTER
    "COMMA",        // COMMA
    "LPAREN",       // LPAREN
    "RPAREN",       // RPAREN
    "LABEL",        // LABEL
    "DOTWORD",      // DOTWORD
    "WHITESPACE",   // WHITESPACE
    "NUL"           // NUL
};

string kindString( Kind k ){
    if ( k < ID || k > NUL ) return "INVALID";
    return kS[k];
}
























// JASONSUN's MODIFICATIONS BELOW

#include <cstdio>    // outbyte
#include <cstdlib>   // exit
#include <map>       // labelling
#include <stdint.h>  // uint16_t
//#include "dbgmsg.h"  // debugging - remove when submitting

//======================================================================
//=======            Binary output procedures                    =======
//======================================================================
void outbyte (int i) {
   putchar (i>>24);
   putchar (i>>16);
   putchar (i>>8);
   putchar (i);
}

void asm_jr (int s) {outbyte ((s<<21)|8);}
void asm_jalr (int s) {outbyte ((s<<21)|9);}

void asm_add  (int d, int s, int t) {outbyte ((s<<21)|(t<<16)|(d<<11)|32);}
void asm_sub  (int d, int s, int t) {outbyte ((s<<21)|(t<<16)|(d<<11)|34);}
void asm_slt  (int d, int s, int t) {outbyte ((s<<21)|(t<<16)|(d<<11)|42);}
void asm_sltu (int d, int s, int t) {outbyte ((s<<21)|(t<<16)|(d<<11)|43);}

void asm_beq (int s, int t, uint16_t i) {outbyte (0x10000000|(s<<21)|(t<<16)|i);}
void asm_bne (int s, int t, uint16_t i) {outbyte (0x14000000|(s<<21)|(t<<16)|i);}

void asm_lis  (int d) {outbyte ((d<<11)|20);}
void asm_mflo (int d) {outbyte ((d<<11)|18);}
void asm_mfhi (int d) {outbyte ((d<<11)|16);}

void asm_mult  (int s, int t) {outbyte ((s<<21)|(t<<16)|24);}
void asm_multu (int s, int t) {outbyte ((s<<21)|(t<<16)|25);}
void asm_div   (int s, int t) {outbyte ((s<<21)|(t<<16)|26);}
void asm_divu  (int s, int t) {outbyte ((s<<21)|(t<<16)|27);}

void asm_lw (int t, uint16_t i, int s) {outbyte (0x8c000000|(s<<21)|(t<<16)|i);}
void asm_sw (int t, uint16_t i, int s) {outbyte (0xac000000|(s<<21)|(t<<16)|i);}

// Binary output procedures
//^====================================================================^




//======================================================================
//=======            Checks                                      =======
//======================================================================

// true if label exists
bool tokenLabelExists (map <string, int> &labelMap, Token tokenLabel) {
//   DBGVAR (cout, tokenLabel.lexeme);
   map <string, int>::iterator it = labelMap.find(tokenLabel.lexeme);
   if (it != labelMap.end()) {   return true;   }
   return false;
}

// displayLabels
void displayLabels (map<string, int> &labelMap) {
   for (map<string, int>::const_iterator it = labelMap.begin();
   it != labelMap.end(); it++) {
         cerr << it->first << " " << it->second << endl;
   }
}

bool inRange16signed (int x) {
   if (x >= -32767 && x <= 32767) {return true;}
   return false;
}



// true if current line has a sequence of:
// REGISTER
// used in jr, jalr, lis, mflo, mfhi
bool check_reg (vector<vector<Token> > &tokLines, int line, int j) {
   if (tokLines[line].size() < j + 2) {return false;}
   Token tok1 = tokLines[line][j+1];
   if (  tok1.kind == REGISTER && inRange16signed (tok1.toInt()) ) {return true;}
   return false;
}

// true if current line has a sequence of:
// REGISTER, COMMA, REGISTER
// used in mult, multu, div, divu
bool check_reg_comma_reg (vector<vector<Token> > &tokLines, int line, int j) {
   if (tokLines[line].size() < j + 4) {return false;}
   Token tok1 = tokLines[line][j+1];
   Token tok2 = tokLines[line][j+2];
   Token tok3 = tokLines[line][j+3];
   if (
         tok1.kind == REGISTER &&
         tok2.kind == COMMA &&
         tok3.kind == REGISTER )    {  return true;   }
   return false;
}

// true if current line has a sequence of:
// REGISTER, COMMA, REGISTER, COMMA, REGISTER
// used in add, sub, slt, sltu
bool check_reg_comma_reg_comma_reg (vector<vector<Token> > &tokLines, int line, int j) {
   if (tokLines[line].size() < j + 6) {return false;}
   Token tok1 = tokLines[line][j+1];
   Token tok2 = tokLines[line][j+2];
   Token tok3 = tokLines[line][j+3];
   Token tok4 = tokLines[line][j+4];
   Token tok5 = tokLines[line][j+5];
   if (
         (tok1.kind == REGISTER && tok1.toInt() >= 0 && tok1.toInt() <= 31) &&
         tok2.kind == COMMA &&
         (tok3.kind == REGISTER && tok3.toInt() >= 0 && tok3.toInt() <= 31) &&
         tok4.kind == COMMA &&
         (tok5.kind == REGISTER && tok5.toInt() >= 0 && tok5.toInt() <= 31)   )

      {  return true;   }
   return false;
}

// true if the current line has a sequence of:
// REGISTER, COMMA, REGISTER, COMMA, INT or HEXINT
// used in beq bne
bool check_reg_comma_reg_comma_intHexintLabel (vector<vector<Token> > &tokLines, int line, int j) {
   if (tokLines[line].size() < j + 6) {return false;}
   Token tok1 = tokLines[line][j+1];
   Token tok2 = tokLines[line][j+2];
   Token tok3 = tokLines[line][j+3];
   Token tok4 = tokLines[line][j+4];
   Token tok5 = tokLines[line][j+5];
   if (
         tok1.kind == REGISTER &&
         tok2.kind == COMMA &&
         tok3.kind == REGISTER &&
         tok4.kind == COMMA  &&
         (( tok5.kind == INT && inRange16signed (tok5.toInt())) ||
          ( tok5.kind == HEXINT && inRange16signed (tok5.toInt())) ||
            tok5.kind == ID)  )
       {  return true;  }

   return false;
}


// true if the current line has a sequence of:
// REGISTER, COMMA, INT, LPARENS, REGISTER, RPARENS
// used in sw lw
bool check_reg_comma_intHex_lpar_reg_rpar (vector<vector<Token> > &tokLines, int line, int j) {
   if (tokLines[line].size() < j + 7) {return false;}
   Token tok1 = tokLines[line][j+1];
   Token tok2 = tokLines[line][j+2];
   Token tok3 = tokLines[line][j+3];
   Token tok4 = tokLines[line][j+4];
   Token tok5 = tokLines[line][j+5];
   Token tok6 = tokLines[line][j+6];
   if (
         tok1.kind == REGISTER &&
         tok2.kind == COMMA &&
         (  ((tok3.kind == INT || tok3.kind == HEXINT)) &&
            (inRange16signed (tok3.toInt()))) &&
         tok4.kind == LPAREN  &&
         tok5.kind == REGISTER &&
         tok6.kind == RPAREN )   {  return true;  }

   return false;
}

// Check procedures
//^====================================================================^



//======================================================================
//=======            Pass # 1                                    =======
//======================================================================
// goes through the code and see if the label isn't already defined
// then add a (label, line) entry on a map
void pass1 (vector<vector<Token> > &tokLines, map <string, int> &labelMap, int &labelNumber) {
   for(unsigned line=0; line < tokLines.size(); line++ ) {
      for(unsigned j=0; j < tokLines[line].size(); j++ ) {

         Token token = tokLines[line][j];
         if ((token.kind == DOTWORD) && (j+1 < tokLines[line].size())) {
            Token nextToken = tokLines[line][j+1];
            map <string, int>::iterator it = labelMap.find(nextToken.lexeme);
            if ((nextToken.kind == ID) && (it != labelMap.end())) {
               labelNumber++;
            }
            else if (nextToken.kind == INT
            || nextToken.kind == HEXINT
            || nextToken.kind == ID) {
               labelNumber++;
            }
         }

         token = tokLines[line][j];
         if (token.kind == LABEL) {
            string trimmedLexeme = token.lexeme.substr(0, token.lexeme.length()-1);
            map <string, int>::iterator it = labelMap.find(trimmedLexeme);
            if ((it == labelMap.end()) &&
            (((j >= 1) && (tokLines[line][j-1].kind == LABEL)) || (j == 0))) {
                  labelMap[trimmedLexeme] = labelNumber * 4;
            } else {
               cerr << "ERROR on labelNumber" << line << " "
               << token.lexeme << " is already defined with value: "
               << it->second << endl;
               line ++;
            }
         } // if (LABEL)

         // check for valid instructions, add labels and increment counter
         if (token.kind == ID) {

            if  (    ((token.lexeme == "jr") || (token.lexeme == "jalr"))  &&
                     (check_reg (tokLines, line, j))  )
               {labelNumber++;}

            if  (    ((token.lexeme == "add") || (token.lexeme == "sub" ) ||
                      (token.lexeme == "slt") || (token.lexeme == "sltu"))  &&
                     (check_reg_comma_reg_comma_reg (tokLines, line, j))  )
               {labelNumber++;}

            if  (    ((token.lexeme == "beq") || (token.lexeme == "bne" ))  &&
                     (check_reg_comma_reg_comma_intHexintLabel (tokLines, line, j))  )
               {labelNumber++;}

            if  (    ((token.lexeme == "lis") || (token.lexeme == "mflo" ) ||
                      (token.lexeme == "mfhi") )  &&
                     (check_reg (tokLines, line, j))  )
               {labelNumber++;}

            if  (    ((token.lexeme == "mult") || (token.lexeme == "multu" ) ||
                      (token.lexeme == "div") || (token.lexeme == "divu"))  &&
                     (check_reg_comma_reg (tokLines, line, j))  )
               {labelNumber++;}

            if  (    ((token.lexeme == "sw") || (token.lexeme == "lw" ))  &&
                     (check_reg_comma_intHex_lpar_reg_rpar (tokLines, line, j))  )
               {labelNumber++;}

         }
      }
   }
}
// End Pass #1
//^====================================================================^



//======================================================================
//=======            Pass # 2                                    =======
//======================================================================
//
void pass2 (vector<vector<Token> > &tokLines, map <string, int> &labelMap, int &labelNumber) {
      int validLines = 0;  // for beq and bne label counting
      for(unsigned line=0; line < tokLines.size(); line++ ) {
         for(unsigned j=0; j < tokLines[line].size(); j++ ) {
            Token token = tokLines[line][j];

            // .word
            if ((token.kind == DOTWORD) && (j+1 < tokLines[line].size())) {
               Token nextToken = tokLines[line][j+1];
               if (nextToken.kind == INT || nextToken.kind == HEXINT) {
                  int value = nextToken.toInt();
                  outbyte (value);
                  j += 1;
                  validLines++;
               } else if (nextToken.kind == ID) { // A3P5: labels as operands
                  map <string, int>::iterator it = labelMap.find(nextToken.lexeme);
                  if (it == labelMap.end()) {
                  cerr << "ERROR: Parse error in line: " << line <<
                  ": No such label: '" << nextToken.lexeme << endl;
                  break;
               } else {
                  // since label exists, dereference address and print to binary
                  int value = labelMap[nextToken.lexeme];
                  outbyte (value);
                  j += 1;
                  validLines++;
                  }
               } else {
               cerr << "ERROR Improper .word on (" << line <<
               "), expecting INT or HEXINT or ID" << endl;
               break;
               }
            }  // End .word


            else if (token.kind == ID) {

            // jr, jalr
               if ((token.lexeme == "jr") && (check_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
//                  DBGVAR (cout, d);
                  asm_jr (d);
                  j += 1;
                  validLines++;
               }
               else if ((token.lexeme == "jalr") && (check_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
//                  DBGVAR (cout, d);
                  asm_jalr (d);
                  validLines++;
                  j += 1;
               }
            // End jr, jalr

            // beq, bne
               else if (  ((token.lexeme == "beq") || (token.lexeme == "bne"))
                     && (check_reg_comma_reg_comma_intHexintLabel (tokLines, line, j))
                     ) {
                  int s = tokLines[line][j+1].toInt();
                  int t = tokLines[line][j+3].toInt();
                  int i;
                  if (  (tokLines[line][j+5].kind == ID)
                        && (tokenLabelExists (labelMap, tokLines[line][j+5]))
                        ) {
                     // get label position
                     map <string, int>::iterator it = labelMap.find(tokLines[line][j+5].lexeme);
                     i = (it->second / 4) - validLines - 1;
                  } else if ((tokLines[line][j+5].kind == INT) || (tokLines[line][j+5].kind == HEXINT)) {
                     i = tokLines[line][j+5].toInt();
                  } else {
                     cerr << "ERROR on line " << line << " " << token.lexeme << endl;
                     break;
                  }
//                  DBGVAR (cout, s);
//                  DBGVAR (cout, t);
//                  DBGVAR (cout, i);
                  if ( token.lexeme == "beq" ) {
                     asm_beq (s, t, i);
                  } else {
                     asm_bne (s, t, i);
                  }
                  j += 6;
                  validLines++;
               }
            // End beq, bne


            // add, sub, slt, sltu
               else if ((token.lexeme == "add") && (check_reg_comma_reg_comma_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
                  int s = tokLines[line][j+3].toInt();
                  int t = tokLines[line][j+5].toInt();
                  asm_add (d, s, t);
                  j += 6;
                  validLines++;
               }
               else if ((token.lexeme == "sub") && (check_reg_comma_reg_comma_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
                  int s = tokLines[line][j+3].toInt();
                  int t = tokLines[line][j+5].toInt();
                  asm_sub (d, s, t);
                  j += 6;
                  validLines++;
               }
               else if ((token.lexeme == "slt") && (check_reg_comma_reg_comma_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
                  int s = tokLines[line][j+3].toInt();
                  int t = tokLines[line][j+5].toInt();
                  asm_slt (d, s, t);
                  j += 6;
                  validLines++;
               }
               else if ((token.lexeme == "sltu")  && (check_reg_comma_reg_comma_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
                  int s = tokLines[line][j+3].toInt();
                  int t = tokLines[line][j+5].toInt();
                  asm_sltu (d, s, t);
                  j += 6;
                  validLines++;
               }
            // End add, sub, slt, sltu

            //  lis, mflo, mfhi
               else if ((token.lexeme == "lis") && (check_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
                  asm_lis (d);
                  j += 2;
                  validLines++;
               }
               else if ((token.lexeme == "mflo") && (check_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
                  asm_mflo (d);
                  j += 2;
                  validLines++;
               }
               else if ((token.lexeme == "mfhi") && (check_reg (tokLines, line, j))) {
                  int d = tokLines[line][j+1].toInt();
                  asm_mfhi (d);
                  j += 2;
                  validLines++;
               }
            // End lis, mflo, mfhi

            // mult, multu, div, divu
               else if ((token.lexeme == "mult") && (check_reg_comma_reg (tokLines, line, j))) {
                  int s = tokLines[line][j+1].toInt();
                  int t = tokLines[line][j+3].toInt();
                  asm_mult (s, t);
                  j += 4;
                  validLines++;
               }
               else if ((token.lexeme == "multu") && (check_reg_comma_reg (tokLines, line, j))) {
                  int s = tokLines[line][j+1].toInt();
                  int t = tokLines[line][j+3].toInt();
                  asm_multu (s, t);
                  j += 4;
                  validLines++;
               }
               else if ((token.lexeme == "div") && (check_reg_comma_reg (tokLines, line, j))) {
                  int s = tokLines[line][j+1].toInt();
                  int t = tokLines[line][j+3].toInt();
                  asm_div (s, t);
                  j += 4;
                  validLines++;
               }
               else if ((token.lexeme == "divu")  && (check_reg_comma_reg (tokLines, line, j))) {
                  int s = tokLines[line][j+1].toInt();
                  int t = tokLines[line][j+3].toInt();
                  asm_divu (s, t);
                  j += 4;
                  validLines++;
               }
            // End mult, multu, div, divu

            // sw, lw
               else if ((token.lexeme == "sw") && (check_reg_comma_intHex_lpar_reg_rpar (tokLines, line, j))) {
                  int t =  tokLines[line][j+1].toInt();
                  int i =  tokLines[line][j+3].toInt();
                  int s =  tokLines[line][j+5].toInt();
                  asm_sw (t, i, s);
                  j += 6;
                  validLines++;
               } else if ((token.lexeme == "lw") && (check_reg_comma_intHex_lpar_reg_rpar (tokLines, line, j))) {
                  int t =  tokLines[line][j+1].toInt();
                  int i =  tokLines[line][j+3].toInt();
                  int s =  tokLines[line][j+5].toInt();
//               DBGVAR (cout, s);
//               DBGVAR (cout, t);
//               DBGVAR (cout, i);
                  asm_lw (t, i, s);
                  j += 6;
                  validLines++;
               }
            // End sw, lw

               else {
                  cerr << "ERROR on line " << line << ": " << token.lexeme << endl;
                  break;
               }

            } // End else if (token.kind == ID)
            else if (token.kind == REGISTER){
               cerr << "ERROR on line " << line << ": " << token.lexeme << endl;
               break;
            }
         } // End for loop of (word) in each (line)
      } // End for loop of (line)
}
// End Pass #2
//^====================================================================^








//======================================================================
//======= CS241 MIPS Compiler: Main program  Author: Jason Sun   =======
//======================================================================

int main() {

   try {
      vector<string> srcLines;

      // Read the entire input file, storing each line as a
      // single string in the array srcLines.
      while(true) {
      string line;
      getline(cin, line);
      if(cin.fail()) break;
      srcLines.push_back(line);
      }

      // Tokenize each line, storing the results in tokLines.
      vector<vector<Token> > tokLines;
      for(unsigned line = 0; line < srcLines.size(); line++) {
      tokLines.push_back(scan(srcLines[line]));
      }

      // containers to keep track of label and line association
      int labelNumber = 0;
      map <string, int> labelMap;

      // Pass #1, grab all the labels into map
      pass1 (tokLines, labelMap, labelNumber);

      // Print the map - a helper procedure for debugging
//      displayLabels (labelMap);

      // Pass #2, translate and output into machine binary
      pass2 (tokLines, labelMap, labelNumber);

   } catch(string msg) {
     cerr << msg << endl;
   }
   return 0;
}

// End main
//^====================================================================^
