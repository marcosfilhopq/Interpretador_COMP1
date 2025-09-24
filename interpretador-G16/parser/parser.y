// Arquivo: parser/parser.y

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "simbolo.h"

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
extern int interpret_error;
%}

%define parse.error verbose

%union {
    int valor;
    char *str;
    struct AstNode* no;
}

%token <valor> NUM
%token <str> ID

%token IF ELSE LBRACE RBRACE LPAREN RPAREN
%token EQ NE LT GT LE GE
%token NEWLINE PLUS MINUS TIMES DIVIDE IGUAL

%type <no> linha expressao atribuicao comando_if

%right IF ELSE
%left GT LT GE LE EQ NE
%left PLUS MINUS
%left TIMES DIVIDE

%%
programa:
    | programa linha
    ;

linha:
    expressao NEWLINE {
                          interpret_error = 0;
                          int resultado = interpretar($1);
                          if (!interpret_error) {
                              printf("Resultado: %d\n", resultado);
                          }
                          liberar_ast($1);
                      }
  | atribuicao NEWLINE {
                          interpret_error = 0;
                          int resultado = interpretar($1);
                          if (!interpret_error) {
                              printf("OK. '%s' agora é %d\n", $1->data.children.left->data.nome, resultado);
                          }
                          liberar_ast($1);
                       }
  | comando_if NEWLINE {
                          interpret_error = 0;
                          interpretar($1);
                          liberar_ast($1);
                       }
  | NEWLINE            { /* Nao faz nada */ }
  | error NEWLINE      { yyerrok; }
  ;

comando_if:
    IF LPAREN expressao RPAREN LBRACE atribuicao RBRACE {
        AstNode* if_node = create_if_node($3, $6, NULL);
        if_node->lineno = yylineno;
        $$ = if_node;
    }
  | IF LPAREN expressao RPAREN LBRACE atribuicao RBRACE ELSE LBRACE atribuicao RBRACE {
        AstNode* if_node = create_if_node($3, $6, $10);
        if_node->lineno = yylineno;
        $$ = if_node;
    }
  ;

atribuicao:
    ID IGUAL expressao {
        AstNode* left = create_id_node($1);
        left->lineno = yylineno;
        $$ = create_assign_node(left, $3);
        $$->lineno = left->lineno;
    }
;

expressao:
    NUM                 { $$ = create_num_node($1); $$->lineno = yylineno; }
  | ID                { $$ = create_id_node($1); $$->lineno = yylineno; }
  | expressao PLUS expressao   { $$ = create_op_node('+', $1, $3); $$->lineno = $1->lineno; }
  | expressao MINUS expressao  { $$ = create_op_node('-', $1, $3); $$->lineno = $1->lineno; }
  | expressao TIMES expressao  { $$ = create_op_node('*', $1, $3); $$->lineno = $1->lineno; }
  | expressao DIVIDE expressao { $$ = create_op_node('/', $1, $3); $$->lineno = $1->lineno; }
  | expressao GT expressao     { $$ = create_op_node('>', $1, $3); $$->lineno = $1->lineno; }
  | expressao LT expressao     { $$ = create_op_node('<', $1, $3); $$->lineno = $1->lineno; }
  | expressao GE expressao     { $$ = create_op_node(GE, $1, $3); $$->lineno = $1->lineno; }
  | expressao LE expressao     { $$ = create_op_node(LE, $1, $3); $$->lineno = $1->lineno; }
  | expressao EQ expressao     { $$ = create_op_node(EQ, $1, $3); $$->lineno = $1->lineno; }
  | expressao NE expressao     { $$ = create_op_node(NE, $1, $3); $$->lineno = $1->lineno; }
  | LPAREN expressao RPAREN    { $$ = $2; }
;

%%
/* SECAO DE CODIGO C FINAL */
struct simbolo tabelaSimbolos[MAX_SIMBOLOS];
int proximoSimbolo = 0;

void yyerror(const char *s) {
    fprintf(stderr, "Linha %d: Erro sintático: %s\n", yylineno, s);
}

int inserir_simbolo(char *nome, int valor) {
    int indice = procurar_simbolo(nome);
    if (indice != -1) {
        tabelaSimbolos[indice].valor = valor;
        return indice;
    }
    if (proximoSimbolo >= MAX_SIMBOLOS) {
        fprintf(stderr, "Tabela de simbolos cheia!\n");
        exit(1);
    }
    strcpy(tabelaSimbolos[proximoSimbolo].nome, nome);
    tabelaSimbolos[proximoSimbolo].valor = valor;
    return proximoSimbolo++;
}

int procurar_simbolo(char *nome) {
    for (int i = 0; i < proximoSimbolo; i++) {
        if (strcmp(tabelaSimbolos[i].nome, nome) == 0) return i;
    }
    return -1;
}