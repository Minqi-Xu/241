#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <utility>

using namespace std;

ifstream file{"WLP4g+pt"};

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

string type(treenode*, map<string,pair<vector<string>, map<string, string>>>*, string);

void get_arg_type(vector<string>* param, treenode* n, map<string,pair<vector<string>, map<string, string>>>* table, string current) {
    if(n->info.size() == 2 && n->info[0] == "arglist" && n->info[1] == "expr") {
        param->push_back(type(&n->children[0], table, current));
        return;
    }
    if(n->info[0] == "COMMA") return;
    for(unsigned i = 0; i < n->children.size(); i++) {
        get_arg_type(param, &n->children[i], table, current);
    }
}

string type(treenode* n, map<string,pair<vector<string>, map<string, string>>>* table, string current) {
    cout << "checking type for : ";
    for(unsigned i = 0; i < n->info.size(); i++) {
        cout << n->info[i] << " ";
    }
    cout << endl;
    if(n->info.size() == 2 && n->info[0] == "expr" && n->info[1] == "term") {
            cout << "expr -> term" << endl;                                     // debug
        return type(&n->children[0], table, current);
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "PLUS"
         && n->info[3] == "term") {
             cout << "expr -> expr PLUS term" << endl;                                      // debug
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int")
            return "int";
        if(type(&n->children[0], table, current) == "int*" && type(&n->children[2], table, current) == "int")
            return "int*";
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int*")
            return "int";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "expr" && n->info[1] == "expr" && n->info[2] == "MINUS"
         && n->info[3] == "term") {
             cout << "expr -> expr MINUS term" << endl;                         //debug
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int")
            return "int";
        if(type(&n->children[0], table, current) == "int*" && type(&n->children[2], table, current) == "int")
            return "int*";
        if(type(&n->children[0], table, current) == "int*" && type(&n->children[2], table, current) == "int*")
            return "int";
        return "error";
    }
    else if(n->info.size() == 2 && n->info[0] == "term" && n->info[1] == "factor") {
            cout << "term -> factor" << endl;                               // debug
        return type(&n->children[0], table, current);
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "STAR"
         && n->info[3] == "factor") {
             cout << "term -> factor" << endl;                             // debug
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int")
            return "int";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "SLASH"
         && n->info[3] == "factor") {
                cout << "term -> term SLASH factor" << endl;                // debug
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int")
            return "int";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "term" && n->info[1] == "term" && n->info[2] == "PCT"
         && n->info[3] == "factor") {
             cout << "term -> term PCT factor" << endl;                     // debug
        if(type(&n->children[0], table, current) == "int" && type(&n->children[2], table, current) == "int")
            return "int";
        return "error";
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "ID") {
            cout << "factor -> ID" << endl;                                 // debug
            cout << "returning type: " << (*table)[current].second[n->children[0].info[1]] << endl;  // debug;
            cout << "n->children[0].info[1]: " << n->children[0].info[1] << endl;               // debug
            map<string, string>::iterator it;
            for(it = (*table)[current].second.begin(); it != (*table)[current].second.end(); ++it) {
                cout << "    " << it->first << " " << it->second << endl;
            }
        return (*table)[current].second[n->children[0].info[1]];
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NUM") {
            cout << "factor -> NUM" << endl;                                // debug
        return "int";
    }
    else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "NULL") {
            cout << "factor -> NULL" << endl;                               // debug
        return "int*";
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "LPAREN" && n->info[2] == "expr"
         && n->info[3] == "RPAREN") {
             cout << "factor -> LPAREN expr RPAREN" << endl;                // debug
        return type(&n->children[1], table, current);
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "AMP" && n->info[2] == "lvalue") {
            cout << "factor -> AMP lvalue" << endl;                         // debug
        if(type(&n->children[1], table, current) == "int")  return "int*";
        return "error";
    }
    else if(n->info.size() == 3 && n->info[0] == "factor" && n->info[1] == "STAR" && n->info[2] == "factor") {
            cout << "factor -> STAR factor" << endl;                        // debug
        if(type(&n->children[1], table, current) == "int*")  return "int";
        return "error";
    }
    else if(n->info.size() == 6 && n->info[0] == "factor" && n->info[1] == "NEW" && n->info[2] == "INT"
         && n->info[3] == "LBRACK" && n->info[4] == "expr" && n->info[5] == "RBRACK") {
             cout << "factor -> NEW INT " << endl;                          // debug
        if(type(&n->children[3], table, current) == "int")  return "int*";
        return "error";
    }
    else if(n->info.size() == 2 && n->info[0] == "lvalue" && n->info[1] == "ID") {
            cout << "lvalue -> ID" << endl;                                 // debug
        return (*table)[current].second[n->children[0].info[1]];
    }
    else if(n->info.size() == 3 && n->info[0] == "lvalue" && n->info[1] == "STAR" && n->info[2] == "factor") {
            cout << "lvalue -> STAR factor" << endl;                        // debug
        if(type(&n->children[1], table, current) == "int*") return "int";
        return "error";
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "LPAREN" && n->info[2] == "lvalue"
         && n->info[3] == "RPAREN") {
             cout << "factor -> LPAREN lvalue RPAREN" << endl;              // debug
        return type(&n->children[1], table, current);
    }
    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN"
         && n->info[3] == "RPAREN") {
            cout << "factor -> ID LPAREN RPAREN" << endl;                   // debug
        if((*table)[n->children[0].info[1]].first.size() == 0)  return "int";
        return "error";
    }
    else if(n->info.size() == 5 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN"
                                && n->info[3] == "arglist" && n->info[4] == "RPAREN") {
            cout << "factor -> ID(....)" << endl;                           // debug
        vector<string> arg_type;
        get_arg_type(&arg_type, &n->children[2], table, current);
        if(arg_type.size() != (*table)[n->children[0].info[1]].first.size())    return "error";
        for(unsigned i = 0; i < arg_type.size(); i++) {
            if(arg_type[i] != (*table)[n->children[0].info[1]].first[i])    return "error";
        }
        return "int";
    }
    cerr << "ERROR: type find false:" << endl;
    for(unsigned i = 0; i < n->info.size(); i++) {
        cerr << n->info[i] << " ";
    }
    cerr << endl;
    return "error";
}

