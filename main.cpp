#include "node.h"
#include "CodeGen.h"
#include "parser.hpp"

extern FILE *yyin;
extern node_Program *Program;
extern int yyparse();

int main(int argc, char **argv)
{
  std::string input_Filename;
  std::string output_Filename;
  //入力、出力ファイル名の取得
  if(argc < 3){
    fprintf(stderr,"引数が足りません:%d\n", argc);
    exit(1);
  }
  input_Filename = argv[1];
  output_Filename = argv[2];

  /*
  InitializeNativeTarget();
  sys::PrintStackTraceOnErrorSignal(argv[0]);
  PrettyStackTraceProgram X(argc, argv);
  EnableDebugBuffering = true;
  */

  //構文解析
  yyin = fopen(argv[1], "r");
  yyparse();
  if(Program->empty()){
    fprintf(stderr,"Program is empty\n");
    exit(1);
  }
  //コード生成
  CodeGen *codegen = new CodeGen();
  if(!codegen->do_CodeGen(Program, input_Filename)){
    fprintf(stderr, "error at CodeGen\n");
    SAFE_DELETE(codegen);
    exit(1);
  }
  Module &mod=codegen->get_Module();
  if(mod.empty()){
    fprintf(stderr, "Module is empty\n");
    SAFE_DELETE(codegen);
    exit(1);
  }

  legacy::PassManager pm;

  //出力
  std::error_code error;
  raw_fd_ostream raw_stream(output_Filename, error);
  pm.add(createPrintModulePass(raw_stream));
  pm.run(mod);
  raw_stream.close();

  SAFE_DELETE(codegen);

  return 0;
}
