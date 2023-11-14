#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "scanner.h"

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 */

// output functions
void out_put(int num) {
    unsigned char c = num >> 24;
    std::cout << c;
    c = num >> 16;
    std::cout << c;
    c = num >> 8;
    std::cout << c;
    c = num;
    std::cout << c;
}

void output_jr(int s) {
    out_put((s<<21)|8);
}

void output_jalr(int s) {
    out_put((s<<21)|9);
}

void output_add(int d, int s, int t) {
    out_put((s<<21)|(t<<16)|(d<<11)|32);
}

void output_sub(int d, int s, int t) {
    out_put((s<<21)|(t<<16)|(d<<11)|34);
}

void output_slt(int d, int s, int t) {
    out_put((s<<21)|(t<<16)|(d<<11)|42);
}

void output_sltu(int d, int s, int t) {
    out_put((s<<21)|(t<<16)|(d<<11)|43);
}

void output_beq(int s, int t, int16_t i) {
    out_put(((1<<28)|(s<<21)|(t<<16))|((0x0000ffff)&i));
}

void output_bne(int s, int t, int16_t i) {
    out_put(((5<<26)|(s<<21)|(t<<16))|((0x0000ffff)&i));
}

void output_lis(int d) {
    out_put((d<<11)|20);
}

void output_mflo(int d) {
    out_put((d<<11)|18);
}

void output_mfhi(int d) {
    out_put((d<<11)|16);
}

void output_mult(int s, int t) {
    out_put((s<<21)|(t<<16)|24);
}

void output_multu(int s, int t) {
    out_put((s<<21)|(t<<16)|25);
}

void output_div(int s, int t) {
    out_put((s<<21)|(t<<16)|26);
}

void output_divu(int s, int t) {
    out_put((s<<21)|(t<<16)|27);
}

void output_lw(int t, int16_t i, int s) {
    out_put((35<<26)|(s<<21)|(t<<16)|((0x0000ffff)&i));
}

void output_sw(int t, int16_t i, int s) {
    out_put((43<<26)|(s<<21)|(t<<16)|((0x0000ffff)&i));
}


// some checking functions
bool int_range16(int x) {
    return (x >= -32768 && x <= 32767);
}

bool hex_range16(unsigned int x) {
    return (x <= 65535);
}

bool lable_exist(std::map<std::string, int> &label_map, std::string label) {
    std::map<std::string, int>::iterator it = label_map.find(label);
    return (it!=label_map.end());
}

bool checking_reg(std::vector<std::vector<Token>> &tokenLines, unsigned int line_num, unsigned int i) {
    if(tokenLines[line_num].size() < i + 2)
        return false;
    if(tokenLines[line_num].size() == i + 2) {
        Token t = tokenLines[line_num][i+1];
        if(t.getKind() == Token::REG)
            return true;
        else
            return false;
    }
    if(tokenLines[line_num][i+2].getKind() == Token::COMMENT)
        return true;
    return false;
}

bool checking_reg_comma_reg_comma_reg(std::vector<std::vector<Token>> &tokenLines, unsigned int line_num, unsigned int i) {
    if(tokenLines[line_num].size() < i + 6)
        return false;
    Token t1 = tokenLines[line_num][i+1];
    Token t2 = tokenLines[line_num][i+2];
    Token t3 = tokenLines[line_num][i+3];
    Token t4 = tokenLines[line_num][i+4];
    Token t5 = tokenLines[line_num][i+5];
    if(tokenLines[line_num].size() == i + 6) {
        return ((t1.getKind() == Token::REG && t1.toNumber() >= 1 && t1.toNumber() < 32) &&
                (t2.getKind() == Token::COMMA) &&
                (t3.getKind() == Token::REG && t3.toNumber() >= 0 && t3.toNumber() < 32) &&
                (t4.getKind() == Token::COMMA) &&
                (t5.getKind() == Token::REG && t5.toNumber() >= 0 && t5.toNumber() < 32));
    }
    if(tokenLines[line_num][i+6].getKind() == Token::COMMENT)
        return ((t1.getKind() == Token::REG && t1.toNumber() >= 1 && t1.toNumber() < 32) &&
                (t2.getKind() == Token::COMMA) &&
                (t3.getKind() == Token::REG && t3.toNumber() >= 0 && t3.toNumber() < 32) &&
                (t4.getKind() == Token::COMMA) &&
                (t5.getKind() == Token::REG && t5.toNumber() >= 0 && t5.toNumber() < 32));
    return false;
}

