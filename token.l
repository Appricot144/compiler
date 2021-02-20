%{
#include "node.h"
#include "parser.hpp"

extern "C" int yywrap() { }

#define PUT { printf("%s\n", yytext); }

// integerはint型の数値を返す
// floatingはdouble型の数値を返す
// charactorはchar型の1文字を返す
// identifierはstring型の文字列を返す
// 二項演算子はstring型の文字を返す
%}

space       [ \t\n]+
floating    [0-9]+\.[0-9]+
integer     [0-9]+
charactor   \'[a-zA-Z_0-9]\'
operator    [=+\-*/]
identifier  [a-zA-Z_][a-zA-Z_0-9]*
other       .

%%

"if"          { return IF; }
"else"        { return ELSE; }
"return"      { PUT; return RETURN; }
"int"         { PUT; return TY_INT; }
"{"           { PUT; return '{'; }
"}"           { PUT; return '}'; }
"("           { PUT; return '('; }
")"           { PUT; return ')'; }
{space}       { }
{operator}    { return yytext[0]; }
{integer}     { PUT; yylval.t_int = std::atoi(yytext); return INT; }
{floating}    { yylval.t_double = std::atof(yytext); return DOUBLE; }
{charactor}   { yylval.t_char = yytext[1]; return CHAR; }
{identifier}  { PUT; yylval.string = new std::string(yytext, yyleng); return IDENTIFIER; }
";"           ;
$+            ;
{other}       { fprintf(stderr, "Unknown token : %s\n", yytext); }

%%
