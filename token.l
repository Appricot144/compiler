%{
#include <string>
#include "node.h"
#include "parser.hpp"

%}

space       [ \t\n]
floating    [0-9]+\.[0-9]+
integer     [0-9]+
charactor   \'[a-zA-Z_0-9]\'
symbol      [=+\-*/()]
identifier  [a-zA-Z_][a-zA-Z_0-9]*
other       .

%%

// integerはint型の数値を返す
// floatingはdouble型の数値を返す
// charactorはchar型の1文字を返す
// identifierはstring型の文字列を返す
{space}+      ;
{symbol}      { return(yytext[0]); }
{integer}     { yylval.int = std::atoi(yytext); return INT; }
{floating}    { yylval.double = std::atof(yytext); return DOUBLE; }
{charactor}   { yylval.char = yytext[1]; return CHAR; }
{identifier}  { yylval.string = new std::string(yytext, yyleng); return IDENTFIER; }
";"           ;
if            { return(IF); }
else          { return(ELSE); }
return        { return(RETURN); }
{other}       { fprintf(stderr, "Unknown token : %c\n", yytext[0]); }

%%
