%{
#include "parser.tab.h"
#include<stdio.h>

%}

space       [ \t]
floating    [0-9]+\.[0-9]+
integer     [0-9]+
symbol      [=+\-\^*/();\n]
letter      [a-zA-Z]
other       .

%%
{space}+
{symbol}    { return(yytext[0]); }
{integer}   { }
{floating}  { }
{symbol}    { }
{letter}    { }
if          { return(IF); }
else        { return(ELSE); }
for         { return(FOR); }
{other}     { fprintf(stderr, "Unknown token : %c\n", yytext[0]); }

%%
