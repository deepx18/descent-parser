#include <stdlib.h>
#include <stdio.h>

typedef enum TokenKind {
    Number,
    Plus,
    Minus,
    Star,
    Slash
} TokenKind;

typedef struct Token {
    TokenKind kind;
    char *value;
    int priority;
} Token;

typedef struct Lexer {
    Token **tokens;
    int cap;
    int count;
    size_t idx;
} Lexer;



typedef struct Node Node;
struct Node {
    Node *left;
    Node *right;
    Token *value;
};


Node *init_node(Token *value, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->value = value;
    node->left = left;
    node->right = right;

    return node;
}

Node *init_tree() {
    Node *root = init_node(NULL, NULL, NULL);
    root->left = init_node(NULL, NULL, NULL);
    root->right = init_node(NULL, NULL, NULL);

    return root;
}

void dump_tree(Node *r, int level) {
    if (r == NULL) return;
    for (int i = 0; i < level; i++) {
        printf("-");
    }
    if (level != 0) printf(" ");

    printf("%s\n", r->value->value);
    if (r->left != NULL) dump_tree(r->left, level + 1);
    if (r->right != NULL) dump_tree(r->right, level + 1);


    return;
}
