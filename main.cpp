#include "node.h"
#include "CodeGen.h"
#include "parser.hpp"

FILE *yyin;
extern node_Program *Program;
extern int yyparse();

int main(int argc, char **argv)
{
  const char *input_Filename;
  std::string output_Filename;
  //入力、出力ファイル名の取得
  if(argc < 3){
    fprintf(stderr,"引数が足りません:%d\n", argc);
    exit(1);
  }
  input_Filename = argv[1];
  output_Filename = argv[2].c_str();

  InitializeNativeTarget();
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  EnableDebugBuffering = true;

  //構文解析
  yyin = fopen(input_Filename, "r");
  yyparse();
  if(Program->empty()){
    fprintf(stderr,"Program is empty\n");
    exit(1);
  }
  //コード生成
  CodeGen *codegen = new CodeGen();
  if(codegen->doCodeGen(Program, input_Filename.c_str())){
    fprintf(stderr, "error at CodeGen\n");
    SAFE_DELETE(codegen);
    exit(1);
  }
  Module &mod=codegen->getModule();
  if(mod.empty()){
    fprintf(stderr, "Module is empty\n");
    SAFE_DELETE(codegen);
    exit(1);
  }

  PassManager pm;

  //出力
  std::string error;
  raw_fd_ostream raw_stream(output_Filename, error);
  pm.add(createPrintModulePass(&raw_stream));
  pm.run(mod);
  raw_stream.close();

  SAFE_DELETE(codegen);
  return 0;
}
