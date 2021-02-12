%{
/* C の宣言部 */
#include "node.h"
node_Program *Program;  //rootノード 最終的なAST

// extern Table Symbol_table     //仮で記号表を宣言しておく 後で直されたし
extern int yylex();
void yyerror(const char* msg){fprintf(stderr, "Parser : %s", msg);}
%}


/* bison の宣言部 */
// 意味値の型とその識別子
// node.hで定義したクラスの型を書く
%union{
  Node          *node;
  node_Function *func_defi;
  node_Function_Declaration *func_decl;
  node_Block    *block;
  node_Variable_Declaration *var_decl;
  node_Statement *stmt;
  node_Expression *expr;
  node_Integer  *int_val;
  node_Double   *double_val;
  node_Char     *char_val;
  node_Variable *var;
  node_Function_Call    *func_call;
  node_Binary_Operator  *bin_op;
  node_Return           *ret;
  std::vector<node_Statement*>    *stmt_list;
  std::vector<node_Expression*>   *expr_list;
  std::vector<node_Variable*>     *var_list;
  std::vector<node_Function_Declaration*>  *func_decl_list;
  std::vector<node_Function*>     *func_defi_list;
  std::string   *string;
  int t_int;
  double t_double;
  char t_char;
}

// トークン型の宣言
%token IF ELSE RETURN
%token <int_val>      INT
%token <double_val>   DOUBLE
%token <char_val>     CHAR
%token <string>       IDENTIFIER

// 非終端記号の宣言
%type <block> block;
%type <stmt_list> stmt_list;
%type <stmt> stmt;
%type <func_decl_list> func_decl_list;
%type <func_decl> func_decl;
%type <var_list> func_args_decl;
%type <func_defi_list> func_defi_list;
%type <func_defi> func_defi;
%type <var_decl> var_decl;
%type <expr_list> call_args;
%type <string> ident;
%type <expr> expr factor;

// 結合性と優先順位の指定
%left '+' '-'
%left '*' '/'

// 開始記号
%start program


%%
/* 文法規則 */
// 各文法規則は、その規則を認識したとき、Nodeを生成して$$に代入する。
// 各規則で生成されるNodeはNode.hで定義されるインスタンスで、
// 各規則におけるノードを表している。

// 宣言時に使える型はintのみ
// 数値としてはint double charが使える
program : func_decl_list func_defi_list {}
        | func_defi_list                {}
        ;

func_decl_list : func_decl                {}
               | func_decl_list func_decl {}
               ;

func_defi_list : func_defi                {}
               | func_defi_list func_defi {}
               ;

block : '{' stmt_list '}' {}
      ;

stmt_list : stmt            {}
          | stmt_list stmt  {}
          ;

stmt : var_decl             {}
     | expr                 {}
   /*| if_stmt              {}*/
     | return               {}
     ;

func_decl : "int" ident '(' func_args_decl ')' {}
          ;

func_defi : "int" ident '(' func_args_decl ')' block {}
          ;

func_args_decl :                              {}
               | var_decl                     {}
               | func_args_decl ',' var_decl  {}
               ;

var_decl : "int" ident  {}
         ;

//if_stmt : IF '(' expr ')' block             {}
//        | IF '(' expr ')' block ELSE block  {}
//        ;

return : RETURN expr  {}
       ;

call_args :                     {}
          | expr                {}
          | call_args ',' expr  {}
          ;

ident : IDENTIFIER  {}
      ;

expr : factor                   { $$ = $1; }
     | ident '(' call_args ')'  {}
     | ident '=' expr           {}
     | expr '+' expr            {}
     | expr '-' expr            {}
     | expr '*' expr            {}
     | expr '/' expr            {}
     | ident                    {}
     ;

factor : INT            { $$ = $1; }
       | DOUBLE         { $$ = $1; }
       | CHAR           { $$ = $1; }
       | '(' expr ')'   { $$ = $2; }
       ;

%%
