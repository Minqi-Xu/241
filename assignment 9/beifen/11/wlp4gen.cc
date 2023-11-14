#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <utility>

using namespace std;

ifstream file{"WLP4g+pt"};
int dcl_count = 0;
bool contain_print = false;
bool contain_new = false;
int while_label_count = 0;
int if_lable_count = 0;
int skip_delete_count = 0;

struct grammar {        // the structure of grammar
    int num_terminal;       // number of terminals
    int num_nonterminal;    // number of nonterminals
    int num_rules;          // number of rules
    string s;               // starting symbol
    vector<string> terminal;
    vector<string> nonterminal;
    vector<vector<string>> rules;
};

void getGrammar(grammar* G) {       // input and store the grammar
    string lineStr;
    // getting terminals
    file >> G->num_terminal;
    for(int i = 0; i < G->num_terminal; ++i) {
        string input;
        file >> input;
        G->terminal.push_back(input);
    }
    // getting nonterminals
    file >> G->num_nonterminal;
    for(int i = 0; i < G->num_nonterminal; ++i) {
        string input;
        file >> input;
        G->nonterminal.push_back(input);
    }
    // getting starting symbol
    file >> G->s;
    // getting rules
    file >> G->num_rules;
    string ignore;
    getline(file,ignore);
    for(int i = 0; i < G->num_rules; ++i) {
        vector<string> rule;
        getline(file, lineStr);
        stringstream line(lineStr);
        string nxt;
        while(line >> nxt) {
            rule.push_back(nxt);
        }
        G->rules.push_back(rule);
    }
}

bool isterminal(string s, grammar* G) {
    for(int i = 0; i < G->num_terminal; ++i) {
        if(s == G->terminal[i])
            return true;
    }
    return false;
}

struct treenode{
    string type_of;
    vector<string> info;
    vector<treenode> children;
};

void get_input(treenode* n, grammar* G) {
    string lineStr;
    getline(cin, lineStr);
    stringstream line(lineStr);
    string nxt;
    while(line >> nxt) {
        n->info.push_back(nxt);
    }

    if(isterminal(n->info[0] , G)) return;

    for(unsigned i = 0; i < n->info.size() - 1; i++) {
        treenode child;
        n->children.push_back(child);
        get_input(&(n->children[i]), G);
    }
}

void get_param_type(vector<string>* param, treenode* n) {
    if(n->info.size() == 1) return;
    if(n->info[0] == "COMMA") return;
    if(n->info.size() >= 3 && n->info[0] == "dcl" && n->info[1] == "type" && n->info[2] == "ID") {
        if(n->children[0].info.size() == 3)
            param->push_back("int*");
        else
            param->push_back("int");
        return;
    }
    for(unsigned i = 0; i < n->children.size(); i++) {
        get_param_type(param, &n->children[i]);
    }
}

string type(treenode*, map<string,pair<vector<string>, map<string, pair<string, int>>>>*, string);

void get_arg_type(vector<string>* param, treenode* n, map<string,pair<vector<string>, map<string, pair<string,int>>>>* table, string current) {
    if(n->info[0] == "expr") {
        param->push_back(type(n, table, current));
        return;
    }
    if(n->info[0] == "COMMA") return;
    for(unsigned i = 0; i <n->children.size(); i++) {
        get_arg_type(param, &n->children[i], table, current);
    }
}