bool checking_reg_comma_reg_comma_num(std::vector<std::vector<Token>> &tokenLines, unsigned int line_num, unsigned int i) {
    if(tokenLines[line_num].size() < i + 6)
        return false;
    Token t1 = tokenLines[line_num][i+1];
    Token t2 = tokenLines[line_num][i+2];
    Token t3 = tokenLines[line_num][i+3];
    Token t4 = tokenLines[line_num][i+4];
    Token t5 = tokenLines[line_num][i+5];
    if(tokenLines[line_num].size() == i + 6) {
        return (t1.getKind() == Token::REG && t2.getKind() == Token::COMMA && t3.getKind() == Token::REG &&t4.getKind() == Token::COMMA &&
                ((t5.getKind() == Token::INT && int_range16(t5.toNumber())) || (t5.getKind() == Token::HEXINT && hex_range16(t5.toNumber()))
                                                                           || t5.getKind() == Token::ID));
    }
    if(tokenLines[line_num][i+6].getKind() == Token::COMMENT)
        return (t1.getKind() == Token::REG && t2.getKind() == Token::COMMA && t3.getKind() == Token::REG &&t4.getKind() == Token::COMMA &&
                ((t5.getKind() == Token::INT && int_range16(t5.toNumber())) || (t5.getKind() == Token::HEXINT && hex_range16(t5.toNumber()))
                                                                           || t5.getKind() == Token::ID));
    return false;
}

bool checking_reg_comma_reg(std::vector<std::vector<Token>> &tokenLines, unsigned int line_num, unsigned int i) {
    if(tokenLines[line_num].size() < i + 4)
        return false;
    Token t1 = tokenLines[line_num][i+1];
    Token t2 = tokenLines[line_num][i+2];
    Token t3 = tokenLines[line_num][i+3];
    if(tokenLines[line_num].size() == i + 4) {
        return (t1.getKind() == Token::REG && t2.getKind() == Token::COMMA && t3.getKind() == Token::REG);
    }
    if(tokenLines[line_num][i+4].getKind() == Token::COMMENT)
        return (t1.getKind() == Token::REG && t2.getKind() == Token::COMMA && t3.getKind() == Token::REG);
    return false;
}

bool checking_reg_comma_num_lp_reg_rp(std::vector<std::vector<Token>> &tokenLines, unsigned int line_num, unsigned int i) {
    if(tokenLines[line_num].size() < i + 7)
        return false;
    Token t1 = tokenLines[line_num][i+1];
    Token t2 = tokenLines[line_num][i+2];
    Token t3 = tokenLines[line_num][i+3];
    Token t4 = tokenLines[line_num][i+4];
    Token t5 = tokenLines[line_num][i+5];
    Token t6 = tokenLines[line_num][i+6];
    if(tokenLines[line_num].size() == i + 7) {
        return (t1.getKind() == Token::REG && t2.getKind() == Token::COMMA &&
                (((t3.getKind() == Token::INT && int_range16(t3.toNumber())) ||
                  (t3.getKind() == Token::HEXINT && hex_range16(t3.toNumber())))) &&
                t4.getKind() == Token::LPAREN && t5.getKind() == Token::REG && t6.getKind() == Token::RPAREN);
    }
    if(tokenLines[line_num][i+4].getKind() == Token::COMMENT)
        return (t1.getKind() == Token::REG && t2.getKind() == Token::COMMA &&
                (((t3.getKind() == Token::INT && int_range16(t3.toNumber())) ||
                  (t3.getKind() == Token::HEXINT && hex_range16(t3.toNumber())))) &&
                t4.getKind() == Token::LPAREN && t5.getKind() == Token::REG && t6.getKind() == Token::RPAREN);
    return false;
}

