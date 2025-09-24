// Arquivo: src/interpretador.c (Unificado com if-else e melhor tratamento de erros)

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "simbolo.h"
#include "parser.tab.h" // Inclui para ter acesso aos tokens (ex: GE, LE)

// Flag global para controle de erro, vindo da versão da sua colega
int interpret_error = 0;

int interpretar(AstNode* no) {
    if (!no) return 0;

    switch (no->type) {
        case NODE_TYPE_NUM:
            return no->data.valor;

        case NODE_TYPE_ID: {
            int indice = procurar_simbolo(no->data.nome);
            if (indice == -1) {
                interpret_error = 1;
                fprintf(stderr, "Linha %d: Erro semântico: variável '%s' não declarada\n", no->lineno, no->data.nome);
                return 0;
            }
            return tabelaSimbolos[indice].valor;
        }

        case NODE_TYPE_ASSIGN: {
            char* var_nome = no->data.children.left->data.nome;
            int valor_expr = interpretar(no->data.children.right);
            if (interpret_error) return 0; // Propaga o erro sem atribuir
            inserir_simbolo(var_nome, valor_expr);
            return valor_expr;
        }

        case NODE_TYPE_OP: {
            int val_esq = interpretar(no->data.children.left);
            if (interpret_error) return 0;
            int val_dir = interpretar(no->data.children.right);
            if (interpret_error) return 0;

            switch (no->op) {
                // Aritméticos
                case '+': return val_esq + val_dir;
                case '-': return val_esq - val_dir;
                case '*': return val_esq * val_dir;
                case '/':
                    if (val_dir == 0) {
                        interpret_error = 1;
                        fprintf(stderr, "Linha %d: Erro semântico: divisão por zero\n", no->lineno);
                        return 0;
                    }
                    return val_esq / val_dir;
                
                // --- OPERADORES DE COMPARAÇÃO ADICIONADOS AQUI ---
                // Retornam 1 para verdadeiro, 0 para falso
                case '>':  return val_esq > val_dir;
                case '<':  return val_esq < val_dir;
                case GE: return val_esq >= val_dir;
                case LE: return val_esq <= val_dir;
                case EQ: return val_esq == val_dir;
                case NE: return val_esq != val_dir;

                default:
                    interpret_error = 1;
                    fprintf(stderr, "Linha %d: Erro semântico: operador desconhecido '%c'\n", no->lineno, no->op);
                    return 0;
            }
        }
        
        // --- LÓGICA DO IF-ELSE ADICIONADA AQUI ---
        case NODE_TYPE_IF: {
            // 1. Avalia a condição
            int condicao_val = interpretar(no->data.if_details.condicao);
            if (interpret_error) return 0;

            // 2. Em C, qualquer valor diferente de 0 é 'verdadeiro'
            if (condicao_val) {
                // 3. Se a condição for verdadeira, executa o bloco 'then'
                return interpretar(no->data.if_details.bloco_then);
            } 
            // 4. Caso contrário, se existir um bloco 'else', executa ele
            else if (no->data.if_details.bloco_else != NULL) {
                return interpretar(no->data.if_details.bloco_else);
            }
            return 0; // Se a condição for falsa e não houver 'else', o comando termina.
        }

        default:
            interpret_error = 1;
            fprintf(stderr, "Erro interno: nó inválido na AST (tipo %d)\n", no->type);
            return 0;
    }
}