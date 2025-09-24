// Arquivo: src/ast.h

#ifndef AST_H
#define AST_H

// Enum para identificar o tipo de cada nó na árvore
typedef enum {
    NODE_TYPE_NUM,      // Nó para um número literal 
    NODE_TYPE_ID,       // Nó para um identificador
    NODE_TYPE_OP,       // Nó para uma operação binária 
    NODE_TYPE_ASSIGN,    // Nó para uma atribuicao
    NODE_TYPE_IF // Nó para uma condicao
} NodeType;

typedef struct AstNode {
    NodeType type;
    char op;
    int lineno;   /* NOVO: linha aproximada onde o nó foi criado */
    union {
        int valor;
        char* nome;

        struct {
            struct AstNode* left;
            struct AstNode* right;
        } children;

        struct {
            struct AstNode* condicao;
            struct AstNode* bloco_then;
            struct AstNode* bloco_else; // Pode ser NULL se não houver 'else'
        } if_details;

    } data;
} AstNode;

// --- PROTÓTIPOS DAS FUNÇÕES ---

// Funções para criar nós (serão implementadas em ast.c)
AstNode* create_num_node(int valor);
AstNode* create_id_node(char* nome);
AstNode* create_op_node(char op, AstNode* left, AstNode* right);
AstNode* create_assign_node(AstNode* left, AstNode* right);
AstNode* create_if_node(AstNode* condicao, AstNode* bloco_then, AstNode* bloco_else);

void liberar_ast(AstNode* no);

// Função do interpretador (será implementada em interpretador.c)
int interpretar(AstNode* no);

#endif