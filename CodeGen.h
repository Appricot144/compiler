//　コード生成するクラス
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/Instructions.h>
#include <llvm/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/ModuleProvider.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>

// Program（翻訳単位）一つにつき一つのコード生成メソッドを生成する。
// Programノードを深さ優先で探索し、LLVM-IRを生成する。
class CodeGen{

  private:
    // 現在コード生成中のFunction
    Function *Current_Func;
    // 生成したModule格納
    Module *Mod;
    // LLVM-IRを生成するIRBuilderクラス
    IRBuilder<> *Builder;

  public:
    CodeGen(){}
    ~CodeGen(){}

    bool do_CodeGen(node_Program &Program, std::string name);
    Module &get_Module();

  private:
    bool generate_Program(node_Program &program, std::string name);
    Function* generate_Prototype(node_Function_Declaration *proto, Module *mod);
    Function* generate_Function(node_Function *func, Module *mod);
    Value* generate_Block(node_Block *block);
};
