#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

struct trans{
    string st1, sym, st2;
    trans(string st1, string sym, string st2): st1{st1}, sym{sym}, st2{st2} {}
};

int main() {
    int num_alpha = 0;
    int num_states = 0;
    int num_accepts = 0;
    int num_trans = 0;
    string temp_str1, temp_str2, temp_str3;
    string current_state;
    string initial_state;
    vector<string> alphabet;
    vector<string> states;
    vector<trans> transitions;
    vector<string> accepts;

    cin >> num_alpha;
    for(int i = 0; i < num_alpha; ++i) {
        cin >> temp_str1;
        alphabet.push_back(temp_str1);
    }

    cin >> num_states;
    for(int i = 0; i < num_states; ++i) {
        cin >> temp_str1;
        states.push_back(temp_str1);
    }

    cin >> initial_state;

    cin >> num_accepts;
    for(int i = 0; i < num_accepts; ++i) {
        cin >> temp_str1;
        accepts.push_back(temp_str1);
    }

    cin >> num_trans;
    for(int i = 0; i < num_trans; ++i) {
        cin >> temp_str1 >> temp_str2 >> temp_str3;
        trans temp {temp_str1, temp_str2, temp_str3};
        transitions.push_back(temp);
    }

    string ignore,lineStr;
    getline(cin, ignore);
    while(getline(cin, lineStr)) {
        stringstream line(lineStr);
        current_state = initial_state;
        string nxt;
        bool flag = true;
        while(line >> nxt) {
            bool isValid = false;
            for(int i = 0; i < num_trans; ++i) {
                if(transitions[i].st1 == current_state && transitions[i].sym == nxt) {
                    isValid = true;
                    current_state = transitions[i].st2;
                    break;
                }
            }
            if(!isValid) {
                cout << "false" << endl;
                flag = false;
                break;
            }
        }
        if(flag) {
            bool isAccepts = false;
            for(int i = 0; i < num_accepts; ++i) {
                if(accepts[i] == current_state){
                    isAccepts = true;
                    break;
                }
            }
            if(isAccepts)
                cout << "true" << endl;
            else
                cout << "false" << endl;
        }
    }
}