int main() {
  std::string line;
  try {
    std::vector<std::vector<Token>> tokenLines;
    while (getline(std::cin, line)) {
        tokenLines.push_back(scan(line));
    }
    int num_lable = 0;
    std::map<std::string, int> lable_map;
    // first time go through to map the labels
    for(unsigned int line_num=0; line_num<tokenLines.size(); ++line_num) {
        for(unsigned int i = 0; i < tokenLines[line_num].size(); ++i) {
            Token t = tokenLines[line_num][i];
            if (t.getKind() == Token::WORD && i+1 < tokenLines[line_num].size()){
                Token nxt = tokenLines[line_num][i+1];
                if(nxt.getKind() == Token::INT ||nxt.getKind() == Token::HEXINT || nxt.getKind() == Token::ID)
                    ++num_lable;
            } else if (t.getKind() == Token::LABEL) {
                std::string lable_without_colon = t.getLexeme().substr(0,t.getLexeme().length()-1);
                std::map<std::string, int>::iterator it = lable_map.find(lable_without_colon);
                if(it == lable_map.end() && ((i>=1 && tokenLines[line_num][i-1].getKind()==Token::LABEL)|| i == 0)) {
                    lable_map[lable_without_colon] = 4 * num_lable;
                } else {
                    std::cerr << "ERROR occur when line_num = " << line_num << ", " << t.getLexeme() << "is defined already." << std::endl;
                    return 2;
                }
            }
            else if(t.getKind() == Token::ID) {
                if(t.getLexeme() == "jr" || t.getLexeme() == "jalr") {
                    if(checking_reg(tokenLines, line_num, i))
                        ++num_lable;
                } else if(t.getLexeme() == "add" || t.getLexeme() == "sub" || t.getLexeme() == "slt" || t.getLexeme() == "sltu") {
                    if(checking_reg_comma_reg_comma_reg(tokenLines, line_num, i))
                        ++num_lable;
                } else if(t.getLexeme() == "beq" || t.getLexeme() == "bne") {
                    if(checking_reg_comma_reg_comma_num(tokenLines, line_num, i))
                        ++num_lable;
                } else if(t.getLexeme() == "lis" || t.getLexeme() == "mflo" || t.getLexeme() == "mfhi") {
                    if(checking_reg(tokenLines, line_num, i))
                        ++num_lable;
                } else if(t.getLexeme() == "mult" || t.getLexeme() == "multu" || t.getLexeme() == "div" || t.getLexeme() == "divu") {
                    if(checking_reg_comma_reg(tokenLines, line_num, i))
                        ++num_lable;
                } else if(t.getLexeme() == "sw" || t.getLexeme() == "lw") {
                    if(checking_reg_comma_num_lp_reg_rp(tokenLines, line_num, i))
                        ++num_lable;
                }
            }
        }

    }

    int valid_counter = 0;
    // second time go through to produce output
    for(unsigned int line_num=0; line_num<tokenLines.size(); ++line_num) {
        for(unsigned int i = 0; i < tokenLines[line_num].size(); ++i) {
            Token t = tokenLines[line_num][i];

            if (t.getKind() == Token::WORD && i+1 < tokenLines[line_num].size()){
                Token nxt = tokenLines[line_num][i+1];
                if(nxt.getKind() == Token::INT || nxt.getKind() == Token::HEXINT) {
                    int64_t num = nxt.toNumber();
                    if(nxt.getKind() == Token::INT && !(num <= 4294967295 && num >= -4294967296))
                    {
                        std::cerr << "ERROR! Decimal number out of bound" << std::endl;
                        break;
                    }
                    if(nxt.getKind() == Token::HEXINT && num > 424967295) {
                        std::cerr << "ERROR! Hexadecimal number out of bound" << std::endl;
                        break;
                    }
                    out_put(num);
                    ++i;
                    ++valid_counter;
                } else if (nxt.getKind() == Token::ID) {
                    std::map<std::string, int>::iterator it =lable_map.find(nxt.getLexeme());
                    if(it != lable_map.end()) {
                        int addr = lable_map[nxt.getLexeme()];
                        out_put(addr);
                        ++i;
                        ++valid_counter;
                    } else {
                        std::cerr << "ERROR! Couldn't find label " << nxt.getLexeme() << std::endl;
                        return 2;
                    }
                }

            } else if (t.getKind() == Token::LABEL) {
                continue;
            } else if (t.getKind() == Token::COMMENT) {
                break;
            } else if (t.getKind() == Token::ID) {
                if(t.getLexeme() == "jr" && checking_reg(tokenLines, line_num, i)) {
                    int val = tokenLines[line_num][i+1].toNumber();
                    output_jr(val);
                    ++i;
                    ++valid_counter;
                } else if (t.getLexeme() == "jalr" && checking_reg(tokenLines, line_num, i)) {
                    int val = tokenLines[line_num][i+1].toNumber();
                    output_jalr(val);
                    ++i;
                    ++valid_counter;
                }

                else if(t.getLexeme() == "add" && checking_reg_comma_reg_comma_reg(tokenLines, line_num, i)) {
                    int d = tokenLines[line_num][i+1].toNumber();
                    int s = tokenLines[line_num][i+3].toNumber();
                    int tt = tokenLines[line_num][i+5].toNumber();
                    output_add(d,s,tt);
                    i+=5;
                    ++valid_counter;
                } else if(t.getLexeme() == "sub" && checking_reg_comma_reg_comma_reg(tokenLines, line_num, i)) {
                    int d = tokenLines[line_num][i+1].toNumber();
                    int s = tokenLines[line_num][i+3].toNumber();
                    int tt = tokenLines[line_num][i+5].toNumber();
                    output_sub(d,s,tt);
                    i+=5;
                    ++valid_counter;
                } else if (t.getLexeme() == "slt" && checking_reg_comma_reg_comma_reg(tokenLines, line_num, i)) {
                    int d = tokenLines[line_num][i+1].toNumber();
                    int s = tokenLines[line_num][i+3].toNumber();
                    int tt = tokenLines[line_num][i+5].toNumber();
                    output_slt(d,s,tt);
                    i+=5;
                    ++valid_counter;
                } else if (t.getLexeme() == "sltu" &&checking_reg_comma_reg_comma_reg(tokenLines, line_num, i)) {
                    int d = tokenLines[line_num][i+1].toNumber();
                    int s = tokenLines[line_num][i+3].toNumber();
                    int tt = tokenLines[line_num][i+5].toNumber();
                    output_sltu(d,s,tt);
                    i+=5;
                    ++valid_counter;
                }

                else if((t.getLexeme() == "beq" || t.getLexeme() == "bne") && checking_reg_comma_reg_comma_num(tokenLines, line_num, i)) {
                    int s = tokenLines[line_num][i+1].toNumber();
                    int tt = tokenLines[line_num][i+3].toNumber();
                    Token temp = tokenLines[line_num][i+5];
                    int ii;
                    if(temp.getKind() == Token::ID && lable_exist(lable_map, temp.getLexeme())) {
                        std::map<std::string, int>::iterator it = lable_map.find(temp.getLexeme());
                        ii = (it->second)/4 - valid_counter - 1;
                    } else if(temp.getKind() == Token::INT || temp.getKind() == Token::HEXINT) {
                        ii = temp.toNumber();
                    } else {
                        std::cerr << "ERROR! occured on beq or bne : line " << line_num << std::endl;
                        break;
                    }
                    if(t.getLexeme() == "beq") {
                        output_beq(s,tt,ii);
                        i+=5;
                        ++valid_counter;
                    } else if(t.getLexeme() == "bne") {
                        output_bne(s,tt,ii);
                        i+=5;
                        ++valid_counter;
                    }
                }

                else if(t.getLexeme() == "lis" && checking_reg(tokenLines, line_num, i)) {
                    int d = tokenLines[line_num][i+1].toNumber();
                    output_lis(d);
                    ++i;
                    ++valid_counter;
                } else if(t.getLexeme() == "mflo" && checking_reg(tokenLines, line_num, i)) {
                    int d = tokenLines[line_num][i+1].toNumber();
                    output_mflo(d);
                    ++i;
                    ++valid_counter;
                } else if(t.getLexeme() == "mfhi" && checking_reg(tokenLines, line_num, i)) {
                    int d = tokenLines[line_num][i+1].toNumber();
                    output_mfhi(d);
                    ++i;
                    ++valid_counter;
                }

                else if(t.getLexeme() == "mult" && checking_reg_comma_reg(tokenLines, line_num, i)) {
                    int s = tokenLines[line_num][i+1].toNumber();
                    int tt = tokenLines[line_num][i+3].toNumber();
                    output_mult(s,tt);
                    i+=3;
                    ++valid_counter;
                } else if(t.getLexeme() == "multu" && checking_reg_comma_reg(tokenLines, line_num, i)) {
                    int s = tokenLines[line_num][i+1].toNumber();
                    int tt = tokenLines[line_num][i+3].toNumber();
                    output_multu(s,tt);
                    i+=3;
                    ++valid_counter;
                } else if(t.getLexeme() == "div" && checking_reg_comma_reg(tokenLines, line_num, i)) {
                    int s = tokenLines[line_num][i+1].toNumber();
                    int tt = tokenLines[line_num][i+3].toNumber();
                    output_div(s,tt);
                    i+=3;
                    ++valid_counter;
                } else if(t.getLexeme() == "divu" && checking_reg_comma_reg(tokenLines, line_num, i)) {
                    int s = tokenLines[line_num][i+1].toNumber();
                    int tt = tokenLines[line_num][i+3].toNumber();
                    output_divu(s,tt);
                    i+=3;
                    ++valid_counter;
                }

                else if(t.getLexeme() == "sw" && checking_reg_comma_num_lp_reg_rp(tokenLines, line_num, i)) {
                    int tt = tokenLines[line_num][i+1].toNumber();
                    int ii = tokenLines[line_num][i+3].toNumber();
                    int s = tokenLines[line_num][i+5].toNumber();
                    output_sw(tt,ii,s);
                    i+=6;
                    ++valid_counter;
                } else if(t.getLexeme() == "lw" && checking_reg_comma_num_lp_reg_rp(tokenLines, line_num, i)) {
                    int tt = tokenLines[line_num][i+1].toNumber();
                    int ii = tokenLines[line_num][i+3].toNumber();
                    int s = tokenLines[line_num][i+5].toNumber();
                    output_lw(tt,ii,s);
                    i+=6;
                    ++valid_counter;
                }



                else {
                    std::cerr << "ERROR occur (Identified as ID) on line " << line_num << " " << t.getLexeme() << std::endl;
                    break;
                }
            } else {
                std::cerr << "ERROR occur on line " << line_num << " " << t.getLexeme() << std::endl;
                return 2;
            }
        }

    }

  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  return 0;
}
