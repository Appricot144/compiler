#include <iostream>
#include "node.h"
extern node_Program* *Program;
extern int yyparse();

int main(int argc, char **argv)
{
    yyparse();
    std::cout << programBlock << std::endl;
    return 0;
}
