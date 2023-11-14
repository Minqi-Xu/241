#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

ifstream file{"WLP4g+pt"};

vector<string> lexe;
int counting = 0;          // lexe and counting use for output the tree

struct in_put {
    string Kind;
    string lexeme;
};

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


struct action{          // the structure of rule in DFA's states
    int current_state;  // the current state where the action in
    string s;           // the symbol is first in the unread input
    string act;         // act must be "reduce" or "shift"
    int apply;          // if act is reduce, then apply is the rule that will apply
                        //    if act is shift, then apply is the next state
};

struct DFA{             // the structure of DFA
    int num_state;       // number of states
    int num_action;     // number of actions
    vector<action> rule;
};

void getDFA(DFA* dfa) {         // input and store DFA
    file >> dfa->num_state;
    file >> dfa->num_action;
    for(int i = 0; i < dfa->num_action; ++i) {
        action a;
        file >> a.current_state >> a.s >> a.act >> a.apply;
        dfa->rule.push_back(a);
    }
}

struct treenode{
    string val;
    vector<treenode> child;
};


void out_put(treenode* n, vector<in_put>* sequence, grammar* G) {
    if(n->child.empty()) {
        if(!isterminal(n->val, G)) {
            cout << n->val << " " << endl;
            return;
        }
        cout << n->val << " " << lexe[counting] << endl;
        counting++;
        return;
    }
    cout << n->val << " ";
    cout << n->child.back().val;
    if(n->child.size() > 1) {
        cout << " ";
    } else {
        cout << endl;
    }
    for(int i = n->child.size() - 2; i >= 1; i--) {
        cout << n->child[i].val << " ";
    }
    if(n->child.size() > 1)
        cout << n->child[0].val << endl;
    while(!n->child.empty()){
//        if(!isterminal(n->child.back().val,G))
            out_put(&(n->child.back()), sequence, G);
        n->child.pop_back();
    }

}

int parse(grammar* G, DFA* dfa, vector<in_put>* sequence){
    int count_k = 0;              // use for error k count
    vector<string> symbol_stack;
    vector<int> state_stack;
    vector<treenode> tree_stack;
    int current_s = 0;          // current state, initialized to be 0, the start state.
    state_stack.push_back(current_s);
    while(!sequence->empty()) {
        bool foundact = false;
        in_put read = sequence->back();

        for(int i = 0; i < dfa->num_action; ++i) {
            if(dfa->rule[i].current_state == current_s && dfa->rule[i].s == read.Kind) {
                foundact = true;
                if(dfa->rule[i].act == "shift") {       // shift needed
                    symbol_stack.push_back(read.Kind);
                    sequence->pop_back();
                    ++count_k;
                    state_stack.push_back(dfa->rule[i].apply);
                    current_s = dfa->rule[i].apply;
                    treenode n;
                    n.val = read.Kind;
                    tree_stack.push_back(n);

                } else {                // reduce needed
                    int rule_applying = dfa->rule[i].apply;
                    unsigned rhs = G->rules[rule_applying].size() - 1;
                    treenode n;
                    n.val = G->rules[rule_applying][0];
                    while(rhs > 0) {
                        n.child.push_back(tree_stack.back());
                        tree_stack.pop_back();
                        symbol_stack.pop_back();
                        state_stack.pop_back();
                        rhs--;
                    }
                    tree_stack.push_back(n);
                    symbol_stack.push_back(G->rules[rule_applying][0]);
                    current_s = state_stack.back();
                    for(int j = 0; j < dfa->num_action; ++j) {
                        if(dfa->rule[j].current_state == current_s && dfa->rule[j].s == G->rules[rule_applying][0] &&
                           dfa->rule[j].act == "shift") {
                            state_stack.push_back(dfa->rule[j].apply);
                            current_s = dfa->rule[j].apply;
                            break;
                        }
                    }

                }
                break;
            }
        }
        if(!foundact) {         // if no action found, reject!
            cerr << "ERROR at " << count_k << endl;
            return 1;
        }
    }

    treenode n;
    n.val = G->rules[0][0];
    unsigned rhs = G->rules[0].size() - 1;
    while(rhs > 0) {
        n.child.push_back(tree_stack.back());
        tree_stack.pop_back();
        rhs--;
    }
    tree_stack.push_back(n);


    // do the output
    out_put(&tree_stack.back(), sequence, G);

    return 0;
}


int main() {
    // input the grammar
    grammar G;
    getGrammar(&G);

    // input the DFA
    DFA dfa;
    getDFA(&dfa);

    file.close();

    // input the sequence that need to be parsed
    vector<in_put> sequence;
    string s, lexemee;
    s = "BOF";
    lexemee = "BOF";
    in_put bof;
    bof.Kind = s;
    bof.lexeme = lexemee;
    sequence.push_back(bof);
    lexe.push_back("BOF");
    while(cin >> s >> lexemee) {
        in_put term;
        term.Kind = s;
        term.lexeme = lexemee;
        sequence.push_back(term);
        lexe.push_back(lexemee);
    }
    s = "EOF";
    lexemee = "EOF";
    in_put eof;
    eof.Kind = s;
    eof.lexeme = lexemee;
    sequence.push_back(eof);
    lexe.push_back("EOF");

    //reverse vector to let it been poped in the order we input it
    vector<in_put>::size_type sz = sequence.size();
    for(unsigned i = 0; i < sz/2; ++i) {
        in_put temp;
        temp = sequence[sz-1-i];
        sequence[sz-1-i] = sequence[i];
        sequence[i] = temp;
    }

    //do the parsing
    int return_val;
    return_val = parse(&G, &dfa, &sequence);


    return return_val;
}
