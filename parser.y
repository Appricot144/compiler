%{
/* C の宣言部 */
#include "node.h"
#include "Symbol_Table.h"

node_Program *Program;      //rootノード 最終的なAST
variable_Table var_Table;  //変数の記号表

extern char *input_Filename;
extern FILE *yyin;
extern int yylex();
void yyerror(const char* msg){fprintf(stderr, "Parser : %s", msg);}
%}


/* bison の宣言部 */
// 意味値の型とその識別子
// node.hで定義したクラスの型を書く
%union{
  Node          *node;
  node_Program  *program;
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
  std::vector<node_Variable_Declaration*>  *var_decl_list;
  std::vector<node_Function_Declaration*>  *func_decl_list;
  std::vector<node_Function*>     *func_defi_list;
  std::string   *string;
  int t_int;
  double t_double;
  char t_char;
}

// トークン型の宣言
%token IF ELSE RETURN TY_INT
%token <t_int>      INT
%token <t_double>   DOUBLE
%token <t_char>     CHAR
%token <string>       IDENTIFIER

// 非終端記号の宣言
%type <program> program;
%type <block> block;
%type <stmt_list> stmt_list;
%type <stmt> stmt;
%type <func_decl_list> func_decl_list;
%type <func_decl> func_decl;
%type <var_decl_list> func_args_decl;
%type <func_defi_list> func_defi_list;
%type <func_defi> func_defi;
%type <var_decl> var_decl;
%type <expr_list> call_args;
%type <string> ident;
%type <expr> expr factor;
%type <ret> return;

// 結合性と優先順位の指定
%nonassoc '='
%left '+' '-'
%left '*' '/'

// 開始記号
%start module


%%
/* 文法規則 */
// 各文法規則は、その規則を認識したとき、Nodeを生成して$$に代入する。
// 各規則で生成されるNodeはNode.hで定義されるインスタンスで、
// 各規則におけるノードを表している。

// 宣言時に使える型はintのみ
// 数値としてはint double charが使える
module : program  { Program = $1;}
       ;

program : func_decl_list func_defi_list { $$ = new node_Program(*$1, *$2); }
        | func_defi_list                { $$ = new node_Program(*$1); }
        ;

func_decl_list : func_decl                { $$ = new std::vector<node_Function_Declaration*>; $$->push_back($1); }
               | func_decl_list func_decl { $1->push_back($2); }
               ;

func_defi_list : func_defi                { $$ = new std::vector<node_Function*>; $$->push_back($1); }
               | func_defi_list func_defi { $1->push_back($2); }
               ;

block : '{' stmt_list '}' { $$ = new node_Block(*$2); }
      ;

stmt_list : stmt            { $$ = new std::vector<node_Statement*>; $$->push_back($1); }
          | stmt_list stmt  { $1->push_back($2); }
          ;

stmt : var_decl             { $$ = $1; }
     | expr                 { $$ = $1; }
   /*| if_stmt              {}*/
     | return               { $$ = $1; }
     ;

func_decl : TY_INT ident '(' func_args_decl ')' { $$ = new node_Function_Declaration($2, *$4); }
          | TY_INT ident '(' ')'                { $$ = new node_Function_Declaration($2); }
          ;

func_defi : func_decl block { $$ = new node_Function($1,$2);}
          ;

func_args_decl : var_decl                     { $$ = new std::vector<node_Variable_Declaration*>; $1->set_Type(node_Variable_Declaration::param); $$->push_back($1); }
               | func_args_decl ',' var_decl  { $3->set_Type(node_Variable_Declaration::param); $1->push_back($3); }
               ;

var_decl : TY_INT ident  { $$ = new node_Variable_Declaration($2); }
         ;

//if_stmt : IF '(' expr ')' block             {}
//        | IF '(' expr ')' block ELSE block  {}
//        ;

return : RETURN expr  { $$ = new node_Return($2); }
       ;

call_args :                     { $$ = new std::vector<node_Expression*>; }
          | expr                { $$ = new std::vector<node_Expression*>; $$->push_back($1); }
          | call_args ',' expr  { $1->push_back($3); }
          ;

ident : IDENTIFIER  { $$ = $1; }
      ;

expr : factor                   { $$ = $1; }
     | ident '(' call_args ')'  { $$ = new node_Function_Call($1, *$3); }
     | ident '=' expr           { $$ = new node_Binary_Operator($<t_char>2, var_Table.get_Tuple(*$1), $3); }
     | expr '+' expr            { $$ = new node_Binary_Operator($<t_char>2, $1, $3); }
     | expr '-' expr            { $$ = new node_Binary_Operator($<t_char>2, $1, $3); }
     | expr '*' expr            { $$ = new node_Binary_Operator($<t_char>2, $1, $3); }
     | expr '/' expr            { $$ = new node_Binary_Operator($<t_char>2, $1, $3); }
     | ident                    { $$ = new node_Variable(*$1); puts("ident");}
     ;

factor : INT            { $$ = new node_Integer($1); }
       | DOUBLE         { $$ = new node_Double($1); }
       | CHAR           { $$ = new node_Char($1); }
       | '(' expr ')'   { $$ = $2; }
       ;

%%
