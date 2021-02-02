%{
/* C の宣言文 */
#include <stdio.h>
#include <ctype.h>
#include <math.h>

int yylex(void);
void yyerror(const char* s);
%}

/* bison の宣言文 */

// 型とその識別子
%union{
  int int_val;
  double double_val;
  char char_val;
}

%token <int_val>      INT_LITERAL
%token <double_val>   DOUBLE_LITERAL
%token <char_val>     CHAR_LITERAL
%token                OPERATOR_ADD
                      OPERATOR_SUB
                      OPERATOR_MUL
                      OPERATOR_DIV
                      EOL


%%
/* 文法規則 */


%%

/* C の追加コード */