void tranverse(treenode* n, map<string,pair<vector<string>, map<string, string>>>* table, string current) {
    cout << "tranversing on : ";
    for(unsigned i = 0; i < n->info.size(); i++) {
        cout << n->info[i] << " ";
    }
    cout << endl;
    if(n->info[0] == "main") {
        pair<vector<string> ,map<string, string>> main_table;
        (*table)["wain"] = main_table;
        current = "wain";
        if(n->children[3].children[0].info.size() == 3) {
            (*table)["wain"].first.push_back("int*");
        } else {
            (*table)["wain"].first.push_back("int");
        }
        if(n->children[5].children[0].info.size() == 3) {
            (*table)["wain"].first.push_back("int*");
            cerr << "ERROR! the arguments of main function false" << endl;
            return;
        } else {
            (*table)["wain"].first.push_back("int");
        }
//        cout << "Main num children: " << n->children.size() << endl;                // debug
        cout << type(&n->children[11], table, current) << endl;                 // debug
        if(type(&n->children[11], table, current) != "int") {
            cerr << "ERROR! main function should return int type!" << endl;
            return;
        }
    }

    else if(n->info[0] == "procedure") {
        pair<vector<string> ,map<string, string>> pro_table;
        current = n->children[1].info[1];
        map<string,pair<vector<string>, map<string, string>>>::iterator it = (*table).find(current);
        if(it != (*table).end()) {
            cerr << "ERROR! procedure declared more than once: " << current << endl;
            return;
        }
        (*table)[current] = pro_table;
        get_param_type(&(*table)[current].first, &n->children[3]);
//        cout << "method num children: " << n->children.size() << endl;             // debug
        cout << type(&n->children[9], table, current) << endl;                 // debug
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

    else if(n->info.size() >= 3 && n->info[0] == "dcl" && n->info[1] == "type" && n->info[2] == "ID") {
        map<string, string>::iterator it = (*table)[current].second.find(n->children[1].info[1]);
        if(it != (*table)[current].second.end()) {
            cerr << "ERROR! declared more than once: " << n->children[1].info[1] << endl;
            return;
        }
        if(n->children[0].info.size() == 3) {
            (*table)[current].second[n->children[1].info[1]] = "int*";
        } else {
            (*table)[current].second[n->children[1].info[1]] = "int";
        }
        return;
    } else if(n->info.size() == 2 && n->info[0] == "factor" && n->info[1] == "ID") {
        map<string, string>::iterator it = (*table)[current].second.find(n->children[0].info[1]);
        if(it == (*table)[current].second.end()) {
            cerr << "ERROR! using before declare: " << n->children[0].info[1] << endl;
            return;
        }
        return;
    } else if(n->info.size() >= 2 && n->info[0] == "lvalue" && n->info[1] == "ID") {
        map<string, string>::iterator it = (*table)[current].second.find(n->children[0].info[1]);
        if(it == (*table)[current].second.end()) {
            cerr << "ERROR! using before declare: " << n->children[0].info[1] << endl;
            return;
        }
        return;
    }

    else if(n->info.size() == 4 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN" && n->info[3] == "RPAREN") {
        map<string,pair<vector<string>, map<string, string>>>::iterator it = (*table).find(n->children[0].info[1]);
        if(it == (*table).end()) {
            cerr << "ERROR! procedure using before decalre: " << n->children[0].info[1] << endl;
            return;
        }
    } else if (n->info.size() == 5 && n->info[0] == "factor" && n->info[1] == "ID" && n->info[2] == "LPAREN"
                                   && n->info[3] == "arglist" && n->info[4] == "RPAREN") {
        map<string,pair<vector<string>, map<string, string>>>::iterator it = (*table).find(n->children[0].info[1]);
        if(it == (*table).end()) {
            cerr << "ERROR! procedure using before decalre: " << n->children[0].info[1] << endl;
            return;
        }
    }


    for(unsigned i = 0; i < n->children.size(); i++)
        tranverse(&n->children[i], table, current);
}

void print_table(map<string ,pair<vector<string>, map<string, string>>>* table) {
    map<string ,pair<vector<string>, map<string, string>>>::iterator it;
    for(it = (*table).begin(); it != (*table).end(); ++it) {
        cerr << it->first << ": ";
        for(unsigned i = 0; i < it->second.first.size(); i++) {
            cerr << it->second.first[i] << " ";
        }
        cerr << endl;
        map<string, string>::iterator itt;
        for(itt = it->second.second.begin(); itt != it->second.second.end(); ++itt) {
            cerr << itt->first << " " << itt->second << endl;
        }
    }
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
    map<string,pair<vector<string>, map<string, string>>> table;
    tranverse(&tree, &table, "");

    // output the table
    print_table(&table);

    return 0;
}
