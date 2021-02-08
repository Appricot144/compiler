%{
/* C の宣言部 */
#include "node.h"

exetern int yylex();
void yyerror(const char* s);
%}


/* bison の宣言部 */
// 意味値の型とその識別子
%union{
  int     int_val;
  double  double_val;
  char    char_val;
  char    *identifier;
}

// トークン型の宣言
%token IF ELSE FOR
%token <int_val>      INT
%token <double_val>   DOUBLE
%token <char_val>     CHAR
%token <identifier>   IDENTIFIER

// 非終端記号の宣言
%type block stmt expr term factor

// 結合性と優先順位の指定

// 開始記号
%start program


%%
/* 文法規則 */
// 各文法規則は、その規則を認識したとき、Nodeを生成して$$に代入する。
// 各規則で生成されるNodeはNode.hで定義されるインスタンスで、
// 各規則におけるrootノードを表している。
program :               { $$ = $1; }
        | block         { }
        ;

block : stmt
      ;

stmt : expr             { $$ = $1; }
     |
     ;

expr : term                 { $$ = $1; }
     | IDENTIFIER '=' expr  { $$ =  }
     | expr + expr          { $$ = $1 + $3; }
     | expr - expr          { $$ = $1 - $3; }
     ;

term : factor           { $$ = $1; }
     | term * term      { $$ = $1 * $3; }
     | term / term      { $$ = $1 / $3; }
     | term ^ term      { $$ = pow($1, $3); }
     ;

factor : INT            { $$ = $1; }
       | DOUBLE         { $$ = (double)$1; }
       | CHAR           { $$ = (char)$1; }
       | IDENTIFIER     { $$ =  }
       | '(' expr ')'   { $$ = $2; }
       ;

%%


/* C の追加コード */
int main(void){

  if(yyparse() == 0)
    printf("Parse : success \n");
  else
    printf("Parse : fail \n");

  return 0;
}

int yyerror(char *msg){
  fprintf(stderr, "Parser : %s", msg);
  return 0;
}