string type(treenode* n, map<string,pair<vector<string>, map<string, pair<string,int>>>>* table, string current) {
    if(n->info.size() == 2 && n->info[0] == "expr" && n->info[1] == "term") {
        string tempo = type(&n->children[0], table, current);
        n->type_of = tempo;
        return tempo;
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "PLUS"
         && n->info[3] == "term") {
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int") {
            n->type_of = "int";
            return "int";
        }
        if(type(&n->children[0], table, current) == "int*" && type(&n->children[2], table, current) == "int") {
            n->type_of = "int*";
            return "int*";
        }
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int*") {
            n->type_of = "int*";
            return "int*";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "MINUS"
         && n->info[3] == "term") {
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int") {
            n->type_of = "int";
            return "int";
        }
        if(type(&n->children[0], table, current) == "int*" && type(&n->children[2], table, current) == "int") {
            n->type_of = "int*";
            return "int*";
        }
        if(type(&n->children[0], table, current) == "int*" && type(&n->children[2], table, current) == "int*") {
            n->type_of = "int";
            return "int";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 2 && n->info[0] == "term" && n->info[1] == "factor") {
        string tempo = type(&n->children[0], table, current);
        n->type_of = tempo;
        return tempo;
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "STAR"
         && n->info[3] == "factor") {
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int") {
            n->type_of = "int";
            return "int";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "SLASH"
         && n->info[3] == "factor") {
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int") {
            n->type_of = "int";
            return "int";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "PCT"
         && n->info[3] == "factor") {
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int") {
            n->type_of = "int";
            return "int";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "ID") {
        string tempo = (*table)[current].second[n->children[0].info[1]].first;
        n->type_of = tempo;
        return tempo;
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NUM") {
        n->type_of = "int";
        return "int";
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NULL") {
        n->type_of = "int*";
        return "int*";
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "LPAREN" && n->info[2] == "expr"
         && n->info[3] == "RPAREN") {
        string tempo = type(&n->children[1], table, current);
        n->type_of = tempo;
        return tempo;
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "AMP" && n->info[2] == "lvalue") {
        if(type(&n->children[1], table, current) == "int")  {
            n->type_of = "int *";
            return "int*";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "STAR" && n->info[2] == "factor") {
        if(type(&n->children[1], table, current) == "int*")  {
                n->type_of = "int";
                return "int";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 6 && n->info[0] == "factor" && n->info[1] == "NEW" && n->info[2] == "INT"
         && n->info[3] == "LBRACK" && n->info[4] == "expr" && n->info[5] == "RBRACK") {
        contain_new = true;
        if(type(&n->children[3], table, current) == "int") {
                n->type_of = "int*";
                return "int*";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 2 && n->info[0] == "lvalue" && n->info[1] == "ID") {
        string tempo = (*table)[current].second[n->children[0].info[1]].first;
        n->type_of = tempo;
        return tempo;
    }
    else if(n->info.size() == 3 && n->info[0] == "lvalue" && n->info[1] == "STAR" && n->info[2] == "factor") {
        if(type(&n->children[1], table, current) == "int*") {
                n->type_of = "int";
                return "int";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "lvalue" && n->info[1] == "LPAREN" && n->info[2] == "lvalue"
         && n->info[3] == "RPAREN") {
        string tempo = type(&n->children[1], table, current);
        n->type_of = tempo;
        return tempo;
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN"
         && n->info[3] == "RPAREN") {
        if((*table)[n->children[0].info[1]].first.size() == 0)  {
                n->type_of = "int";
                return "int";
        }
        n->type_of = "error";
        return "error";
    }
    else if(n->info.size() == 5 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN"
                                && n->info[3] == "arglist" && n->info[4] == "RPAREN") {
        vector<string> arg_type;
        get_arg_type(&arg_type, &n->children[2], table, current);
        if(arg_type.size() != (*table)[n->children[0].info[1]].first.size())    return "error";
        for(unsigned i = 0; i < arg_type.size(); i++) {
            if(arg_type[i] != (*table)[n->children[0].info[1]].first[i])    return "error";
        }
        n->type_of = "int";
        return "int";
    }
    cerr << "ERROR: type find false:" << endl;
    for(unsigned i = 0; i < n->info.size(); i++) {
        cerr << n->info[i] << " ";
    }
    cerr << endl;
    n->type_of = "error";
    return "error";
}

void tranverse(treenode* n, map<string,pair<vector<string>, map<string, pair<string, int>>>>* table, string current) {
    if(n->info[0] == "main") {
        dcl_count = 0;
        pair<vector<string> ,map<string, pair<string,int>>> main_table;
        (*table)["wain"] = main_table;
        current = "wain";
        if(n->children[3].children[0].info.size() == 3) {
            (*table)["wain"].first.push_back("int*");
        } else {
            (*table)["wain"].first.push_back("int");
        }
        if(n->children[5].children[0].info.size() == 3) {
            (*table)["wain"].first.push_back("int*");
        } else {
            (*table)["wain"].first.push_back("int");
        }
    }

    else if(n->info[0] == "procedure") {
        dcl_count = 0;
        pair<vector<string> ,map<string, pair<string, int>>> pro_table;
        current = n->children[1].info[1];
        map<string,pair<vector<string>, map<string, pair<string, int>>>>::iterator it = (*table).find(current);
        if(it != (*table).end()) {
            cerr << "ERROR! procedure declared more than once: " << current << endl;
            return;
        }
        (*table)[current] = pro_table;
        get_param_type(&(*table)[current].first, &n->children[3]);
    }

    else if(n->info.size() >= 3 && n->info[0] == "dcl" && n->info[1] == "type" && n->info[2] == "ID") {
        map<string, pair<string, int>>::iterator it = (*table)[current].second.find(n->children[1].info[1]);
        if(it != (*table)[current].second.end()) {
            cerr << "ERROR! declared more than once: " << n->children[1].info[1] << endl;
            return;
        }
        if(n->children[0].info.size() == 3) {
            (*table)[current].second[n->children[1].info[1]].first = "int*";
        } else {
            (*table)[current].second[n->children[1].info[1]].first = "int";
        }
        (*table)[current].second[n->children[1].info[1]].second = 0 - (dcl_count * 4);
        dcl_count++;
        return;
    } else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "ID") {
        map<string, pair<string,int>>::iterator it = (*table)[current].second.find(n->children[0].info[1]);
        if(it == (*table)[current].second.end()) {
            cerr << "ERROR! using before declare: " << n->children[0].info[1] << endl;
            return;
        }
        return;
    } else if(n->info.size() >= 2 && n->info[0] == "lvalue" && n->info[1] == "ID") {
        map<string, pair<string, int>>::iterator it = (*table)[current].second.find(n->children[0].info[1]);
        if(it == (*table)[current].second.end()) {
            cerr << "ERROR! using before declare: " << n->children[0].info[1] << endl;
            return;
        }
        return;
    }

    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN" && n->info[3] == "RPAREN") {
        map<string,pair<vector<string>, map<string, pair<string,int>>>>::iterator it = (*table).find(n->children[0].info[1]);
        if(it == (*table).end()) {
            cerr << "ERROR! procedure using before decalre: " << n->children[0].info[1] << endl;
            return;
        }
    } else if (n->info.size() == 5 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN"
                                   && n->info[3] == "arglist" && n->info[4] == "RPAREN") {
        map<string,pair<vector<string>, map<string, pair<string, int>>>>::iterator it = (*table).find(n->children[0].info[1]);
        if(it == (*table).end()) {
            cerr << "ERROR! procedure using before decalre: " << n->children[0].info[1] << endl;
            return;
        }
    }


    for(unsigned i = 0; i < n->children.size(); i++)
        tranverse(&n->children[i], table, current);
}

void check_type_tree(treenode* n, map<string,pair<vector<string>, map<string, pair<string, int>>>>* table, string current) {
    if(n->info[0] == "main") {
        current = "wain";
        if(n->children[5].children[0].info.size() == 3) {
            cerr << "ERROR! the arguments of main function false" << endl;
            return;
        }
        if(type(&n->children[11], table, current) != "int") {
            cerr << "ERROR! main function should return int type!" << endl;
            return;
        }
    }

    else if(n->info[0] == "procedure") {
        current = n->children[1].info[1];
        if(type(&n->children[9], table, current) != "int") {
            cerr << "ERROR! procedure should return int type: " << current << endl;
            return;
        }
    }

    else if(n->info.size() == 6 && n->info[0] == "dcls" && n->info[1] == "dcls" && n->info[2] == "dcl" && n->info[3] == "BECOMES"
                                && n->info[4] == "NUM" && n->info[5] == "SEMI") {
        if(n->children[1].children[0].info.size() == 3) {
            cerr << "ERROR! declaration false: " << endl;
            for(unsigned i = 0; i < n->info.size(); i++)
                cerr << n->info[i] << " ";
            cerr << endl;
            return;
        }
    }

    else if(n->info.size() == 6 && n->info[0] == "dcls" && n->info[1] == "dcls" && n->info[2] == "dcl" && n->info[3] == "BECOMES"
                                && n->info[4] == "NULL" && n->info[5] == "SEMI") {
        if(n->children[1].children[0].info.size() == 2) {
            cerr << "ERROR! declaration false: " << endl;
            for(unsigned i = 0; i < n->info.size(); i++)
                cerr << n->info[i] << " ";
            cerr << endl;
            return;
        }
    }

    else if(n->info.size() == 5 && n->info[0] == "statement" && n->info[1] == "lvalue" && n->info[2] == "BECOMES"
                                && n->info[3] == "expr" && n->info[4] == "SEMI") {
        if(type(&n->children[0], table, current) != type(&n->children[2], table, current)) {
            cerr << "ERROR! assignment false: " << endl;
            for(unsigned i = 0; i < n->info.size(); i++)
                cerr << n->info[i] << " ";
            cerr << endl;
            return;
        }
    }

    else if(n->info.size() == 6 && n->info[1] == "PRINTLN") {
        contain_print = true;
        if(type(&n->children[2], table, current) != "int") {
            cerr << "ERROR! print error: " << endl;
            for(unsigned i = 0; i < n->info.size(); i++)
                cerr << n->info[i] << " ";
            cerr << endl;
            return;
        }
    }

    else if(n->info.size() == 6 && n->info[1] == "DELETE") {
        if(type(&n->children[3], table, current) != "int*") {
            cerr << "ERROR! deallocate error: " << endl;
            for(unsigned i = 0; i < n->info.size(); i++)
                cerr << n->info[i] << " ";
            cerr << endl;
            return;
        }
    }

    else if(n->info.size() == 4 && n->info[0] == "test") {
        if(type(&n->children[0], table, current) != type(&n->children[2], table, current)) {
            cerr << "ERROR! comparison error: " << endl;
            for(unsigned i = 0; i < n->info.size(); i++)
                cerr << n->info[i] << " ";
            cerr << endl;
            return;
        }
    }
    for(unsigned i = 0; i < n->children.size(); i++)
        check_type_tree(&n->children[i], table, current);
}

void print_table(map<string ,pair<vector<string>, map<string, pair<string, int>>>>* table) {
    map<string ,pair<vector<string>, map<string, pair<string, int>>>>::iterator it;
    for(it = (*table).begin(); it != (*table).end(); ++it) {
        cerr << it->first << ": ";
        for(unsigned i = 0; i < it->second.first.size(); i++) {
            cerr << it->second.first[i] << " ";
        }
        cerr << endl;
        map<string, pair<string,int>>::iterator itt;
        for(itt = it->second.second.begin(); itt != it->second.second.end(); ++itt) {
            cerr << itt->first << " " << itt->second.first << " " << itt->second.second << endl;
        }
    }
}


// ------------------------------------------------------- code generation

void code(string var, string current_pro, map<string ,pair<vector<string>, map<string, pair<string, int>>>>* table) {
    cout << "lw $3, " << (*table)[current_pro].second[var].second << "($29)" << endl;
}

void push(int x) {
    cout << "sw $" << x << ", -4($30)" << endl;
    cout << "sub $30, $30, $4" << endl;
}

void pop(int x) {
    cout << "add $30, $30, $4" << endl;
    cout << "lw $" << x << ", -4($30)" << endl;
}

string find_lexeme(treenode* n, map<string ,pair<vector<string>, map<string, pair<string, int>>>>* table, string current) {
    if(n->info.size() == 2 && n->info[0] == "lvalue" && n->info[1] == "ID")  return n->children[0].info[1];
    // otherwise, it must be lvalue -> LPAREN lvalue RPAREN
    return find_lexeme(&n->children[1], table, current);
}

bool isID(grammar* G, treenode* n, map<string ,pair<vector<string>, map<string, pair<string, int>>>>* table, string current) {
    if(n->info.size() == 2 && n->info[1] == "ID")   return true;
    else if(n->info.size() == 3 && n->info[1] == "STAR")    return false;
    else return isID(G, &n->children[1], table, current);
}

void generate_tranverse(grammar* G, treenode* n, map<string ,pair<vector<string>, map<string, pair<string, int>>>>* table, string current) {
    if(isterminal(n->info[0], G))
        return;
    else if(n->info[0] == "main") {
        current = "wain";
        cout << "; generator reach the main function" << endl;
        if(contain_new) {
            push(2);
            if(n->children[3].children[0].info.size() == 2) {
                cout << "add $2, $0, $0" << endl;
            }
            push(31);
            cout << "lis $5" << endl;
            cout << ".word init" << endl;
            cout << "jalr $5" << endl;
            pop(31);
            pop(2);
        }

        cout << "sub $29, $30, $4" << endl;
        push(1);
        push(2);
        generate_tranverse(G, &(n->children[8]), table, current);
        generate_tranverse(G, &(n->children[9]), table, current);
        generate_tranverse(G, &(n->children[11]), table, current);
        cout << "; begin epilogue" << endl;
        for(unsigned i = 0; i < (*table)[current].second.size(); i++) {
            cout << "add $30, $30, $4" << endl;
        }
        cout << "jr $31" << endl;
    }
    else if(n->info[0] == "start") {
        generate_tranverse(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "procedures") {
        generate_tranverse(G, &n->children[0], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "type" && n->info[1] == "INT") {
        return;
    }
    else if(n->info.size() == 2 && n->info[0] == "lvalue" && n->info[1] == "ID") {
        return;         // will modify in a9
    }
    else if(n->info.size() == 3 && n->info[0] == "lvalue" && n->info[1] == "STAR" && n->info[2] == "factor") {
        generate_tranverse(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 4 && n->info[0] == "lvalue" && n->info[1] == "LPAREN" && n->info[2] == "lvalue"
         && n->info[3] == "RPAREN") {
        generate_tranverse(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 6 && n->info[0] == "dcls" && n->info[1] == "dcls" && n->info[2] == "dcl"
         && n->info[3] == "BECOMES" && n->info[4] == "NUM" && n->info[5] == "SEMI") {
        generate_tranverse(G, &n->children[0], table, current);
        generate_tranverse(G, &n->children[1], table, current);
        if(n->children[1].children[1].info.size() == 2) {   // dcl an int
            cout << "lis $3" << endl;
            cout << ".word " << n->children[3].info[1] << endl;
            cout << "sw $3, " << (*table)[current].second[n->children[1].children[1].info[1]].second << "($29)" <<endl;
            cout << "sub $30, $30, $4" << endl;
        }
        else {
            cerr << "dcl error! NUM" << endl;
        }
    }
    else if(n->info.size() == 6 && n->info[0] == "dcls" && n->info[1] == "dcls" && n->info[2] == "dcl"
         && n->info[3] == "BECOMES" && n->info[4] == "NULL" && n->info[5] == "SEMI") {
        generate_tranverse(G, &n->children[0], table, current);
        generate_tranverse(G, &n->children[1], table, current);
        if(n->children[1].children[0].info.size() == 3) {  // dcl an pointer
            cout << "add $3, $0, $11" << endl;
            cout << "sw $3, " << (*table)[current].second[n->children[1].children[1].info[1]].second << "($29)" << endl;
            cout << "sub $30, $30, $4" << endl;
        }
        else {
            cerr << "dcl error! NULL" << endl;
        }
    }
    else if(n->info.size() == 5 && n->info[0] == "statement" && n->info[1] == "lvalue" && n->info[2] == "BECOMES"
         && n->info[3] == "expr" && n->info[4] == "SEMI") {
        generate_tranverse(G, &n->children[2], table, current);
        if(isID(G, &n->children[0], table, current)) {  // lvalue -> ID
            cout << "sw $3, " << (*table)[current].second[find_lexeme(&n->children[0], table, current)].second << "($29)" << endl;
        }
        else {  // lvalue -> STAR factor
            push(3);
            generate_tranverse(G, &n->children[0], table, current);
            pop(5);
            cout << "sw $5, 0($3)" << endl;
        }
    }
    else if(n->info.size() == 6 && n->info[1] == "NEW") {
        generate_tranverse(G, &n->children[3], table, current);
        cout << "add $1, $3, $0" << endl;
        push(31);
        cout << "lis $5" << endl;
        cout << ".word new" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        cout << "bne $3, $0, 1" << endl;
        cout << "add $3, $11, $0" << endl;
    }
    else if(n->info.size() == 6 && n->info[1] == "DELETE") {
        generate_tranverse(G, &n->children[3], table, current);
        cout << "beq $3, $11, skipDelete" << skip_delete_count << endl;
        cout << "add $1, $3, $0" << endl;
        push(31);
        cout << "lis $5" << endl;
        cout << ".word delete" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        cout << "skipDelete" << skip_delete_count << ":" << endl;
        skip_delete_count++;
    }
    else if(n->info[0] == "dcls") {
        return;
    }
    else if(n->info[0] == "dcl") {
        return;         // will modify later
    }
    else if(n->info.size() == 1 && n->info[0] == "statements") {
        return;
    }
    else if(n->info.size() == 2 && n->info[0] == "expr" && n->info[1] == "term") {
        generate_tranverse(G, &n->children[0], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "term" && n->info[1] == "factor") {
        generate_tranverse(G, &n->children[0], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "ID") {
        code(n->children[0].info[1], current, table);
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NULL") {
        cout << "add $3, $0, $11" << endl;
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "AMP" && n->info[2] == "lvalue") {
        if(n->children[1].info.size() == 2 && n->children[1].info[1] == "ID") {     // factor -> AMP lvalue, lvalue -> ID
            cout << "lis $3" << endl;
            cout << ".word " << (*table)[current].second[n->children[1].children[0].info[1]].second <<endl;
            cout << "add $3, $3, $29" << endl;
        }
        else if(n->children[1].info.size() == 4 && n->children[1].info[1] == "LPAREN") {    // factor -> AMP lvalue, lvalue -> ( lvalue )
            generate_tranverse(G, &n->children[1].children[1], table, current);
        }
        else if(n->children[1].info.size() == 3 && n->children[1].info[1] == "STAR") {      // factor -> AMP lvavlue, lvalue -> *factor
            generate_tranverse(G, &n->children[1].children[1], table, current);
        }
        else {
            cerr << "factor -> AMP lvalue: ERROR" << endl;
        }
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "STAR" && n->info[2] == "factor") {
        generate_tranverse(G, &n->children[1], table, current);
        cout << "lw $3, 0($3)" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "LPAREN" && n->info[2] == "expr"
         && n->info[3] == "RPAREN") {
        generate_tranverse(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "PLUS"
         && n->info[3] == "term") {
        if(n->children[0].type_of == "int" && n->children[2].type_of == "int") {
            generate_tranverse(G, &n->children[0], table, current);
            push(3);
            generate_tranverse(G, &n->children[2], table, current);
            pop(5);
            cout << "add $3, $5, $3" << endl;
        } else if(n->children[0].type_of == "int*") {
            generate_tranverse(G, &n->children[0], table, current);
            push(3);
            generate_tranverse(G, &n->children[2], table, current);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "add $3, $5, $3" << endl;
        } else {
            generate_tranverse(G, &n->children[2], table, current);
            push(3);
            generate_tranverse(G, &n->children[0], table, current);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "add $3, $5, $3" << endl;
        }
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "MINUS"
         && n->info[3] == "term") {
        if(n->children[0].type_of == "int" && n->children[2].type_of == "int") {
            generate_tranverse(G, &n->children[0], table, current);
            push(3);
            generate_tranverse(G, &n->children[2], table, current);
            pop(5);
            cout << "sub $3, $5, $3" << endl;
        } else if (n->children[0].type_of == "int*" && n->children[2].type_of == "int") {
            generate_tranverse(G, &n->children[0], table, current);
            push(3);
            generate_tranverse(G, &n->children[2], table, current);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "sub $3, $5, $3" << endl;
        } else {
            generate_tranverse(G, &n->children[0], table, current);
            push(3);
            generate_tranverse(G, &n->children[2], table, current);
            pop(5);
            cout << "sub $3, $5, $3" << endl;
            cout << "div $3, $4" << endl;
            cout << "mflo $3" << endl;
        }
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "STAR"
         && n->info[3] == "factor") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        cout << "mult $5, $3" << endl;
        cout << "mflo $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "SLASH"
         && n->info[3] == "factor") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        cout << "div $5, $3" << endl;
        cout << "mflo $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "PCT"
         && n->info[3] == "factor") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        cout << "div $5, $3" << endl;
        cout << "mfhi $3" << endl;
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NUM") {
        cout << "lis $3" << endl;
        cout << ".word " << n->children[0].info[1] << endl;
    }
    else if(n->info.size() == 3 && n->info[0] == "statements" && n->info[1] == "statements" && n->info[2] == "statement") {
        generate_tranverse(G, &n->children[0], table, current);
        generate_tranverse(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 6 && n->info[1] == "PRINTLN") {
        push(1);
        generate_tranverse(G, &n->children[2], table, current);
        cout << "add $1, $3, $0" << endl;
        push(31);
        cout << "lis $5" << endl;
        cout << ".word print" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        pop(1);
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "LT" && n->info[3] == "expr") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $5, $3" << endl;
        else
            cout << "sltu $3, $5, $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "GT" && n->info[3] == "expr") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $3, $5" << endl;
        else
            cout << "sltu $3, $3, $5" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "EQ" && n->info[3] == "expr") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int") {
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
        } else if(n->children[0].type_of == "int*") {
            cout << "sltu $6, $3, $5" << endl;
            cout << "sltu $7, $5, $3" << endl;
        }
        cout << "add $3, $6, $7" << endl;
        cout << "sub $3, $11, $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "NE" && n->info[3] == "expr") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int") {
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
        } else if(n->children[0].type_of == "int*") {
            cout << "sltu $6, $3, $5" << endl;
            cout << "sltu $7, $5, $3" << endl;
        }
        cout << "add $3, $6, $7" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "LE" && n->info[3] == "expr") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $3, $5" << endl;
        else
            cout << "sltu $3, $3, $5" << endl;
        cout << "sub $3, $11, $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "GE" && n->info[3] == "expr") {
        generate_tranverse(G, &n->children[0], table, current);
        push(3);
        generate_tranverse(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $5, $3" << endl;
        else
            cout << "sltu $3, $5, $3" << endl;
        cout << "sub $3, $11, $3" << endl;
    }
    else if(n->info.size() == 8 && n->info[1] == "WHILE") {
        int countt = while_label_count;
        cout << "loop" << countt << ":" << endl;
        while_label_count++;
        generate_tranverse(G, &n->children[2], table, current);
        cout << "beq $3, $0, endWhile" << countt << endl;
        generate_tranverse(G, &n->children[5], table, current);
        cout << "beq $0, $0, loop" << countt << endl;
        cout << "endWhile" << countt << ":" << endl;
    }
    else if(n->info.size() == 12 && n->info[1] == "IF") {
        int countt = if_lable_count;
        if_lable_count++;
        generate_tranverse(G, &n->children[2], table, current);
        cout << "beq $3, $0, else" << countt << endl;
        generate_tranverse(G, &n->children[5], table, current);
        cout << "beq $0, $0, endif" << countt << endl;
        cout << "else" << countt << ":" << endl;
        generate_tranverse(G, &n->children[9], table, current);
        cout << "endif" << countt << ":" << endl;
    }
    else if(n->info.size() == 3 && n->info[0] == "procedures" && n->info[1] == "procedure" && n->info[2] == "procedures") {
        generate_tranverse(G, &n->children[0], table, current);
        generate_tranverse(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 13 && n->info[0] == "procedures") {
        current = n->children[1].info[1];
        int num_para = (*table)[current].first.size();
        map<string, pair<string, int>>::iterator it;
        for(it = (*table)[current].second.begin(); it != (*table)[current].second.end(); ++it) {
            it->second.second -= (4 * num_para);
        }
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN" && n->info[3] == "RPAREN") {
        push(1);
        push(2);
        push(3);
        push(5);
        push(6);
        push(7);
        push(29);
        push(31);
        cout << "lis $5" << endl;
        cout << ".word F" << n->children[0].info[1] << endl;
        cout << "jalr $5" << endl;
        pop(31);
        pop(29);
        pop(7);
        pop(6);
        pop(5);
        pop(3);
        pop(2);
        pop(1);
    }
    else {
        cerr << "rule not found when generating code: " << endl;
        for(unsigned i = 0; i < n->info.size(); i++) {
            cerr << n->info[i] << " ";
        }
        cerr << endl;
    }
}

void generate_tranverse2(grammar* G, treenode* n, map<string ,pair<vector<string>, map<string, pair<string, int>>>>* table, string current) {
    if(isterminal(n->info[0], G))
        return;
    else if(n->info[0] == "main") {
        current = "wain";
    }
    else if(n->info[0] == "start") {
        generate_tranverse2(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "procedures") {
        generate_tranverse2(G, &n->children[0], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "type" && n->info[1] == "INT") {
        return;
    }
    else if(n->info.size() == 2 && n->info[0] == "lvalue" && n->info[1] == "ID") {
        return;         // will modify in a9
    }
    else if(n->info.size() == 3 && n->info[0] == "lvalue" && n->info[1] == "STAR" && n->info[2] == "factor") {
        generate_tranverse2(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 4 && n->info[0] == "lvalue" && n->info[1] == "LPAREN" && n->info[2] == "lvalue"
         && n->info[3] == "RPAREN") {
        generate_tranverse2(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 6 && n->info[0] == "dcls" && n->info[1] == "dcls" && n->info[2] == "dcl"
         && n->info[3] == "BECOMES" && n->info[4] == "NUM" && n->info[5] == "SEMI") {
        generate_tranverse2(G, &n->children[0], table, current);
        generate_tranverse2(G, &n->children[1], table, current);
        if(n->children[1].children[1].info.size() == 2) {   // dcl an int
            cout << "lis $3" << endl;
            cout << ".word " << n->children[3].info[1] << endl;
            cout << "sw $3, " << (*table)[current].second[n->children[1].children[1].info[1]].second << "($29)" <<endl;
            cout << "sub $30, $30, $4" << endl;
        }
        else {
            cerr << "dcl error! NUM" << endl;
        }
    }
    else if(n->info.size() == 6 && n->info[0] == "dcls" && n->info[1] == "dcls" && n->info[2] == "dcl"
         && n->info[3] == "BECOMES" && n->info[4] == "NULL" && n->info[5] == "SEMI") {
        generate_tranverse2(G, &n->children[0], table, current);
        generate_tranverse2(G, &n->children[1], table, current);
        if(n->children[1].children[0].info.size() == 3) {  // dcl an pointer
            cout << "add $3, $0, $11" << endl;
            cout << "sw $3, " << (*table)[current].second[n->children[1].children[1].info[1]].second << "($29)" << endl;
            cout << "sub $30, $30, $4" << endl;
        }
        else {
            cerr << "dcl error! NULL" << endl;
        }
    }
    else if(n->info.size() == 5 && n->info[0] == "statement" && n->info[1] == "lvalue" && n->info[2] == "BECOMES"
         && n->info[3] == "expr" && n->info[4] == "SEMI") {
        generate_tranverse2(G, &n->children[2], table, current);
        if(isID(G, &n->children[0], table, current)) {  // lvalue -> ID
            cout << "sw $3, " << (*table)[current].second[find_lexeme(&n->children[0], table, current)].second << "($29)" << endl;
        }
        else {  // lvalue -> STAR factor
            push(3);
            generate_tranverse2(G, &n->children[0], table, current);
            pop(5);
            cout << "sw $5, 0($3)" << endl;
        }
    }
    else if(n->info.size() == 6 && n->info[1] == "NEW") {
        generate_tranverse2(G, &n->children[3], table, current);
        cout << "add $1, $3, $0" << endl;
        push(31);
        cout << "lis $5" << endl;
        cout << ".word new" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        cout << "bne $3, $0, 1" << endl;
        cout << "add $3, $11, $0" << endl;
    }
    else if(n->info.size() == 6 && n->info[1] == "DELETE") {
        generate_tranverse2(G, &n->children[3], table, current);
        cout << "beq $3, $11, skipDelete" << skip_delete_count << endl;
        cout << "add $1, $3, $0" << endl;
        push(31);
        cout << "lis $5" << endl;
        cout << ".word delete" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        cout << "skipDelete" << skip_delete_count << ":" << endl;
        skip_delete_count++;
    }
    else if(n->info[0] == "dcls") {
        return;
    }
    else if(n->info[0] == "dcl") {
        return;         // will modify later
    }
    else if(n->info.size() == 1 && n->info[0] == "statements") {
        return;
    }
    else if(n->info.size() == 1 && n->info[0] == "params") {
        return;
    }
    else if(n->info.size() == 2 && n->info[0] == "expr" && n->info[1] == "term") {
        generate_tranverse2(G, &n->children[0], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "term" && n->info[1] == "factor") {
        generate_tranverse2(G, &n->children[0], table, current);
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "ID") {
        code(n->children[0].info[1], current, table);
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NULL") {
        cout << "add $3, $0, $11" << endl;
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "AMP" && n->info[2] == "lvalue") {
        if(n->children[1].info.size() == 2 && n->children[1].info[1] == "ID") {     // factor -> AMP lvalue, lvalue -> ID
            cout << "lis $3" << endl;
            cout << ".word " << (*table)[current].second[n->children[1].children[0].info[1]].second <<endl;
            cout << "add $3, $3, $29" << endl;
        }
        else if(n->children[1].info.size() == 4 && n->children[1].info[1] == "LPAREN") {    // factor -> AMP lvalue, lvalue -> ( lvalue )
            generate_tranverse2(G, &n->children[1].children[1], table, current);
        }
        else if(n->children[1].info.size() == 3 && n->children[1].info[1] == "STAR") {      // factor -> AMP lvavlue, lvalue -> *factor
            generate_tranverse2(G, &n->children[1].children[1], table, current);
        }
        else {
            cerr << "factor -> AMP lvalue: ERROR" << endl;
        }
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "STAR" && n->info[2] == "factor") {
        generate_tranverse2(G, &n->children[1], table, current);
        cout << "lw $3, 0($3)" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "LPAREN" && n->info[2] == "expr"
         && n->info[3] == "RPAREN") {
        generate_tranverse2(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "PLUS"
         && n->info[3] == "term") {
        if(n->children[0].type_of == "int" && n->children[2].type_of == "int") {
            generate_tranverse2(G, &n->children[0], table, current);
            push(3);
            generate_tranverse2(G, &n->children[2], table, current);
            pop(5);
            cout << "add $3, $5, $3" << endl;
        } else if(n->children[0].type_of == "int*") {
            generate_tranverse2(G, &n->children[0], table, current);
            push(3);
            generate_tranverse2(G, &n->children[2], table, current);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "add $3, $5, $3" << endl;
        } else {
            generate_tranverse2(G, &n->children[2], table, current);
            push(3);
            generate_tranverse2(G, &n->children[0], table, current);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "add $3, $5, $3" << endl;
        }
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "MINUS"
         && n->info[3] == "term") {
        if(n->children[0].type_of == "int" && n->children[2].type_of == "int") {
            generate_tranverse2(G, &n->children[0], table, current);
            push(3);
            generate_tranverse2(G, &n->children[2], table, current);
            pop(5);
            cout << "sub $3, $5, $3" << endl;
        } else if (n->children[0].type_of == "int*" && n->children[2].type_of == "int") {
            generate_tranverse2(G, &n->children[0], table, current);
            push(3);
            generate_tranverse2(G, &n->children[2], table, current);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "sub $3, $5, $3" << endl;
        } else {
            generate_tranverse2(G, &n->children[0], table, current);
            push(3);
            generate_tranverse2(G, &n->children[2], table, current);
            pop(5);
            cout << "sub $3, $5, $3" << endl;
            cout << "div $3, $4" << endl;
            cout << "mflo $3" << endl;
        }
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "STAR"
         && n->info[3] == "factor") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        cout << "mult $5, $3" << endl;
        cout << "mflo $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "SLASH"
         && n->info[3] == "factor") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        cout << "div $5, $3" << endl;
        cout << "mflo $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "PCT"
         && n->info[3] == "factor") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        cout << "div $5, $3" << endl;
        cout << "mfhi $3" << endl;
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NUM") {
        cout << "lis $3" << endl;
        cout << ".word " << n->children[0].info[1] << endl;
    }
    else if(n->info.size() == 3 && n->info[0] == "statements" && n->info[1] == "statements" && n->info[2] == "statement") {
        generate_tranverse2(G, &n->children[0], table, current);
        generate_tranverse2(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 6 && n->info[1] == "PRINTLN") {
        push(1);
        generate_tranverse2(G, &n->children[2], table, current);
        cout << "add $1, $3, $0" << endl;
        push(31);
        cout << "lis $5" << endl;
        cout << ".word print" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        pop(1);
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "LT" && n->info[3] == "expr") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $5, $3" << endl;
        else
            cout << "sltu $3, $5, $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "GT" && n->info[3] == "expr") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $3, $5" << endl;
        else
            cout << "sltu $3, $3, $5" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "EQ" && n->info[3] == "expr") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int") {
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
        } else if(n->children[0].type_of == "int*") {
            cout << "sltu $6, $3, $5" << endl;
            cout << "sltu $7, $5, $3" << endl;
        }
        cout << "add $3, $6, $7" << endl;
        cout << "sub $3, $11, $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "NE" && n->info[3] == "expr") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int") {
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
        } else if(n->children[0].type_of == "int*") {
            cout << "sltu $6, $3, $5" << endl;
            cout << "sltu $7, $5, $3" << endl;
        }
        cout << "add $3, $6, $7" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "LE" && n->info[3] == "expr") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $3, $5" << endl;
        else
            cout << "sltu $3, $3, $5" << endl;
        cout << "sub $3, $11, $3" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "test" && n->info[1] == "expr" && n->info[2] == "GE" && n->info[3] == "expr") {
        generate_tranverse2(G, &n->children[0], table, current);
        push(3);
        generate_tranverse2(G, &n->children[2], table, current);
        pop(5);
        if(n->children[0].type_of == "int")
            cout << "slt $3, $5, $3" << endl;
        else
            cout << "sltu $3, $5, $3" << endl;
        cout << "sub $3, $11, $3" << endl;
    }
    else if(n->info.size() == 8 && n->info[1] == "WHILE") {
        int countt = while_label_count;
        cout << "loop" << countt << ":" << endl;
        while_label_count++;
        generate_tranverse2(G, &n->children[2], table, current);
        cout << "beq $3, $0, endWhile" << countt << endl;
        generate_tranverse2(G, &n->children[5], table, current);
        cout << "beq $0, $0, loop" << countt << endl;
        cout << "endWhile" << countt << ":" << endl;
    }
    else if(n->info.size() == 12 && n->info[1] == "IF") {
        int countt = if_lable_count;
        if_lable_count++;
        generate_tranverse2(G, &n->children[2], table, current);
        cout << "beq $3, $0, else" << countt << endl;
        generate_tranverse2(G, &n->children[5], table, current);
        cout << "beq $0, $0, endif" << countt << endl;
        cout << "else" << countt << ":" << endl;
        generate_tranverse2(G, &n->children[9], table, current);
        cout << "endif" << countt << ":" << endl;
    }
    else if(n->info.size() == 3 && n->info[0] == "procedures" && n->info[1] == "procedure" && n->info[2] == "procedures") {
        generate_tranverse2(G, &n->children[0], table, current);
        generate_tranverse2(G, &n->children[1], table, current);
    }
    else if(n->info.size() == 13 && n->info[0] == "procedures") {
        current = n->children[1].info[1];
        cout << "F" << n->children[1].info[1] << ":" << endl;
        cout << "sub $29, $30, $4" << endl;
        generate_tranverse2(G, &n->children[6], table, current);
        generate_tranverse2(G, &n->children[7], table, current);
        generate_tranverse2(G, &n->children[9], table, current);
        cout << "add $30, $29, $4" << endl;;
        cout << "jr $31" << endl;
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN" && n->info[3] == "RPAREN") {
        push(1);
        push(2);
        push(3);
        push(5);
        push(6);
        push(7);
        push(29);
        push(31);
        cout << "lis $5" << endl;
        cout << ".word F" << n->children[0].info[1] << endl;
        cout << "jalr $5" << endl;
        pop(31);
        pop(29);
        pop(7);
        pop(6);
        pop(5);
        pop(3);
        pop(2);
        pop(1);
    }
    else {
        cerr << "rule not found when generating code 2: " << endl;
        for(unsigned i = 0; i < n->info.size(); i++) {
            cerr << n->info[i] << " ";
        }
        cerr << endl;
    }
}



void code_generate(grammar* G, treenode* n, map<string ,pair<vector<string>, map<string, pair<string, int>>>>* table, string current) {
    cout << "; some initial settings" << endl;
    if(contain_print) {
        cout << ".import print" << endl;
        cout << "lis $10" << endl;
        cout << ".word print" << endl;
    }
    if(contain_new) {
        cout << ".import init" << endl;
        cout << ".import new" <<endl;
        cout << ".import delete" << endl;
    }
    cout << "lis $4" << endl;
    cout << ".word 4" << endl;
    cout << "lis $11" << endl;
    cout << ".word 1" << endl;
    generate_tranverse(G, n, table, current);   // print the main function
    generate_tranverse2(G, n, table, current);  // print other procedures
}


int main() {
    // input the grammar
    grammar G;
    getGrammar(&G);
    file.close();

    // get the input(tree)
    treenode tree;
    get_input(&tree, &G);

    // tranverse the tree with a table;
    map<string,pair<vector<string>, map<string, pair<string, int>>>> table;
    tranverse(&tree, &table, "");

    // check types
    check_type_tree(&tree, &table, "");

    // output the table
    print_table(&table);

    // generating code
    code_generate(&G, &tree, &table, "");

    return 0;
}
