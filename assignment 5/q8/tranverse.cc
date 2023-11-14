#include <iostream>
#include <vector>

using namespace std;

struct Node {
    int val;
    int num_children;
    vector<Node*> childrens;
};

Node* build_tree() {
    Node* node = new Node;
    cin >> node->val >> node->num_children;
    for(int i = 0; i < node->num_children; ++i) {
        (node->childrens).push_back(build_tree());
    }
    return node;
}

void tranverse(Node* node) {
    for(int i = 0; i < node->num_children; ++i) {
        tranverse(node->childrens[i]);
    }
    cout << node->val << " " << node->num_children << endl;
}

void destroy(Node* node) {
    for(int i = 0; i < node->num_children; ++i) {
        destroy(node->childrens[i]);
    }
    delete node;
}

int main() {
    Node* root = build_tree();
    tranverse(root);
    destroy(root);
    return 0;
}
