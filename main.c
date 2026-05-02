#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

#define LEXER_CAP 1024
#define CHECK_PEEK_TOKEN(x, xo) peek_token(lexer, xo)->kind == x


// Pure Lexer Related Functions
Lexer *init_lexer() {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->cap = LEXER_CAP;
    lexer->tokens = malloc(sizeof(Token) * LEXER_CAP);
    lexer -> count = 0;
    lexer->idx = 0;
    return lexer;
}
Lexer *expand_lexer(Lexer *lexer) {
    assert(0 && "Re-allocate more memory for tokens if needed.");
    return NULL;
}

// In-Lexer Token Related Functions
void insert_token(Lexer *lexer, TokenKind kind, char *value, int pr) {
    Token *token = malloc(sizeof(Token));
    token->kind = kind;
    token->value = value;
    token->priority = pr;

    assert(lexer->cap > lexer->count);
    lexer->tokens[lexer->count++] = token;

    return;
}
void get_tokens(Lexer *lexer, char src[]) {
    size_t src_len = strlen(src);
    for (int i = 0; i < src_len; i++) {
        char tok = src[i];
        if (tok == '+') {
            insert_token(lexer, Plus, "+", 1);
        } else if (tok == '-') {
            insert_token(lexer, Minus, "/", 1);
        } else if (tok == '*') {
            insert_token(lexer, Star, "*", 2);
        } else if (tok == '/') {
            insert_token(lexer, Slash, "/", 2);
        } else if (tok == ' ' || tok == '\n') {
            continue;
        } else {
            int start = i;
            while (isdigit((int)src[++i]) != 0);
            char *num = malloc(sizeof(char) * (i - start));
            strncpy( num, src + start, i-- - start);
            insert_token(lexer, Number, num, 0);
        }

    }

    // changed tokenization method from striping by space to actualy verifying every char and constructing tokens based on that
    // char *tok;
    // tok = strtok(src, "");
    // while (tok != NULL) {
    //     // if (strcmp(tok, "+") == 0) {
    //     //     insert_token(lexer, Plus, tok, 1);
    //     // } else if (strcmp(tok, "-") == 0) {
    //     //     insert_token(lexer, Minus, tok, 1);
    //     // } else if (strcmp(tok, "*") == 0) {
    //     //     insert_token(lexer, Star, tok, 2);
    //     // } else if (strcmp(tok, "/") == 0) {
    //     //     insert_token(lexer, Slash, tok, 2);
    //     // } else {
    //     //     insert_token(lexer, Number, tok, 0);
    //     // }
    //     printf("TOKEN: [%s]", tok);
    //     tok = strtok(NULL, " ");
    // }

    return;
}
Token *peek_token(Lexer *lexer, size_t offset){
    if (lexer->idx + offset < lexer->count) return lexer->tokens[lexer->idx+offset];
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

int check_peek_tokens(Lexer *lexer, TokenKind x, TokenKind y, size_t xo, size_t yo) {
    if (peek_token(lexer, xo) != NULL && peek_token(lexer, yo) != NULL)
        return peek_token(lexer, xo)->kind == x ||  peek_token(lexer, yo)->kind == y;

    return 0;
}



// Parsing Functions

size_t find_main_op(Lexer *lexer) {

    for (size_t i = 0; i < lexer->count; i++) {
        if (
            lexer->tokens[i]->kind == Plus ||
            lexer->tokens[i]->kind == Minus
        ) {
            return i;
        }
    }
    return -1;
}


Node *get_ast(Lexer *lexer, size_t start, size_t end) {
    Node *root = NULL;
    Token *curr_num_tok = NULL;
    size_t idx = find_main_op(lexer);

    if (idx == -1) {
        for (int i = start; i < end; i++) {
            if (lexer->tokens[i]->kind == Number) {
                curr_num_tok = get_token(lexer);
            } else if (
                (lexer->tokens[i]->kind == Plus) ||
                (lexer->tokens[i]->kind == Minus) ||
                (lexer->tokens[i]->kind == Star) ||
                (lexer->tokens[i]->kind == Slash) )
            {
                Node *left = init_node(curr_num_tok, NULL, NULL);
                Token *op_tok = get_token(lexer);
                Token *next_num_tok = get_token(lexer);
                Node *right = init_node(next_num_tok, NULL, NULL);
                root = init_node(op_tok, left, right);
            }
        }
    }

    return root;
}


int traverse_ast(Node *r) {
    int result = 0;
    switch (r->value->kind) {
    case Plus:
        result =
            traverse_ast(r->left) + traverse_ast(r->right);
        break;
    case Minus:
        result =
            traverse_ast(r->left) - traverse_ast(r->right);
        break;
    case Star:
        result =
            traverse_ast(r->left) * traverse_ast(r->right);
        break;
    case Slash:
        result =
            traverse_ast(r->left) / (float)traverse_ast(r->right);
        break;
    case Number:
        return atoi(r->value->value);
        break;
    }
    // printf("%s %s %s = %d\n", r->left->value->value, r->value->value, r->right->value->value, result);
    return result;
}

Node *parse_factor(Lexer *lexer) {
    if (peek_token(lexer, 0)->kind == Number) {
        Node *node = init_node(get_token(lexer), NULL, NULL);
        return node;
    }
    return NULL;
}

Node *parse_term(Lexer *lexer) {
    Node *node = init_node(NULL, NULL, NULL);
    while (peek_token(lexer, 0) != NULL) {
        if (peek_token(lexer, 0)->kind == Star ||
            peek_token(lexer, 0)->kind == Slash)
        {
            node->value = get_token(lexer);
            node->right = parse_factor(lexer);
            break;
        } else if (
            peek_token(lexer, 0)->kind == Plus ||
            peek_token(lexer, 0)->kind == Minus)
        {
            break;
        } else {
            if (check_peek_tokens(lexer, Star, Slash, 1, 1))
                node->left = parse_factor(lexer);
            else return parse_factor(lexer);
        }
    }

    return node;
}

Node *parse_expression(Lexer *lexer) {
    Node *left = parse_term(lexer);

    while (peek_token(lexer, 0) != NULL) {
        if (check_peek_tokens(lexer, Plus, Minus, 0, 0))
        {
            Node *node = init_node(get_token(lexer), NULL, NULL);
            node->right = parse_term(lexer);
            node->left = left;
            left = node;
         }
        // else {
        //    node->left = parse_term(lexer);
        // }
    }

    return left;
}

int main () {
    // char buffer[] = "2 * 4 + 2 + 2";
    char buffer[256];
    printf("> ");
    fgets(&buffer, 256, stdin);

    // printf("%s", buffer);
    // return 0;
    Lexer *lexer = init_lexer();
    get_tokens(lexer, buffer);
    // dump_tokens(lexer);
    // printf("%ld\n", find_main_op(lexer));

    // Node *root = get_ast(lexer, lexer->idx, lexer->count);
    Node *root = parse_expression(lexer);
    // dump_tree(root, 0);
    printf("%d\n", traverse_ast(root));

    return 0;
}
