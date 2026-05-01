#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

#define LEXER_CAP 1024

Lexer *init_lexer() {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->cap = LEXER_CAP;
    lexer->tokens = malloc(sizeof(Token) * LEXER_CAP);
    lexer -> count = 0;
    lexer->idx = 0;
    return lexer;
}

void insert_token(Lexer *lexer, TokenKind kind, char *value) {
    Token *token = malloc(sizeof(Token));
    token->kind = kind;
    token->value = value;

    assert(lexer->cap > lexer->count);
    lexer->tokens[lexer->count++] = token;

    return;
}

void get_tokens(Lexer *lexer, char src[]) {
    char *tok;
    tok = strtok(src, " ");

    while (tok != NULL) {
        if (strcmp(tok, "+") == 0) {
            insert_token(lexer, Plus, tok);
        } else if (strcmp(tok, "-") == 0) {
            insert_token(lexer, Minus, tok);
        } else if (strcmp(tok, "*") == 0) {
            insert_token(lexer, Star, tok);
        } else if (strcmp(tok, "/") == 0) {
            insert_token(lexer, Slash, tok);
        } else {
            insert_token(lexer, Number, tok);
        }

        tok = strtok(NULL, " ");
    }

    return;
}


Token *peek_token(Lexer *lexer) {
    if (lexer->idx < lexer->count) return lexer->tokens[lexer->idx];
    return NULL;
}

Token *get_token(Lexer *lexer) {
    if (lexer->idx < lexer->count) return lexer->tokens[lexer->idx++];
    return NULL;
}


void dump_tokens(Lexer *lexer) {
    for (int i = 0; i < lexer->count; i++) {
        Token *token = lexer->tokens[i];
        switch (lexer->tokens[i]->kind) {
            case Number:
                printf("Number: [%s]\n", token->value);
                break;
            case Plus:
                printf("Plus: [%s]\n", token->value);
                break;
            case Minus:
                printf("Minus: [%s]\n", token->value);
                break;
            case Star:
                printf("Star: [%s]\n", token->value);
                break;
            case Slash:
                printf("Slash: [%s]\n", token->value);
                break;
        }

    }
}

Node *get_ast(Lexer *lexer) {
    Node *root = NULL;
    Token *curr_num_tok = NULL;
    while (peek_token(lexer) != NULL) {
        if (peek_token(lexer)->kind == Number) {
            curr_num_tok = get_token(lexer);
        } else if ((peek_token(lexer)->kind == Plus) || (peek_token(lexer)->kind == Minus) || (peek_token(lexer)->kind == Star) || (peek_token(lexer)->kind == Slash) ) {

            Node *left = init_node(curr_num_tok, NULL, NULL);
            Token *op_tok = get_token(lexer);
            Token *next_num_tok = get_token(lexer);
            Node *right = init_node(next_num_tok, NULL, NULL);
            root = init_node(op_tok, left, right);
        }
    }

    return root;
}

void traverse_ast(Node *r) {
    int result = 0;
    switch (r->value->kind) {
    case Plus:
        result = atoi(r->left->value->value) + atoi(r->right->value->value);
        break;
    case Minus:
        result = atoi(r->left->value->value) - atoi(r->right->value->value);
        break;
    case Star:
        result = atoi(r->left->value->value) * atoi(r->right->value->value);
        break;
    case Slash:
        result = atoi(r->left->value->value) / (float)atoi(r->right->value->value);
        break;
    }
    printf("%s %s %s = %d\n", r->left->value->value, r->value->value, r->right->value->value, result);
    return;
}

int main () {
    char buffer[] = "2 * 4";
    Lexer *lexer = init_lexer();
    get_tokens(lexer, buffer);
    // dump_tokens(lexer);


    Node *root = get_ast(lexer);
    dump_tree(root, 0);
    traverse_ast(root);


    return 0;
}
